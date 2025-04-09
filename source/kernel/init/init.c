#include "init.h"
#include "comm/cpu_instr.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"

/**
 * 内核入口
 */
void kernel_init (boot_info_t * boot_info) {
    // 初始化CPU，再重新加载
    int c = boot_info->ram_region_count;
    cpu_init();
    irq_init(); 
}

void init_main(void) {
    // int a =2;
    int c =3/0;
    // for (;;) {}
    while(1);;
}
