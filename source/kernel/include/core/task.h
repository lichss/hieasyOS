#ifndef __TASK_H__
#define __TASK_H__


#include "cpu/cpu.h"
#include "tools/list.h"
#define TASK_NAME_SIZE      32 
#define TASK_TIME_SLICE_DEFAULT 10 
/*
* task_t 
*
*/
typedef struct _task_t{
    // uint32_t* stack;
    enum {
        TASK_CREATED,
        TASK_RUNNING,
        TASK_SLEEP,
        TASK_READY,
        TASK_WAITTING,
    }state;

    char name[TASK_NAME_SIZE];

    int time_tick;
    int slice_tick;

    list_node_t run_node;
    list_node_t all_node;

    tss_t tss;
    int tss_sel;
}task_t;


int task_init(task_t *task,const char* name,uint32_t entry,uint32_t esp);
void task_switch_from_to(task_t *form, task_t* to);

typedef struct _task_manager_t{
    task_t* curr_task;
    list_t ready_list;
    list_t task_list;
    task_t first_task;

}task_manager_t;

void task_time_tick();

void task_manager_init(void);
void task_first_init(void);

task_t* task_first_task(void);

task_t* task_current(void);
task_t* task_next_run(void);

void task_set_ready(task_t* task);

void task_set_block(task_t* task);

int sys_sched_yield(void);

void task_dispatch(void);


#endif
