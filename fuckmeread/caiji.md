``` C
    uint32_t* pesp = (uint32_t*) esp; 

    if(pesp){
        *(--pesp) = 0;      /* 一个小小的表达式 就很考验水平*/
        *(--pesp) = 0;      /* 前缀运算符 -- 先于解引用  效果等价于 --pesp; *pesp = 0; */
        *(--pesp) = 0;      /* 之所以是--而非加加 是因为这是栈的生长方向 */
        *(--pesp) = 0;      /* 抽空要把前缀运算符优先级给记牢 */
    }

```



发现一个有意思的链表用法
```C
typedef struct _list_node_t{

    struct _list_node_t* prev;
    struct _list_node_t* next;


}list_node_t;
```

这个链表节点是没有内容的,跟leetcode里常见的不同,使用这种链表的方法是
在目标结构中嵌入一个节点,例如这样

```C
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

    list_node_t run_node;
    list_node_t all_node;

    tss_t tss;
    int tss_sel;
}task_t;

```

在需要使用时使用类似下面宏的方式

```C
#define offset_in_parent(parent_type, node_name)    \
    ((uint32_t)&(((parent_type*)0)->node_name))

#define offset_to_parent(node, parent_type, node_name)   \
    ((uint32_t)node - offset_in_parent(parent_type, node_name))

/*     task_t* nextRun = list_node_parent(task_node,task_t,run_node); */
#define list_node_parent(node, parent_type, node_name)   \
        ((parent_type *)(node ? offset_to_parent((node), parent_type, node_name) : 0))
```

比较考究的链表和使用方法,和linux内核源码给我的感觉比较接近