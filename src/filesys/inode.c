#include "filesys/inode.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"
#include "filesys/cache.h"

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44

bool allocate_sector (block_sector_t *sector_index);
bool extend_inode(struct inode *inode, int sectors);
void deallocate_direct_indirect_sectors(struct inode_disk *inode_disk, int sector_count);
void deallocate_direct_sectors(struct inode_disk *inode_disk, int sector_count);
void deallocate_indirect_sectors(block_sector_t indirect, int sector_count);
bool allocate_direct_indirect_sectors(struct inode_disk *disk_inode, int sector_count);
bool allocate_direct_sectors(struct inode_disk *inode_disk, int sector_count);
bool allocate_indirect_sectors(block_sector_t indirect, int sector_count);


/* Returns the number of sectors to allocate for an inode SIZE
   bytes long. */
static inline size_t
bytes_to_sectors (off_t size)
{
    return DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}

/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */
static block_sector_t
byte_to_sector(const struct inode *inode, off_t pos) {
    ASSERT (inode != NULL);

    if (pos >= inode->data.length) {
        return -1;
    }
    int sector_index = pos / BLOCK_SECTOR_SIZE;
    if (sector_index < DIRECT_SECTORS) {
        return inode->data.direct[sector_index];
    }
    sector_index -= DIRECT_SECTORS;
    if (sector_index < INDIRECT_SECTORS) {
        block_sector_t indirect_blocks[INDIRECT_SECTORS];
        cache_read(fs_device, inode->data.indirect, &indirect_blocks, 0, BLOCK_SECTOR_SIZE);
        return indirect_blocks[sector_index];
    }
    sector_index -= INDIRECT_SECTORS;
    if (sector_index < INDIRECT_SECTORS * INDIRECT_SECTORS){
        block_sector_t indirect_blocks[INDIRECT_SECTORS];
        cache_read(fs_device, inode->data.double_indirect, &indirect_blocks, 0, BLOCK_SECTOR_SIZE);

        int first_pointer_index = sector_index / INDIRECT_SECTORS;
        block_sector_t double_indirect_blocks[INDIRECT_SECTORS];
        cache_read(fs_device, indirect_blocks[first_pointer_index], &double_indirect_blocks, 0, BLOCK_SECTOR_SIZE);

        return double_indirect_blocks[sector_index % INDIRECT_SECTORS];
    }
    return -1;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

/* Initializes the inode module. */
void
inode_init (void)
{
    list_init (&open_inodes);
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
bool
inode_create (block_sector_t sector, off_t length)
{
    struct inode_disk *disk_inode = NULL;
    bool success = false;

    ASSERT (length >= 0);

    /* If this assertion fails, the inode structure is not exactly
       one sector in size, and you should fix that. */
    ASSERT (sizeof *disk_inode == BLOCK_SECTOR_SIZE);

    disk_inode = calloc(1, sizeof *disk_inode);
    if (disk_inode != NULL) {
        size_t sectors = bytes_to_sectors(length);
        disk_inode->length = length;
        disk_inode->magic = INODE_MAGIC;
        if (allocate_direct_indirect_sectors(disk_inode, sectors)) {
            success = true;
            cache_write(fs_device, sector, disk_inode, 0, BLOCK_SECTOR_SIZE);
        }

        free(disk_inode);
    }
    return success;
}

/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *
inode_open (block_sector_t sector)
{
    struct list_elem *e;
    struct inode *inode;

    /* Check whether this inode is already open. */
    for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
         e = list_next (e))
    {
        inode = list_entry (e, struct inode, elem);
        if (inode->sector == sector)
        {
            inode_reopen (inode);
            return inode;
        }
    }

    /* Allocate memory. */
    inode = malloc (sizeof *inode);
    if (inode == NULL)
        return NULL;

    /* Initialize. */
    list_push_front (&open_inodes, &inode->elem);
    inode->sector = sector;
    inode->open_cnt = 1;
    inode->deny_write_cnt = 0;
    inode->removed = false;
    cache_read(fs_device, inode->sector, &inode->data, 0 , BLOCK_SECTOR_SIZE);
    return inode;
}

/* Reopens and returns INODE. */
struct inode *
inode_reopen (struct inode *inode)
{
    if (inode != NULL)
        inode->open_cnt++;
    return inode;
}

/* Returns INODE's inode number. */
block_sector_t
inode_get_inumber (const struct inode *inode)
{
    return inode->sector;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
void
inode_close (struct inode *inode)
{
    /* Ignore null pointer. */
    if (inode == NULL)
        return;

    /* Release resources if this was the last opener. */
    if (--inode->open_cnt == 0)
    {
        /* Remove from inode list and release lock. */
        list_remove (&inode->elem);

        /* Deallocate blocks if removed. */
        if (inode->removed)
        {
            free_map_release (inode->sector, 1);
            deallocate_direct_indirect_sectors(&inode->data, inode->data.length / BLOCK_SECTOR_SIZE + 1);

        }

        free (inode);
    }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void
inode_remove (struct inode *inode)
{
    ASSERT (inode != NULL);
    inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t
inode_read_at (struct inode *inode, void *buffer_, off_t size, off_t offset)
{
    uint8_t *buffer = buffer_;
    off_t bytes_read = 0;

    while (size > 0)
    {
        /* Disk sector to read, starting byte offset within sector. */
        block_sector_t sector_idx = byte_to_sector (inode, offset);
        int sector_ofs = offset % BLOCK_SECTOR_SIZE;

        /* Bytes left in inode, bytes left in sector, lesser of the two. */
        off_t inode_left = inode_length (inode) - offset;
        int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
        int min_left = inode_left < sector_left ? inode_left : sector_left;

        /* Number of bytes to actually copy out of this sector. */
        int chunk_size = size < min_left ? size : min_left;
        if (chunk_size <= 0)
            break;
        cache_read (fs_device, sector_idx, (void *)(buffer + bytes_read), sector_ofs, chunk_size);

        /* Advance. */
        size -= chunk_size;
        offset += chunk_size;
        bytes_read += chunk_size;
    }

    return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t
inode_write_at (struct inode *inode, const void *buffer_, off_t size,
                off_t offset)
{
    const uint8_t *buffer = buffer_;
    off_t bytes_written = 0;

    if (inode->deny_write_cnt)
        return 0;

    if (inode->data.length < offset + size) {
        if (!extend_inode(inode, offset + size)) {
            lock_release(&inode->ilock);
            return 0;
        }
    }

    while (size > 0)
    {
        /* Sector to write, starting byte offset within sector. */
        block_sector_t sector_idx = byte_to_sector (inode, offset);
        int sector_ofs = offset % BLOCK_SECTOR_SIZE;

        /* Bytes left in inode, bytes left in sector, lesser of the two. */
        off_t inode_left = inode_length (inode) - offset;
        int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
        int min_left = inode_left < sector_left ? inode_left : sector_left;

        /* Number of bytes to actually write into this sector. */
        int chunk_size = size < min_left ? size : min_left;
        if (chunk_size <= 0)
            break;
        cache_write (fs_device, sector_idx, (void *)(buffer + bytes_written), sector_ofs, chunk_size);

        /* Advance. */
        size -= chunk_size;
        offset += chunk_size;
        bytes_written += chunk_size;
    }

    return bytes_written;
}

void
deallocate_indirect_sectors(block_sector_t indirect, int sector_count) {
    block_sector_t indirect_sectors[INDIRECT_SECTORS];
    cache_read(fs_device, indirect, indirect_sectors, 0, BLOCK_SECTOR_SIZE);
    for (int i = 0; i < INDIRECT_SECTORS && i < sector_count; i++) {
        if (!indirect_sectors[i]) {
            free_map_release(indirect_sectors[i], 1);
            clear_cache_if_needed(indirect_sectors[i]);
        }
    }
    free_map_release(indirect, 1);
    clear_cache_if_needed(indirect);
}

void
deallocate_direct_sectors(struct inode_disk *inode_disk, int sector_count) {
    for (int i = 0; i < DIRECT_SECTORS && i < sector_count; ++i) {
        if (!inode_disk->direct[i]) {
            free_map_release(inode_disk->direct[i], 1);
            clear_cache_if_needed(inode_disk->direct[i]);
        }
    }
}


void
deallocate_direct_indirect_sectors(struct inode_disk *inode_disk, int sector_count) {
    deallocate_direct_sectors(inode_disk, sector_count);
    sector_count -= DIRECT_SECTORS;
    if (sector_count <= 0)
        return;

    if (!inode_disk->indirect)
        deallocate_indirect_sectors(inode_disk->indirect, sector_count);
    sector_count -= INDIRECT_SECTORS;
    if (sector_count <= 0)
        return;

    block_sector_t indirect_sectors[INDIRECT_SECTORS];
    cache_read(fs_device, inode_disk->double_indirect, indirect_sectors, 0, BLOCK_SECTOR_SIZE);
    for (int i = 0; i < INDIRECT_SECTORS && i < sector_count; i++) {
        if (!indirect_sectors[i]) {
            deallocate_indirect_sectors(indirect_sectors[i], sector_count);
            sector_count -= INDIRECT_SECTORS;
            if (sector_count <= 0)
                break;
        }
    }
    free_map_release(inode_disk->double_indirect, 1);
    clear_cache_if_needed(inode_disk->double_indirect);
}


bool
allocate_sector (block_sector_t *sector_index)
{
    static char zeros[BLOCK_SECTOR_SIZE];
    if (free_map_allocate(1, sector_index))
    {
        cache_write (fs_device, *sector_index, zeros, 0, BLOCK_SECTOR_SIZE);
        return true;
    }
    return false;
}

bool
allocate_indirect_sectors(block_sector_t indirect, int sector_count) {
    block_sector_t indirect_sectors[INDIRECT_SECTORS];
    cache_read(fs_device, indirect, &indirect_sectors, 0, BLOCK_SECTOR_SIZE);
    for (int i = 0; i < INDIRECT_SECTORS && i < sector_count; i++) {
        if (!indirect_sectors[i]) {
            if (!allocate_sector(&indirect_sectors[i])) {
                return false;
            }
        }
    }
    cache_write (fs_device, indirect, &indirect_sectors, 0, BLOCK_SECTOR_SIZE); // check address
    return true;
}

bool
allocate_direct_sectors(struct inode_disk *inode_disk, int sector_count) {
    for (int i = 0; i < DIRECT_SECTORS && i < sector_count; i++) {
        if (!inode_disk->direct[i]) {
            if (!allocate_sector(&inode_disk->direct[i])) {
                return false;
            }
        }
    }
    return true;
}

bool
allocate_direct_indirect_sectors(struct inode_disk *disk_inode, int sector_count) {
    if (!allocate_direct_sectors(disk_inode, sector_count)) {
        return false;
    }
    sector_count -= DIRECT_SECTORS;
    if (sector_count <= 0)
        return true;
    if (!disk_inode->indirect)
        if (!allocate_sector(&disk_inode->indirect))
            return false;

    if (!allocate_indirect_sectors(disk_inode->indirect, sector_count)) {
        return false;
    }

    sector_count -= INDIRECT_SECTORS;
    if (sector_count <= 0)
        return true;
    if (!disk_inode->double_indirect)
        if (!allocate_sector(&disk_inode->double_indirect))
            return false;

    block_sector_t indirect_sectors[INDIRECT_SECTORS];
    cache_read(fs_device, disk_inode->double_indirect, &indirect_sectors, 0, BLOCK_SECTOR_SIZE);
    for (int i = 0; i < INDIRECT_SECTORS && sector_count > 0; i++) {
        if (!indirect_sectors[i])
            if (!allocate_sector(&indirect_sectors[i]))
                return false;
        if (!allocate_indirect_sectors(indirect_sectors[i], sector_count))
            return false;
        sector_count -= INDIRECT_SECTORS;
    }
    if (sector_count <= 0) {
        cache_write (fs_device, disk_inode->double_indirect, &indirect_sectors, 0, BLOCK_SECTOR_SIZE); // check address
        return true;
    }
    return false;
}

bool
extend_inode(struct inode *inode, int size) {
    int sector_count = size / BLOCK_SECTOR_SIZE + 1;
    if (sector_count > (DIRECT_SECTORS + INDIRECT_SECTORS + INDIRECT_SECTORS * INDIRECT_SECTORS)) {
        return false;
    }
    if (allocate_direct_indirect_sectors(&inode->data, sector_count)) {
        inode->data.length = size;
        cache_write(fs_device, inode->sector, &inode->data, 0, BLOCK_SECTOR_SIZE);
        return true;
    }
    return false;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void
inode_deny_write (struct inode *inode)
{
    inode->deny_write_cnt++;
    ASSERT (inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void
inode_allow_write (struct inode *inode)
{
    ASSERT (inode->deny_write_cnt > 0);
    ASSERT (inode->deny_write_cnt <= inode->open_cnt);
    inode->deny_write_cnt--;
}

/* Returns the length, in bytes, of INODE's data. */
off_t
inode_length (const struct inode *inode)
{
    return inode->data.length;
}
