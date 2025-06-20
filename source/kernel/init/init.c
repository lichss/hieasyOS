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
}

static task_t first_task;       /* 这个东西应该类似进程控制块 */
static uint32_t init_task_stack[1024];     
static task_t init_task;

void init_task_entry(void){
    int count = 0;
    for(;;){
        log_printf("task %d \n",count++);
        task_switch_from_to(&init_task, &first_task);
    }

}

void list_test(){
    list_t l;
    list_t* list = &l;
    list_node_t nodes[5];
    list_init(list);
    for(int i=0;i<5;i++){
        list_insert_first(list, nodes+i); 
        log_printf("insert 0x%x to list.\n",nodes+i);
    }
    list_node_t* node = list->first;
    for(int i=0;i<9;i++){
        if(node == (void*)0)
            break;
        
        log_printf("node[%d] - addr:0x%x\n",i,node);
        node = node->next;
    }
    // log_printf("first = 0x%x\nlast = 0x%x\n",nodes,nodes+4);
    log_printf("first = 0x%x\nlast = 0x%x\n cout = %d",list_first(list),list_last(list),list_count(list));


}

void init_main(void) {

    log_printf("running version:%s","no version\n");
    list_test();

    task_init(&init_task,(uint32_t)init_task_entry,(uint32_t)&init_task_stack[1024]);
    task_init(&first_task,(uint32_t)0,0);
    write_tr(first_task.tss_sel);

    int a=0;
    // irq_enable_global();
    // ASSERT(!a); 
    while(1){
        log_printf("init main:%d",a++);
        task_switch_from_to(&first_task,&init_task);
    }

    return;
}
