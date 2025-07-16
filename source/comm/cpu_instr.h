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
/**
 * ljmpl 是复合指令 不仅仅跳转，cpu寄存器的存取也集成在这条指令当中
 * 我尝试观察这条指令在反汇编中的状态 。
 * 因为使用inline关键字，但反汇编代码中仍然使用 `call` 导致实际上的 ljmpl被隐藏了
 * 但ljmpl的三个作用是不会错的 
 * 1 保存当前任务状态
 * 2 读取新任务状态
 * 3 更新cpu寄存器 (完成跳转)
 以下是部分反汇编
0001106d <switch_to_tss>:
void switch_to_tss(int tss_sel){
   1106d:	55                   	push   %ebp
   1106e:	89 e5                	mov    %esp,%ebp
    far_jump(tss_sel,0);
   11070:	8b 45 08             	mov    0x8(%ebp),%eax
   11073:	6a 00                	push   $0x0
   11075:	50                   	push   %eax
   11076:	e8 f6 fd ff ff       	call   10e71 <far_jump>
   1107b:	83 c4 08             	add    $0x8,%esp
}

通过袪掉 inline 关键字 在反汇编文件中找到了ljmpl对应的反汇编码
000118fe <far_jump>:
   118fe:	55                   	push   %ebp
   118ff:	89 e5                	mov    %esp,%ebp
   11901:	83 ec 10             	sub    $0x10,%esp
   11904:	8b 45 0c             	mov    0xc(%ebp),%eax
   11907:	89 45 f8             	mov    %eax,-0x8(%ebp)
   1190a:	8b 45 08             	mov    0x8(%ebp),%eax
   1190d:	89 45 fc             	mov    %eax,-0x4(%ebp)
   11910:	8d 45 f8             	lea    -0x8(%ebp),%eax
   11913:	ff 28                	ljmp   *(%eax)		就是这一行了 ljmp == ljmpl 上面是far_jump函数的传参过程
   11915:	90                   	nop
   11916:	c9                   	leave
   11917:	c3                   	ret

 */
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