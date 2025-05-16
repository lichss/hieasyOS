#include "init.h"
#include "comm/cpu_instr.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"
#include "tools/log.h"
#include "tools/klib.h"
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
}

static task_t first_task;
static uint32_t init_task_stack[1024];     
static task_t init_task;

void init_task_entry(void){
    int count = 0;
    for(;;){
        log_printf("task %d \n",count++);
        task_switch_from_to(&init_task, &first_task);
    }

}

void init_main(void) {


    log_printf("running version:%s","no version\n");

    task_init(&init_task,(uint32_t)init_task_entry,(uint32_t)&init_task_stack[1024]);
    task_init(&first_task,(uint32_t)0,0);
    write_tr(first_task.tss_sel);

    int a=1;
    // irq_enable_global();
    // ASSERT(!a); 
    while(1){
        log_printf("init main:%d",a++);
        task_switch_from_to(&first_task,&init_task);
    }

    return;
}
