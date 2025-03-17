    .text
    .extern kernel_init
    .global _start
_start:
    # 下面这段代码 用汇编续写了一部分，
    # 实现带参数跳转，如果编译没有问题，这一段应该是跟着loader_32.c的代码后面执行的
    mov 0x4(%esp), %eax  ; # sent boot_infoPtr to eax
    push %eax           ; # push eax to stack, like call with argument
    call kernel_init

    jmp .