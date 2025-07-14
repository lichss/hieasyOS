``` C
    uint32_t* pesp = (uint32_t*) esp; 

    if(pesp){
        *(--pesp) = 0;      /* 一个小小的表达式 就很考验水平*/
        *(--pesp) = 0;      /* 前缀运算符 -- 先于解引用  效果等价于 --pesp; *pesp = 0; */
        *(--pesp) = 0;      /* 之所以是--而非加加 是因为这是栈的生长方向 */
        *(--pesp) = 0;      /* 抽空要把前缀运算符优先级给记牢 */
    }

```



### 发现一个有意思的链表用法
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


### 关于空闲进程
空闲进程是不参与`ready_task`的队列的,这也很合理,空闲进程的作用是兜底 不应该参与.
实现的方法是,在dispatch过程中检查一下,如果`ready_list`确实空了,`idle_task`就上.
`idel_task`还有一点就是,切入后要能出的来.
设计没有出什么问题,(*除了一些低级错误 栈起始地址出错. 另外,之前的kernel报错机制挺好用的.*)


### 为什么是可计数的互斥锁 aka可重入锁
考虑一种场景 递归调用

```
recur(){
    mutex_lock();
    recur();

    mutex_unlock;
}
```

如果同一个线程不能对一个资源多次加锁,上面的例子则会形成错误.

另外 实际上用锁后很容易想明白这点, 锁在逻辑上应该是属于某个资源的
举个例子就是资源是仓库里的东西,锁是挂仓库门上的


### 小小的C语言震撼


```C
uint32_t testa(){
    int a = 2;

    return a+1;
}

void kernel_init (boot_info_t * boot_info) {
    // 初始化CPU，再重新加载
    ASSERT( boot_info->ram_region_count);
    cpu_init();
    *(uint8_t*)testa = 0x12;
    memory_init(boot_info);
    *(uint8_t*)testa = 0x56;
    log_init();
    irq_init();
    time_init();

    task_manager_init();
}
```

你可以看到直接把函数名当成uint8指针并尝试修改内容，
一般的程序肯定就出错了
不过在咱们这可以
所以写OS的是这个👍




