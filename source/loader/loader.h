#ifndef _LOADER_H_
#define _LOADER_H_


#include "comm/types.h"
#include "comm/boot_info.h"
#include "comm/cpu_instr.h"

void protected_mode_entry(void);

// 内存检测信息结构
typedef struct SMAP_entry {
    uint32_t BaseL; // base address uint64_t
    uint32_t BaseH;
    uint32_t LengthL; // length uint64_t
    uint32_t LengthH;
    uint32_t Type; // entry Type
    uint32_t ACPI; // extended
}__attribute__((packed)) SMAP_entry_t;

#define SECTOR_SIZE     512
#define KERNEL_ADDR    (1024*1024) 

extern boot_info_t boot_info;

#endif