#include "init.h"
#include "comm/cpu_instr.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"
#include "tools/log.h"
#include "tools/klib.h"
#include "tools/list.h"
#include "core/task.h"

/**
 * 内核入口
 */

void kernel_init (boot_info_t * boot_info) {
    // 初始化CPU，再重新加载
    ASSERT( boot_info->ram_region_count);
    cpu_init();

    log_init();
    irq_init();
    time_init();

    task_manager_init();
}

static uint32_t init_task_stack[1024];     
static task_t init_task;

void init_task_entry(void){
    int count = 0;
    for(;;){
        log_printf("task %d \n",count++);
        task_switch_from_to(&init_task, task_first_task());
    }

}



void init_main(void) {

    log_printf("running version:%s","no version\n");

    task_init(&init_task,"init task",(uint32_t)init_task_entry,(uint32_t)&init_task_stack[1024]);
    task_first_init();

    int a=0;
    // irq_enable_global();
    // ASSERT(!a); 
    while(1){
        log_printf("init main:%d",a++);
        task_switch_from_to(task_first_task(),&init_task);
    }

    return;
}
