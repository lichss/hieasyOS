#ifndef LIB_SYACALL_H__
#define LIB_SYSCALL_H__

#include "comm/types.h"
#include "os_cfg.h"
#include "core/syscall.h"

// #define SYS_sleep       (0)
// #define SYS_get_pid     (1)
// #define SYS_printmsg    (2)


typedef struct _syscall_args_t{
    int id;
    int arg0;
    int arg1;
    int arg2;
    int arg3;

}syscall_args_t;


static inline int sys_call(syscall_args_t* args){
    uint32_t addr[] = {0,SELECTOR_SYSCALL | 0};
    int ret;

    asm volatile(
        "push %[arg3]\n\t" 
        "push %[arg2]\n\t" 
        "push %[arg1]\n\t" 
        "push %[arg0]\n\t" 
        "push %[id]\n\t" 
        "lcalll *(%[a])"
        :"=a"(ret)
        :
        [arg3]"r"(args->arg3),
        [arg2]"r"(args->arg2),
        [arg1]"r"(args->arg1),
        [arg0]"r"(args->arg0),
        [id]"r"(args->id),
        [a]"r"(addr)
    );

    return ret;
}

static inline void msleep(int ms){
    if(ms <= 0){
        return;
    }

    syscall_args_t args;
    args.id = SYS_sleep;
    args.arg0 = ms;

    sys_call(&args);
}

static inline int getpid(){
    syscall_args_t args;
    args.id = SYS_getpid;

    return sys_call(&args);
}

static inline void print_msg(const char* fmt,int arg){
    syscall_args_t args;
    args.id = SYS_printmsg;
    args.arg0 = (int)fmt;
    args.arg1 = arg;
    sys_call(&args);

}

static inline int fork() {
    syscall_args_t args;
    args.id = SYS_fork;
    return sys_call(&args);
}


static inline int execve(const char *name, char * const *argv, char * const *env) {
    syscall_args_t args;
    args.id = SYS_execve;
    args.arg0 = (int)name;
    args.arg1 = (int)argv;
    args.arg2 = (int)env;
    return sys_call(&args);
}

#endif