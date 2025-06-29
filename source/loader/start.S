/**


 */
	// 关于下面这段权限指令可以参考.md文件
	.section .note.GNU-stack,"",%progbits
  	// 16位代码,务必加上
  	.code16 
 	.text
	.global _start
	.extern loader_entry
_start:

	jmp loader_entry

	// 32位代码
	.code32
	.text
	.global protected_mode_entry
	.extern loader_anomi_entry
protected_mode_entry:
	mov $16, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs

	jmp $8, $loader_anomi_entry

	jmp .
