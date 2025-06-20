#ifndef __TASK_H__
#define __TASK_H__

#include "cpu/cpu.h"
/*
* task_t 这个结构应该类似进程控制块
*
*/
typedef struct _task_t{
    // uint32_t* stack;

    tss_t tss;
    int tss_sel;
}task_t;


int task_init(task_t *,uint32_t entry,uint32_t esp);
void task_switch_from_to(task_t *form, task_t* to);

#endif
