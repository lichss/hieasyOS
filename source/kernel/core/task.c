#include "core/task.h"
#include "tools/klib.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "cpu/cpu.h"
#include "comm/cpu_instr.h"

static task_manager_t task_manager;
    /* 操作系统的 */
static int tss_init (task_t * task, uint32_t entry, uint32_t esp) {
    // 为TSS分配GDT
    int tss_sel = gdt_alloc_desc();
    if (tss_sel < 0) {
        log_printf("alloc tss failed.\n");
        return -1;
    }

    segment_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t), 
            SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS);

    // tss段初始化
    kernel_memset(&task->tss, 0, sizeof(tss_t));

    task->tss.eip = entry;
    task->tss.esp = task->tss.esp0 = esp;
    task->tss.ss0 = KERNEL_SELECTOR_DS;
    task->tss.eip = entry;
    task->tss.eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    task->tss.es = task->tss.ss = task->tss.ds
            = task->tss.fs = task->tss.gs = KERNEL_SELECTOR_DS;   // 暂时写死
    task->tss.cs = KERNEL_SELECTOR_CS;    // 暂时写死
    task->tss.iomap = 0;

    task->tss_sel = tss_sel;
    return 0;
}





int task_init(task_t* task,uint32_t entry,uint32_t esp){
    ASSERT(task!= 0);
    tss_init(task,entry,esp);

    // uint32_t* pesp = (uint32_t*) esp; 
    // if(pesp){
    //     *(--pesp) = entry;

    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     task->stack = pesp;
    // }
    return 0;
}
/* 这个函数的具体实现在 kernel/init/start.s 里 */
void simple_switch(uint32_t** from,uint32_t* to);

void task_switch_from_to(task_t *form, task_t* to){
    switch_to_tss(to->tss_sel); 
    // simple_switch(&form->stack,to->stack);
}


void task_first_init(void){
    task_init(&task_manager.first_task,(uint32_t)0,0);
    write_tr(task_manager.first_task.tss_sel);
    task_manager.curr_task = &task_manager.first_task;
}
task_t* task_first_task(void){
    return &task_manager.first_task;
}
void task_manager_init(){
    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);
    task_manager.curr_task = 0;

}