/**
 * 自己动手写操作系统
 *
 * 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * 该部分程序存储于磁盘的第1个扇区，在计算机启动时将会由BIOS加载到0x7c00处
 * 之后，将由BIOS跳转至0x7c00处开始运行
 * 
 * 作者：李述铜
 * 联系邮箱: 527676163@qq.com
 */
	#include "boot.h"

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
	mov $'o', %al
	int $0x10
	mov $'o', %al
	int $0x10
	mov $'t', %al
	int $0x10
	mov $'e', %al
	int $0x10
	mov $'n', %al
	int $0x10
	mov $'d', %al
	int $0x10
	mov $'\n', %al
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
