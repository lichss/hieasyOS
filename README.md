# DO NOT READ ME 

###
内联汇编关键字 `asm volatile` 和 `__asm__ __volatile__` 在功能上无区别,兼容性方面加下划线的关键字更加
我看着下划线太丑了 采用不加下划线版本


## Mar-15 修错日记：


```C
' __asm__(".code16gcc");  /*惨痛教训 这个指令必须放在文件第一行，不然很可能会不生效*/ '
```
在[loader_16.c](./source/loader/loader_16.c) 中第一行习惯性写`#inclde "loader.h"` 导致`__asm__();`没有正常生效，导致后面lgdt函数失效，所以GDT寄存器的值一直不对。


## Mar-16 修错日记：

昨天走之前一次提交误添加了一些文件，今天回溯版本时候--hard 直接把我脚本干没了。就因为这个今天一天都在找错。也算是警告，不要只看`[build] Build finished with exit code 0` 应该要注意下`.bat`脚本里反馈的执行结果。
原则上不应该添加设置文件和脚本文件，但是随着项目进程，脚本和设置需要改改，这次的也算是个教训。以后的提交里面都跟踪脚本和设置。


## Mar-18 日记：
那天没写，现在记不清是哪天了. 做了一点工作，现在这个项目使用独立。两个vhd镜像文件和测试用分离。修改了脚本文件和所有Cmakelists.txt的相对目录。

## Mar-20 日记：
整个源码kernel部分应该有很多问题，一开始`boot_info`参数能正产传递，但`init_main`不能正常跳转，问题可能出在 `segment_desc_set` [cpu.c](./source/kernel/cpu/cpu.c)函数上,照抄之后可以正常跳转`init_main` ，但`boot_info`又出问题。。艹了。现在整个[kernel](./source/kernel)照抄，现在应该没有问题了。
我决定从现在开始尽量保持接口命名对齐，以方便以后直接照抄。

## 一点思考
```C
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
```
其中的 ` list_node_t* const node = list->first; ` 加 `const` 的意图是希望编译器内联优化这一段,使得代码既有可读性,同时还能节省内存(*虽然这一点点的内存是否有必要节省存疑*)
如果希望改进的话应该引入`restrict`关键字


## 关于多线程的想法
```C    
    while(node!=0){ /*   看似安全 实际上如果是多线程则可能导致 崩溃 */ 
        if(node == del_node){
            node->next->prev = node->prev;
            node->prev->next = node->next;

            node->prev = node->next = 0;
            break;
        }
        node = node->next;
    }
```


## 内存分页 有点问题
现在内存分页开了以后，不能开全剧终断开关了，具体点就是`sti`指令，已执行就重启（win平台重启Qemu,linux平台coredown，跟Qemu有关。 暂时不知道咋解决）

### 只是进程页表没处理好而已。稍安勿躁小问题

### 一个很难找的问题