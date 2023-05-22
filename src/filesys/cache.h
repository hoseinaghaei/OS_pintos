#ifndef FILESYS_CACHE_H
#define FILESYS_CACHE_H

#include "filesys/off_t.h"
#include "devices/block.h"
#include "threads/synch.h"
#include "lib/kernel/list.h"
#define CACHE_SIZE 64

typedef struct cache_block_item {
    block_sector_t sector_index;
    bool valid;
    bool dirty;
    char data[BLOCK_SECTOR_SIZE];
    struct list_elem cache_elem;
    struct lock cache_item_lock;
} cache_block_item;

typedef struct cache_results
{
    uint16_t hit;        /* # of hit */
    uint16_t miss;       /* # of miss */
    uint16_t write_num;   /* # of writes*/
} cache_results;


cache_block_item cache[CACHE_SIZE];
struct list cache_used_list;
struct lock cache_list_lock;
static cache_results cache_read_results;

// initializing the cache for using
void cache_init(void);
void cache_down();
void cache_read(struct block *fs_device, block_sector_t sector, void *buffer, off_t offset, int size);
void cache_write(struct block *fs_device, block_sector_t sector, void *buffer, off_t offset, int size);
void clear_cache_if_needed(block_sector_t sector);
void reset_cache ();
uint32_t get_hit (void);
uint16_t get_num_writes ();
#endif /* filesys/cache.h */
