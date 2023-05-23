#include <debug.h>
#include <string.h>
#include "cache.h"
#include "filesys/filesys.h"


int get_cache_item_index(block_sector_t sector);
cache_block_item *get_cache_block_item(struct block *fs_device, block_sector_t sector);
void flush_cache_item(struct block *fs_device, struct cache_block_item *cache_item);

void
cache_emptying(void) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        lock_init(&cache[i].cache_item_lock);
        list_push_back(&cache_used_list, &(cache[i].cache_elem));
    }

}

void
cache_init(void) {
    lock_init(&cache_list_lock);
    list_init(&cache_used_list);
    cache_emptying();
}

void
cache_down() {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].valid && cache[i].dirty) {
            flush_cache_item(fs_device, &cache[i]);
            cache[i].valid = false;
        }

    }
}

uint16_t
get_num_writes ()
{
    return cache_read_results.write_num;
}

uint32_t
get_hit ()
{
    if (cache_read_results.hit + cache_read_results.miss == 0)
    {
        return 0;
    }
    return cache_read_results.hit / (cache_read_results.hit + cache_read_results.miss);
}

void
reset_cache ()
{
    cache_read_results = (cache_results) {0, 0, 0};
}

void
cache_read(struct block *fs_device, block_sector_t sector, void *buffer, int offset, int size) {
    struct cache_block_item *cache_item = get_cache_block_item(fs_device, sector);

    lock_acquire(&cache_item->cache_item_lock);
    memcpy(buffer,  &(cache_item->data[offset]), size); // check address
    lock_release(&cache_item->cache_item_lock);
}

cache_block_item *
get_cache_block_item(struct block *fs_device, block_sector_t sector) {
    int cache_item_index = get_cache_item_index(sector);
    lock_acquire(&cache_list_lock);
    if (cache_item_index != -1) {
        list_remove(&cache[cache_item_index].cache_elem);
        list_push_back(&cache_used_list, &cache[cache_item_index].cache_elem);
        cache_read_results.hit += 1;
        lock_release(&cache_list_lock);
        return &cache[cache_item_index];
    } else {
        struct cache_block_item *cache_item = list_entry(list_pop_front(&cache_used_list), struct cache_block_item, cache_elem);
        lock_acquire(&cache_item->cache_item_lock);
        if (cache_item->valid && cache_item->dirty) {
            flush_cache_item(fs_device, cache_item);
            cache_read_results.write_num += 1;
        }
        block_read (fs_device, sector, cache_item->data);
        cache_item->valid = true;
        cache_item->dirty = false;
        cache_item->sector_index = sector;
        list_push_back(&cache_used_list, &cache_item->cache_elem);
        cache_read_results.miss += 1;
        lock_release(&cache_item->cache_item_lock);
        lock_release(&cache_list_lock);
        return cache_item;
    }
}

void
cache_write(struct block *fs_device, block_sector_t sector, void *buffer, int offset, int size) {
    struct cache_block_item *cache_item = get_cache_block_item(fs_device, sector);

    lock_acquire(&cache_item->cache_item_lock);
    memcpy(&(cache_item->data[offset]), buffer, size);
    cache_item->dirty = true;
    lock_release(&cache_item->cache_item_lock);
}

void
flush_cache_item(struct block *fs_device, struct cache_block_item *cache_item) {
    block_write(fs_device, cache_item->sector_index, cache_item->data);
    cache_item->dirty = false;
    cache_item->valid = false;
}

int
get_cache_item_index(block_sector_t sector) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].valid && cache[i].sector_index == sector) {
            return i;
        }
    }
    return -1;
}

void
clear_cache_if_needed(block_sector_t sector) {
//    int cache_index = get_cache_item_index(sector);
//    if (cache_index != -1) {
//        struct cache_block_item cache_item = cache[cache_index];
//        lock_acquire(&cache_item.cache_item_lock);
//        cache_item.valid = false;
//        lock_release(&cache_item.cache_item_lock);
//    }
return;
}