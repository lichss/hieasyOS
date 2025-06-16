#ifndef LIST_H_
#define LIST_H_

typedef struct _list_node_t{

    struct _list_node_t* prev;
    struct _list_node_t* next;


}list_node_t;

static inline void list_node_init (list_node_t* node){
    node->next = node->prev = (list_node_t*)0;
}

static inline list_node_t* list_node_prev (list_node_t* node){
    return node->prev;
}

static inline list_node_t* list_node_next (list_node_t* node){
    return node->next;
}

typedef struct _list_t{
    list_node_t* first;
    list_node_t* last;
    int count;

}list_t;

void list_init(list_t* list);

#endif