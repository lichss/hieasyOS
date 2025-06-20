#include "tools/list.h"

void list_init(list_t* list){
    list->first = list->last = (list_node_t*) 0;
    list->count = 0;
}



void list_insert_first(list_t* list,list_node_t* node){
    if(!node)
        return;

    node->prev = (list_node_t*) 0;
    node->next = list->first;
    list->first = node;

    if(list_is_empty(list)){
        list->last = node;
    }else{
        list->first->prev = node;
    }
    
    list->count++;
    return;
}

void list_insert_last(list_t* list,list_node_t* node){
    if(!node)
        return;

    node->next = (list_node_t*) 0;
    node->prev = list->last;
    list->last = node;

    if(list_is_empty(list)){
        list->first = node;
    }else{
        list->last->next = node;
    }

    list->count++;
    return;
}
/** 
     * 如果我说我在这里生命一个常量指针,
     * 期望编译器把它内联优化,从而不占用
     * 额外内存,你会不会觉得我很聪明呢? 
     */
    
list_node_t* list_remove_first(list_t* list) {
    if(list_is_empty(list))
        return 0;
        
    list_node_t* const node = list->first;
    list->first = node->next; // 更新 first 指针

    if(list->first) {
        list->first->prev = 0; // 如果新 first 存在，清空它的 prev
    } else {
        list->last = 0; // 如果链表变空，清空 last
    }

    node->prev = node->next = 0; // 隔离移除的节点
    list->count--;
    return node;
}
/**
 * @brief 更简洁的做法. 唯一缺点是需要假定node存在于链表当中,否则会出现未知错误
 */
list_node_t* list_remove(list_t* list,list_node_t* node){
    if(!node)
        return 0;
    if(node==list->first)
        list->first = node->next;
    
    if(node == list->last)
        list->last = node->prev;

    if(node->prev)
        node->prev->next= node->next;

    if(node->next)
        node->next->prev = node->prev;

    node->prev = node->next = 0;
    list->count--;

    return node;
}
#if 0   /*疑似有点蠢了*/
list_node_t* list_remove(list_t* list,list_node_t* del_node){
    if(list_is_empty(list)|| del_node == 0)
        return 0;

    if (list->count == 1) {
        list_node_t* node = list->first;
        list->first = 0;
        list->last = 0;
        list->count--;
        return node;  //返回被删除的节点
    }

    if(del_node == list->last ){
        list_node_t* node = list->last; 
        list->last = list->last->prev;
        if(list->last)
            list->last->next = 0;
        list->count--;
        return node;
    }
    if(del_node == list->first){
        list_node_t* node = list->first;
        list->first = list->first->next;
        if(list->first)
            list->first->prev = 0;
        
        list->count--;
        return node;
    }
    list_node_t* node = list->first->next;
    while(node!=0){ /*   看似安全 实际上如果是多线程则可能导致 崩溃 */ 
        if(node == del_node){
            node->next->prev = node->prev;
            node->prev->next = node->next;

            node->prev = node->next = 0;
            break;
        }
        node = node->next;
    }
    if(node)
        list->count--;
    return node; 
}
#endif