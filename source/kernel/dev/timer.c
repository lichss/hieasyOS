#include "dev/timer.h"
#include "cpu/irq.h"
#include "comm/cpu_instr.h"
#include "os_cfg.h"
#include "tools/log.h"
#include "core/task.h"

static uint32_t sys_tick;


/* do_handler_virtual_exception */
void do_handler_timer(exception_frame_t* frame){
    sys_tick++;
    // exception_handler_timer();
    if(sys_tick %100 == 0)
        log_printf("1s passed\n");
    pic_send_eoi(IRQ0_TIMER);   // 有点像清中断标志位
    task_time_tick();

}


static void init_pit(void){

    uint32_t reload_count = PIT_OSC_FREQ / (1000.0 / OS_TICK_MS);

    // 2023-3-18 写错了，应该是模式3或者模式2
    //outb(PIT_COMMAND_MODE_PORT, PIT_CHANNLE0 | PIT_LOAD_LOHI | PIT_MODE0);
    outb(PIT_COMMAND_MODE_PORT, PIT_CHANNLE0 | PIT_LOAD_LOHI | PIT_MODE3);
    outb(PIT_CHANNEL0_DATA_PORT, reload_count & 0xFF);   // 加载低8位
    outb(PIT_CHANNEL0_DATA_PORT, (reload_count >> 8) & 0xFF); // 再加载高8位

    irq_install(IRQ0_TIMER, (irq_handler_t)exception_handler_timer);
    irq_enable(IRQ0_TIMER);

    return;
}

void time_init (void) {
    sys_tick = 0;

    init_pit();
}

