#include "core/task.h"
#include "tools/klib.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "cpu/cpu.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "core/memory.h"
#include "cpu/mmu.h"

// static uint32_t init_task_stack[IDLE_TASK_SIZE];
static uint32_t idle_task_stack[IDLE_STACK_SIZE];
static task_manager_t task_manager;
    /* 操作系统的 */
static int tss_init (task_t * task, uint32_t prv_level, uint32_t entry, uint32_t esp) {
    // 为TSS分配GDT
    int tss_sel = gdt_alloc_desc();
    if (tss_sel < 0) {
        log_printf("alloc tss failed.\n");
        return -1;
    }

    segment_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t), 
            SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS);

    /*到这里为止 完成的工作是获取新的段描述符 然后设置描述符属性*/

    // tss段初始化
    kernel_memset(&task->tss, 0, sizeof(tss_t));

    uint32_t code_sel,data_sel;
    if(prv_level & TASK_PRVLEVEL_SYSTEM){
        code_sel = KERNEL_SELECTOR_CS;
        data_sel = KERNEL_SELECTOR_DS;
    }else{
        code_sel = task_manager.app_code_sel | SEG_CPL3;
        data_sel = task_manager.app_data_sel | SEG_CPL3;
    }
    /* maybe ,in this case, evey task has small kernel stack. */
    uint32_t kernel_stack  = (uint32_t)memory_alloc_page();
    if(kernel_stack == 0){
        goto tss_init_fail;
    }
    task->tss.eip = entry;
    task->tss.esp = esp;
    task->tss.esp0 = kernel_stack + MEM_PAGE_SIZE; // 0级栈顶
    task->tss.ss = data_sel;
    task->tss.ss0 = KERNEL_SELECTOR_DS; // 0级数据段
    task->tss.eip = entry;
    task->tss.eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    task->tss.es = task->tss.ss = task->tss.ds
            = task->tss.fs = task->tss.gs = data_sel;   // 暂时写死
    task->tss.cs = code_sel;    // 暂时写死
    task->tss.iomap = 0;

    uint32_t page_dir = memory_create_uvm();
    if(page_dir == 0){
        gdt_free_sel(tss_sel);
        return -1;
    }

    task->tss.cr3 = page_dir;

    task->tss_sel = tss_sel;
    return 0;

tss_init_fail:
    gdt_free_sel(tss_sel);
    if(kernel_stack != 0)
        memory_free_page(kernel_stack);

    return -1;

}





int task_init(task_t* task,const char* name, uint32_t prv_level, uint32_t entry,uint32_t esp){
    ASSERT(task!= 0);
    tss_init(task, prv_level, entry, esp);

    kernel_strncpy(task->name,name,TASK_NAME_SIZE);

    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    task->sleep_ticks = 0;

    task->state = TASK_CREATED;
    
    list_node_init(&task->run_node);
    list_node_init(&task->all_node);
    list_node_init(&task->wait_node);

    irq_state_t state = irq_enter_protection(); 
    task_set_ready(task);
    list_insert_last(&task_manager.task_list,&task->all_node);
    task->pid = (uint32_t)task;

    irq_leave_protection(state);
    return 0;
}
/* 这个函数的具体实现在 kernel/init/start.s 里 */
void simple_switch(uint32_t** from,uint32_t* to);

void task_switch_from_to(task_t *form, task_t* to){
    switch_to_tss(to->tss_sel); 
    // simple_switch(&form->stack,to->stack);
}

// extern void* first_task_entry;
void task_first_init(void){
    extern uint8_t s_first_task[], e_first_task[];
    uint32_t copy_size = e_first_task - s_first_task;
    uint32_t alloc_size = 10 * MEM_PAGE_SIZE;
    uint32_t first_start = (uint32_t)first_task_entry;

    task_init(&task_manager.first_task,"first task",(0), (uint32_t)first_task_entry,first_start + alloc_size);  
    /* consider this for reason of
        esp grow down, so esp should be higher than entry point*/
    
    write_tr(task_manager.first_task.tss_sel);
    task_manager.curr_task = &task_manager.first_task;

    mmu_set_page_dir(task_manager.first_task.tss.cr3);

    memory_alloc_page_for(first_start,alloc_size,PTE_P | PTE_W | PTE_U);
    kernel_memcpy((void*)first_task_entry, s_first_task, copy_size);

}

task_t* task_first_task(void){
    return &task_manager.first_task;
}

static void idle_task_entry(void){

    while(1){
        hlt();
    }

}

void task_manager_init(){

    int data_sel = gdt_alloc_desc();
    if(data_sel < 0){
        log_printf("alloc idle task tss failed.\n");
        return;
    }    
    segment_desc_set(data_sel, 0x00000000,0xFFFFFFFF,
                        SEG_P_PRESENT | SEG_DPL3 | SEG_S_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D
    );
    task_manager.app_data_sel = data_sel;

    int code_sel = gdt_alloc_desc();
    if(code_sel < 0){
        log_printf("alloc idle task tss failed.\n");
        return;
    }    
    segment_desc_set(code_sel, 0x00000000,0xFFFFFFFF,
                        SEG_P_PRESENT | SEG_DPL3 | SEG_S_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D
    );
    task_manager.app_code_sel = code_sel;

    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);
    list_init(&task_manager.sleep_list);

    task_init(&task_manager.idle_task,"idel task",(TASK_PRVLEVEL_SYSTEM), (uint32_t)idle_task_entry,(uint32_t)(idle_task_stack + IDLE_STACK_SIZE));


    task_manager.curr_task = 0;

}



void task_set_ready(task_t* task){
    if(task == &task_manager.idle_task)
        return;
    task->state = TASK_READY;
    list_insert_last(&task_manager.ready_list,&task->run_node);

}

void task_set_block(task_t* task){
    if(task == &task_manager.idle_task)
        return;

    list_remove(&task_manager.ready_list,&task->run_node);
}

task_t* task_next_run(void){

    if (list_count(&task_manager.ready_list) == 0) {
        return &task_manager.idle_task;
    }
    
    list_node_t* task_node = list_first(&task_manager.ready_list);
    return list_node_parent(task_node,task_t,run_node);
}
task_t* task_current(void){
    return task_manager.curr_task;
}

int sys_sched_yield(){
    
    irq_state_t state = irq_enter_protection();

    if(list_count(&task_manager.ready_list)>1){
        task_t* curr = task_current();
        task_set_block(curr);
        task_set_ready(curr);   /*相当于滚后面排队了*/

        task_dispatch();
    }

    irq_leave_protection(state);
    return 0;
}

void task_dispatch(void){

    irq_state_t state = irq_enter_protection();

    task_t* to = task_next_run();
    if(to != task_manager.curr_task){
        task_t* from = task_current(); 
        task_manager.curr_task = to;
        to->state = TASK_RUNNING;
        task_switch_from_to(from,to);
    }

    irq_leave_protection(state);
}


void task_time_tick(){
    task_t* curr = task_current();
    if(--curr->slice_ticks == 0){

        curr->slice_ticks = curr->time_ticks;

        task_set_block(curr);
        task_set_ready(curr);

        task_dispatch();
    }
    // list_node_t* node = task_manager.sleep_list.first;
    list_node_t* node = list_first(&task_manager.sleep_list);
    while(node){
        list_node_t* next = node->next;
        task_t* task = list_node_parent(node,task_t,run_node);
        if(--task->sleep_ticks == 0){
            task_set_wakeup(list_node_parent(node,task_t,run_node));
            task_set_ready(task);
        }
        node = next;
    }

    task_dispatch();
}

void task_set_sleep(task_t* task,uint32_t ticks){
    if(ticks == 0)
        return;

    task->state = TASK_SLEEP;
    task->sleep_ticks = ticks;
    list_insert_last(&task_manager.sleep_list,&task->run_node);

}
void task_set_wakeup(task_t* task){
    list_remove(&task_manager.sleep_list,&task->run_node);

}

void sys_sleep(uint32_t ms){
    irq_state_t state = irq_enter_protection();
    task_set_block(task_manager.curr_task);
    task_set_sleep(task_manager.curr_task,(ms + OS_TICK_MS -1) / OS_TICK_MS);

    task_dispatch();
    irq_leave_protection(state);
}

int sys_getpid(){
    task_t* curr = task_current();
    int pid = curr->pid;
    return pid;
}

void sys_printmsg(int fmt,int arg){
    log_printf((const char*)fmt,arg);
}

void manager_report(void){
    log_printf("-------Manager report-----\n");
    log_printf("now ready task number:%d",task_manager.ready_list.count);
    char* nameList[9];
    int ready_listN = task_manager.ready_list.count;
    list_node_t* node = task_manager.ready_list.first;
    int i=0;
    for(;i<9 && i<ready_listN;i++){
        if(node==0)
            break;
        nameList[i] = list_node_parent(node,task_t,run_node)->name;
        node = node->next;

    }
    log_printf("ready list: ");
    for(int j=0;j<i;j++){
        log_printf("%s",nameList[j]);
    }

}