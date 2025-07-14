#ifndef __CPU_INSTR_H__
#define __CPU_INSTR_H__

#include "types.h"


/* 
    全他妈是汇编
    根本就看不懂
*/
#if 0
#define cli() \
    asm volatile("cli")

#define sti() \
    asm volatile("sti")


#else
static inline void cli() {
	__asm__ __volatile__("cli");
}

static inline void sti() {
	__asm__ __volatile__("sti");
}

#endif

static inline uint8_t inb(uint16_t  port) {
	uint8_t rv;
	/* inb al,dx */
	asm volatile("inb %[p], %[v]" : [v]"=a" (rv) : [p]"d"(port));
	return rv;
}

static inline uint16_t inw(uint16_t  port) {
	uint16_t rv;
	/* in ax,dx */
	asm volatile("in %[p], %[v]" : [v]"=a" (rv) : [p]"d"(port));
	return rv;
}

static inline void outb(uint16_t port, uint8_t data) {
	asm volatile("outb %[v], %[p]" : : [p]"d" (port), [v]"a" (data));
}

static inline uint32_t read_cr0() {
	uint32_t cr0;
	asm volatile("mov %%cr0, %[v]":[v]"=r"(cr0));
	return cr0;
}

static inline void write_cr0(uint32_t v) {
	asm volatile("mov %[v], %%cr0"::[v]"r"(v));
}

/* 这里使用u32 而不是u16 */
static inline uint32_t read_cr3() {
	uint32_t cr3;
	asm volatile("mov %%cr3, %[v]":[v]"=r"(cr3));
	return cr3;
}

static inline void write_cr3(uint32_t v) {
	asm volatile("mov %[v], %%cr3"::[v]"r"(v));
}

static inline uint32_t read_cr4() {
	uint32_t cr4;
	asm volatile("mov %%cr4, %[v]":[v]"=r"(cr4));
	return cr4;
}

static inline void write_cr4(uint32_t v) {
	asm volatile("mov %[v], %%cr4"::[v]"r"(v));
}

static inline void far_jump(uint32_t selector, uint32_t offset) {
	uint32_t addr[] = {offset, selector };
	asm volatile("ljmpl *(%[a])"::[a]"r"(addr));
}

static inline void lgdt(uint32_t start, uint32_t size) {
	struct {
		uint16_t limit;
		uint16_t start15_0;    // 视频中这里写成了32位
		uint16_t start31_16;    // 视频中这里写成了32位
	} gdt;

	gdt.start31_16 = start >> 16;
	gdt.start15_0 = start & 0xFFFF;
	gdt.limit = size - 1;

	asm volatile("lgdt %[g]"::[g]"m"(gdt));
}

static inline void lidt(uint32_t start, uint32_t size){
	struct {
		uint16_t limit;
		uint16_t start15_0;    // 视频中这里写成了32位
		uint16_t start31_16;    // 视频中这里写成了32位
	} idt;

	idt.start31_16 = start >> 16;
	idt.start15_0 = start & 0xFFFF;
	idt.limit = size - 1;

	asm volatile("lidt %[g]"::[g]"m"(idt));
}

static inline void hlt(void){
	asm volatile("hlt");
}

static inline void write_tr(int tss_sel){
	asm volatile ("ltr %%ax"::"a"(tss_sel));

}

static inline uint32_t read_eflags (void) {
    uint32_t eflags;

    asm volatile("pushfl\n\tpopl %%eax":"=a"(eflags));
    return eflags;
}

static inline void write_eflags (uint32_t eflags) {
    asm volatile("pushl %%eax\n\tpopfl"::"a"(eflags));
}

#endif