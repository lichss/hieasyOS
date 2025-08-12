#ifndef __TASK_H__
#define __TASK_H__


#include "cpu/cpu.h"
#include "tools/list.h"
#define TASK_NAME_SIZE              32 
#define TASK_TIME_SLICE_DEFAULT     10 
#define TASK_PRVLEVEL_SYSTEM        (1 << 0) 
/* TASK_PREVLEVEL == TASK_FLAGS_SYS */
#define TASK_PRVLEVEL_APP           (0 << 0)
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
    
    int pid;
    struct _task_t * parent;

    int sleep_ticks;
    int time_ticks;
    int slice_ticks;

    list_node_t run_node;
    list_node_t all_node;
    list_node_t wait_node;

    tss_t tss;
    int tss_sel;
}task_t;


int task_init(task_t *task,const char* name, uint32_t pre_level,uint32_t entry,uint32_t esp);
void task_switch_from_to(task_t *form, task_t* to);

typedef struct _task_manager_t{
    task_t* curr_task;
    list_t ready_list;
    list_t task_list;
    list_t sleep_list;

    task_t first_task;
    task_t idle_task;

    int app_code_sel;
    int app_data_sel;

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
void manager_report(void);

void task_set_sleep(task_t* taks,uint32_t ticks);
void task_set_wakeup(task_t* task);

void sys_sleep(uint32_t ms);
int sys_getpid();
void sys_printmsg(int fmt,int arg);
int sys_fork();
int sys_execve(char* name,char** argv,char* env);
void first_task_entry(void);
#endif
