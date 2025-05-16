#ifndef __TASK_H__
#define __TASK_H__

#include "cpu/cpu.h"

typedef struct _task_t{
    tss_t tss;
    int tss_sel;
}task_t;


int task_init(task_t *,uint32_t entry,uint32_t esp);
void task_switch_from_to(task_t *form, task_t* to);

#endif
