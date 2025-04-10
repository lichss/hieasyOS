#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "cpu/cpu.h"
#include  "os_cfg.h"
#define IDT_TABLE_NR    128

static gate_desc_t idt_table[IDT_TABLE_NR];


static void do_default_handler(exception_frame_t* frame, const char* info){
    while(1);
}


void do_handler_unknown(exception_frame_t* frame){
    do_default_handler(frame,"unkonw exception");

    return;
}


void do_handler_divider(exception_frame_t * frame) {
	do_default_handler(frame, "Divider Error.");
}

void do_handler_Debug(exception_frame_t * frame) {
	do_default_handler(frame, "Debug Exception");
}

void do_handler_NMI(exception_frame_t * frame) {
	do_default_handler(frame, "NMI Interrupt.");
}

void do_handler_breakpoint(exception_frame_t * frame) {
	do_default_handler(frame, "Breakpoint.");
}

void do_handler_overflow(exception_frame_t * frame) {
	do_default_handler(frame, "Overflow.");
}

void do_handler_bound_range(exception_frame_t * frame) {
	do_default_handler(frame, "BOUND Range Exceeded.");
}

void do_handler_invalid_opcode(exception_frame_t * frame) {
	do_default_handler(frame, "Invalid Opcode.");
}

void do_handler_device_unavailable(exception_frame_t * frame) {
	do_default_handler(frame, "Device Not Available.");
}

void do_handler_double_fault(exception_frame_t * frame) {
	do_default_handler(frame, "Double Fault.");
}

void do_handler_invalid_tss(exception_frame_t * frame) {
	do_default_handler(frame, "Invalid TSS");
}

void do_handler_segment_not_present(exception_frame_t * frame) {
	do_default_handler(frame, "Segment Not Present.");
}

void do_handler_stack_segment_fault(exception_frame_t * frame) {
	do_default_handler(frame, "Stack-Segment Fault.");
}

void do_handler_general_protection(exception_frame_t * frame) {
	do_default_handler(frame, "General Protection.");
}

void do_handler_page_fault(exception_frame_t * frame) {
	do_default_handler(frame, "Page Fault.");
}

void do_handler_fpu_error(exception_frame_t * frame) {
	do_default_handler(frame, "X87 FPU Floating Point Error.");
}

void do_handler_alignment_check(exception_frame_t * frame) {
	do_default_handler(frame, "Alignment Check.");
}

void do_handler_machine_check(exception_frame_t * frame) {
	do_default_handler(frame, "Machine Check.");
}

void do_handler_smd_exception(exception_frame_t * frame) {
	do_default_handler(frame, "SIMD Floating Point Exception.");
}

void do_handler_virtual_exception(exception_frame_t * frame) {
	do_default_handler(frame, "Virtualization Exception.");
}

void irq_init(void){
    for(int i =0;i< IDT_TABLE_NR;i++){
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)exception_handler_unknown,
                  GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_IDT);

    }

    irq_install(IRQ0_DE,(irq_handler_t)exception_handler_divider);

    lidt((uint32_t)idt_table,sizeof(idt_table));
}


int irq_install(int irq_num, irq_handler_t handler){
    if (irq_num >= IDT_TABLE_NR)
        return -1;

    gate_desc_set(idt_table + irq_num, KERNEL_SELECTOR_CS, (uint32_t)handler,
              GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_IDT);
    return 0;

}