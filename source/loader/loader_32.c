#include "loader.h"
#include "elf.h"
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
/* 艹了 这个函数用来干嘛的
 * 想读懂这个函数 就需要对elf文件有一定的了解
 * 这个函数 elf_load 的主要作用是加载一个 ELF（Executable and Linkable Format）格式的可执行文件到内存中，
 * 并返回该文件的入口点地址。
 * ELF 是一种常见的可执行文件格式
*/
static uint32_t elf_load(void* file_buffer){
    Elf32_Ehdr* elf_header = (Elf32_Ehdr* )file_buffer;
    if(elf_header->e_ident[0] != 0x7F || elf_header->e_ident[1] != 'E' || 
        elf_header->e_ident[2] != 'L' || elf_header->e_ident[3] != 'F'){            
        return 0;
    }
    /* 程序头表（Program Header Table）：描述了如何将文件中的段（Segment）加载到内存中。每个条目对应一个段。 */
    /* elf_header->e_phnum 表示程序头表的数量 */
    for(int i=0;i<elf_header->e_phnum;i++){
        Elf32_Phdr* phdr = (Elf32_Phdr*)(file_buffer + elf_header->e_phoff ) + i;
        /* phdr 就是程序头表了 */
        if(phdr->p_type!=PT_LOAD)
            continue;
        
        uint8_t* src = (uint8_t*)file_buffer + phdr->p_offset;
        uint8_t* dst = (uint8_t*)phdr->p_paddr;

            /* 下面是针对.rodata的处理*/
        for(int j=0;j<phdr->p_filesz;j++)
            *dst++ = *src++;
        
        dst = (uint8_t*)phdr->p_paddr + phdr->p_filesz;
        for(int j=0;j<phdr->p_memsz - phdr->p_filesz;j++)
            *dst++ = 0;
    }

    return elf_header->e_entry;
}

void loader_anomi_entry(void) {
    read_disk(100,500,(void*)KERNEL_ADDR);
    uint32_t entry = elf_load((void*)KERNEL_ADDR); 
    if(entry == 0){
        while(1){
            ;
        }
    }

    # if 1
    void (*call_kernel)(boot_info_t* ) = (void(*)(boot_info_t*))entry;
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