#ifndef _CPU_H_
#define _CPU_H_

#include "comm/types.h"

#pragma pack(1)
typedef struct _segment_desc_t{
    uint16_t limit15_0;
    uint16_t base15_0;
    uint8_t base23_16;
    uint16_t attr;
    uint8_t base31_24;

    
} segment_desc_t;

#endif