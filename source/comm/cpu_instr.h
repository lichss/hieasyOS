#ifndef __CPU_INSTR_H__
#define __CPU_INSTR_H__

#include "types.h"


/* 
    全他妈是汇编
    根本就看不懂
*/
#if 0
#define cli() \
    __asm__ __volatile__("cli")

#define sti() \
    __asm__ __volatile__("sti")


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
	__asm__ __volatile__("inb %[p], %[v]" : [v]"=a" (rv) : [p]"d"(port));
	return rv;
}

static inline uint16_t inw(uint16_t  port) {
	uint16_t rv;
	/* in ax,dx */
	__asm__ __volatile__("in %[p], %[v]" : [v]"=a" (rv) : [p]"d"(port));
	return rv;
}

static inline void outb(uint16_t port, uint8_t data) {
	__asm__ __volatile__("outb %[v], %[p]" : : [p]"d" (port), [v]"a" (data));
}

static inline uint32_t read_cr0() {
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %[v]":[v]"=r"(cr0));
	return cr0;
}

static inline void write_cr0(uint32_t v) {
	__asm__ __volatile__("mov %[v], %%cr0"::[v]"r"(v));
}


static inline void far_jump(uint32_t selector, uint32_t offset) {
	uint32_t addr[] = {offset, selector };
	__asm__ __volatile__("ljmpl *(%[a])"::[a]"r"(addr));
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

	__asm__ __volatile__("lgdt %[g]"::[g]"m"(gdt));
}



#endif