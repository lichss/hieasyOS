/**
 * 好复杂
 * 
 * 
 */
#include "core/memory.h"
#include "tools/log.h"
#include "tools/klib.h"
#include "cpu/mmu.h"

static addr_alloc_t paddr_alloc;
static pde_t kernel_page_dir[PDE_CNT]  __attribute__((aligned(MEM_PAGE_SIZE)));


static void addr_alloc_init(addr_alloc_t* alloc,uint8_t* bits,uint32_t start,uint32_t size, uint32_t page_size){
    mutex_init(&alloc->mutex);
    alloc->page_size = page_size;
    alloc->size = size;
    alloc->start = start;

    bitmap_init(&alloc->bitmap,bits,size/page_size,0);

}

static uint32_t addr_alloc_page(addr_alloc_t* alloc,int page_count){
    uint32_t addr = 0;
    mutex_lock(&alloc->mutex);

    int page_index = bitmap_alloc_nbits(&alloc->bitmap,0,page_count);
    if(page_count>=0){
        addr = alloc->start + page_index * alloc->page_size;
    }

    mutex_unlock(&alloc->mutex);
    return addr;
}
static void addr_free_page(addr_alloc_t* alloc,uint32_t addr,int page_count){
    mutex_lock(&alloc->mutex);
    uint32_t page_index = (addr - alloc->start) / alloc->page_size;
    bitmap_set_bit(&alloc->bitmap,page_index,page_count,0);

    mutex_unlock(&alloc->mutex);
}

void show_mem_info(boot_info_t* boot_info){
    log_printf("mem region");
    for(int i=0;i< boot_info->ram_region_count;i++){
        log_printf("[%d] :0x%x - 0x%x",i,boot_info->ram_region_cfg[i].start,boot_info->ram_region_cfg[i].size);
    }
    log_printf("\n");


}

static uint32_t total_mem_size(boot_info_t* boot_info){

    log_printf("mem init");
    uint32_t mem_size = 0;
    for(int i=0;i< boot_info->ram_region_count;i++){
        mem_size += boot_info->ram_region_cfg[i].size;
    }
    return mem_size;
}

uint32_t memory_create_uvm(void){

}
extern uint8_t s_text[];
extern uint8_t e_text[];
extern uint8_t s_data[];
extern uint8_t kernel_base[];

void create_kernel_table(void){
    static memory_map_t kernel_map[] = {
        {kernel_base,   s_text,  kernel_base,    0 },
        {s_text,  e_text,  s_text,    0 },
        {s_data,  (void*)MEM_EBDA_START,  s_data,    0 },
    };
    for(int i=0;i< sizeof(kernel_map)/sizeof(memory_map_t);i++){
        memory_map_t* map = kernel_map+i;

        uint32_t vstart = down2( (uint32_t)map->vstart,MEM_PAGE_SIZE);
        uint32_t vend = up2( (uint32_t)map->vend,MEM_PAGE_SIZE );
        int page_count = (vend - vstart) / MEM_PAGE_SIZE;

    }

}

void memory_init(boot_info_t* boot_info){
    extern uint32_t* mem_free_start;
    show_mem_info(boot_info);

    uint8_t* mem_free = (uint8_t*)&mem_free_start;
    uint32_t mem_up1MB_free = total_mem_size(boot_info) - MEM_EXT_START;
    mem_up1MB_free = down2(mem_up1MB_free,MEM_PAGE_SIZE);

    addr_alloc_init(&paddr_alloc,mem_free,MEM_EXT_START, mem_up1MB_free,MEM_PAGE_SIZE);
    mem_free += bitmap_byte_count(paddr_alloc.size / MEM_PAGE_SIZE);

    ASSERT(mem_free < (uint8_t*)MEM_EBDA_START);
    create_kernel_table();
}