/**
 * 自己动手写操作系统
 *
 * 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * boot扇区容量较小，仅512字节。由于dbr占用了不少字节，导致其没有多少空间放代码，
 * 所以功能只能最简化,并且要开启最大的优化-os
 *
 * 作者：徐程升！！
 * 联系邮箱: evtjk@hotmail.com
 */
__asm__(".code16gcc");

#include "boot.h"
#define BOOT_LOADER_ENTRY 0x8000
/**
 * Boot的C入口函数
 * 只完成一项功能，即从磁盘找到loader文件然后加载到内容中，并跳转过去
 */

void (*boot_loaderP)(void) = (void *)BOOT_LOADER_ENTRY; /* 我发现一个非常奇怪的bug  */
void boot_entry(void) {
    boot_loaderP = (void *)BOOT_LOADER_ENTRY;
    boot_loaderP();
    // ((void (*)(void))BOOT_LOADER_ENTRY)();
    // while(1){};

    /* 这里是不是该加点报错退出啊  */
} 

