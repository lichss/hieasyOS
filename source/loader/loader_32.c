#include "loader.h"

// loader_anomi_entry
static int read_disk(uint32_t  sector, uint32_t sector_count, void *buf) {
    const uint8_t slaveBit = 0;
    outb(0x1F6, 0xE0 | (slaveBit << 4));
    outb(0x1F2, (uint8_t)(sector_count >> 8));
    outb(0x1F3, (uint8_t)(sector >> 24));
    outb(0x1F4, (uint8_t)0);
    outb(0x1F5, (uint8_t)0);
    
    outb(0x1F2, (uint8_t) sector_count);
    outb(0x1F3, (uint8_t) sector);
    outb(0x1F4, (uint8_t) (sector >> 8));
    outb(0x1F5, (uint8_t) (sector >> 16));
    
    outb(0x1F7, 0x24);
    uint16_t* databuffer = (uint16_t*)buf;
    while(sector_count--){
        while((inb(0x1F7) & 0x88) != 0x08){};

        for (int i = 0; i < SECTOR_SIZE/2; i++){
            *databuffer++ = inw(0x1F0);
        }
        
    }
    return 0;
}

void loader_anomi_entry(void) {
    read_disk(100,500,(void*)KERNEL_ADDR);
    // void(*)(void)KERNEL_ADDR(); 
    # if 1
    void (*call_kernel)(boot_info_t* ) = (void(*)(boot_info_t*))KERNEL_ADDR;
    call_kernel(&boot_info);    /*jump to assembly*/
    /*
        如果没用使用C显式调用，就会少一条 push 指令
    */
    #else   /*反例 不要解除注释*/
    void (*call_kernel)(void ) = (void(*)(void))KERNEL_ADDR;
    call_kernel();    /*jump to assembly with no arguments*/
    #endif
    /*
        while
        push boot_infoPtr
    
        
    
    */
    while(1){
        ;
    } 

    return ;
} 