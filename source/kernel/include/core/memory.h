#ifndef MEMORY_H
#define MEMORY_H

#include "comm/types.h"
#include "tools/bitmap.h"
#include "ipc/mutex.h"
#include "comm/boot_info.h"

#define MEM_EXT_START (1024*1024) // 1MB
#define MEM_PAGE_SIZE (4096) // 4KB
#define MEM_EBDA_START (0x80000) // EBDA start address

typedef struct _addr_alloc_t{
    mutex_t mutex;
    bitmap_t bitmap;

    uint32_t start;
    uint32_t size;
    uint32_t page_size;

}addr_alloc_t;

void memory_init(boot_info_t* boot_info);

#endif