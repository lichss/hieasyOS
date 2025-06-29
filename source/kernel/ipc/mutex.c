#include "ipc/mutex.h"
#include "cpu/irq.h"

void mutex_init(mutex_t* mutex){

    mutex->locked_count = 0;
    mutex->owner  = (void*)0;
    list_init(&mutex->wait_list);


}

void mutex_lock(mutex_t* mutex){
    irq_state_t state = irq_enter_protection();

    task_t* curr = task_current();
    if(mutex->locked_count == 0){
        mutex->locked_count++;
        mutex->owner = curr;
    }else if(mutex->owner == curr){
        mutex->locked_count++;
    }else{
        task_set_block(curr);
        list_insert_last(&mutex->wait_list,&curr->wait_node);
        task_dispatch();
    }


    irq_leave_protection(state);
}
/**
 * @brief 解锁 但计数情况为负就麻烦了 以后可以改进
 */

void mutex_unlock(mutex_t* mutex){
    irq_state_t state = irq_enter_protection();
    
    task_t* curr = task_current();
    if(mutex->owner == curr){
        if(--mutex->locked_count==0){
            mutex->owner = (void*)0;
            if(list_count(&mutex->wait_list)){
                list_node_t* node = list_remove_first(&mutex->wait_list);
                task_t* task = list_node_parent(node,task_t,wait_node);
                task_set_ready(task);

                mutex->locked_count = 1;
                mutex->owner = task;

                task_dispatch();
            }
        }

    }


    irq_leave_protection(state);
}

/**
 * 李仔的风格还是很激进的,有很多潜在的出错的地方
 * 比如这里 互斥锁计数没考虑过为负数的情况,这就要求编程时必须注意加锁和解锁
 * 如果有意外,解锁函数八成不能正常运行. 不嫌弃工作量的话 更好的做法显然是检查 然后报错
 * 
 * 
 */