 #include "os_cfg.h"

	// 关于下面这段权限指令可以参考.md文件
	.section .note.GNU-stack,"",%progbits

  	// 不必加.code32因默认就是32位
 	.text
 	.global _start
	.extern kernel_init
	.extern init_main
_start:


    # 第三种方法
    push 4(%esp)

    # kernel_init(boot_info)
    call kernel_init

	// 重新加载GDT
	jmp $KERNEL_SELECTOR_CS, $gdt_reload

gdt_reload:
	mov $KERNEL_SELECTOR_DS, %ax		
	mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs


	mov $(stack + KERNEL_STACK_SIZE), %esp

	jmp init_main

	.bss
.comm stack, KERNEL_STACK_SIZE    // comm 声明未初始化的通用内存区域，以字节计



    

// 中断发生时, 会自动切换到特权级0对应的栈中去执行
// 并且只保存ss,esp,cs,eip,flags寄存器
// 所以需要在中断中自行保存其它寄存器

	// 下面的代码基本上是对上面代码的重复，只不过换用了宏来处理
	// 注意确定没写错，可对照反汇编来看。另外，下面的代码没有办法调试，原因未知
   .text
.macro exception_handler name num with_error_code
	    .extern do_handler_\name
		.global exception_handler_\name
	exception_handler_\name:
		// 如果没有错误码，压入一个缺省值
		// 这样堆栈就和有错误码的情形一样了
		.if \with_error_code == 0
			push $0
		.endif

		// 压入异常号
		push $\num

		// 保存所有寄存器
		pushal
		push %ds
		push %es
		push %fs
		push %gs

		// 调用中断处理函数
		push %esp
		call do_handler_\name
		add $(1*4), %esp		// 丢掉esp

		// 恢复保存的寄存器
		pop %gs
		pop %fs
		pop %es
		pop %ds
		popal

		// 跳过压入的异常号和错误码
		add $(2*4), %esp
		iret
.endm
 

exception_handler unknown, -1, 0
exception_handler divider, 0, 0
exception_handler Debug, 1, 0
exception_handler NMI, 2, 0
exception_handler breakpoint, 3, 0
exception_handler overflow, 4, 0
exception_handler bound_range, 5, 0
exception_handler invalid_opcode, 6, 0
exception_handler device_unavailable, 7, 0
exception_handler double_fault, 8, 1
exception_handler invalid_tss, 10, 1
exception_handler segment_not_present, 11, 1
exception_handler stack_segment_fault, 12, 1
exception_handler general_protection, 13, 1
exception_handler page_fault, 14, 1
exception_handler fpu_error, 16, 0
exception_handler alignment_check, 17, 1
exception_handler machine_check, 18, 0
exception_handler smd_exception, 19, 0
exception_handler virtual_exception, 20, 0

// 硬件中断
exception_handler timer, 0x20, 0

	// simple_switch(&from , to)
	// 这个函数整体完成的工作是 
	// 将from进程的的四个需要保存的寄存器 ebp ebx esi edi 压入,
	// 将to进程的寄存器弹出. 注意是不同的栈空间
	.text
	.global simple_switch
simple_switch:
	mov 4(%esp), %eax
	mov 8(%esp), %edx

	push %ebp
	push %ebx
	push %esi
	push %edi
	mov %esp, (%eax)
	
	mov %edx, %esp
	pop %edi
	pop %esi
	pop %ebx
	pop %ebp

	ret

	.global exception_handler_syscall
	.extern do_handler_syscall
exception_handler_syscall:
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	pushf

	mov %esp, %eax
	push %eax
	call do_handler_syscall
	add $4,%esp

	popf
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa

	retf $(5*4)