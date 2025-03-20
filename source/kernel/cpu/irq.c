#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "cpu/cpu.h"
#include  "os_cfg.h"
#define IDE_TABLE_NR    128

static gate_desc_t idt_table[IDE_TABLE_NR];

void exception_handler_unknown(void);

void irq_init(void){
    for(int i =0;i< IDE_TABLE_NR;i++){
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)exception_handler_unknown,
                  GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_IDT);

    }
    lidt((uint32_t)idt_table,sizeof(idt_table));
}