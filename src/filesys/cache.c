#include "cache.h"
#include "filesys/filesys.h"

void 
cache_emptying (void)
{
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        lock_init(&cache[i].cache_item_lock);
        cache[i].dirty = false;
        cache[i].valid = false;
        list_push_back (&cache_used_list, &(cache[i].cache_elem));
    }
    
}

void
cache_init (void)
{
    lock_init(&cache_list_lock);
    list_init(&cache_used_list);
    cache_emptying();
}

void 
cache_down ()
{
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if (cache[i].valid && cache[i].dirty)
        {
            flush_item();
        }
        
    }
    
}

void 
flush_item ()
{
    //flush data to disk
}