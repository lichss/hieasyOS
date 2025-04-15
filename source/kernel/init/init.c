#include "init.h"
#include "comm/cpu_instr.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"
#include "tools/log.h"
#include "tools/klib.h"


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

void init_main(void) {
    // int a =2;
    // int c =3/0;
    // for (;;) {}

    log_printf("running version:%s","no version\n");

    int a=1;
    // irq_enable_global();
    ASSERT(!a); 
    while(1){
        a  = a/0;
    }
}
