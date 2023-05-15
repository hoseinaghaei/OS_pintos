#include "threads/synch.h"
#include "lib/kernel/list.h"
#include "devices/block.h"

#define CACHE_SIZE 64

typedef struct cache_block_item
    {
        bool valid;
        bool dirty;
        char data[BLOCK_SECTOR_SIZE];
        struct list_elem cache_elem;
        struct lock cache_item_lock;
    } cache_block_item;

cache_block_item cache[CACHE_SIZE];
struct list cache_used_list;
struct lock cache_list_lock;

// initializing the cache for using
void cache_init(void);
