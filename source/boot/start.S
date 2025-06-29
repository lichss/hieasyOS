
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
	mov $0x8000, %bx
	mov $0x02, %ah
	mov $0x02, %cx
	mov $64, %al
	mov $0x0080, %dx
	int $0x13

	jc _loader

	jmp boot_entry
	jmp .
	
	// 引导结束段
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
