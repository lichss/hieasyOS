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
#include "ipc/sem.h"
#include "core/memory.h"
/**
 * 内核入口
 */

static uint32_t init_task_stack[1024];     
  
static task_t init_task;

static sem_t sem;



void kernel_init (boot_info_t * boot_info) {
    // 初始化CPU，再重新加载
    ASSERT( boot_info->ram_region_count);
    cpu_init();
    log_init();
    // *(uint8_t*)testa = 0x12;
    memory_init(boot_info);
    // *(uint8_t*)testa = 0x56;
    irq_init();
    time_init();

    task_manager_init();
}


void move_to_first_task(){
    task_t* curr = task_current();
    ASSERT(curr != 0);
    tss_t* tss = &(curr->tss);
    asm volatile("jmp *%[ip]"::[ip]"r"(tss->eip));

}



void init_main(void) {

    log_printf("running version:%s","no version\n");
    int a=1;
    int b=2;
    int c=3;
    task_first_init();
    move_to_first_task();

    return;
}
