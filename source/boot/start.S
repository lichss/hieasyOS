
	#include "boot.h"
	// 经我的测试 linux环境下会有可执行栈的相关警告 如果加上这些指令则不会警告
	.section .note.GNU-stack,"",%progbits 
  	// 16位代码,务必加上
  	.code16
 	.text
	.global _start
	.extern boot_entry
_start:
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs

	mov $_start, %esp

	mov $0x0e, %ah
	
	mov $'b', %al
	int $0x10

_loader:
	mov $0x8000, %bx	// 目标内存地址 (ES:BX = 0x0000:0x8000)
	mov $0x02, %ah		// 功能号：读取扇区
	mov $0x02, %cx		// 柱面 0,扇区 2
	mov $64, %al		// 读取扇区数(64个扇区 = 32KB)
	mov $0x0080, %dx	// 驱动器号(0x80 = 第一块硬盘），磁头 0
	int $0x13			// 调用 BIOS 磁盘中断

	jc _loader			// 若出错(CF=1),重试

	jmp boot_entry		// 跳转到外部定义的 boot_entry
	jmp .				// 无限循环（冗余保险）
	
	// 引导结束段
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
