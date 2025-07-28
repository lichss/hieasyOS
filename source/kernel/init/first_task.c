#include "tools/log.h"
#include "core/task.h"
// #include "core/syscall.h"
#include "applib/lib_syscall.h"


int first_task_main(){
    int pid = get_pid();
    int a = 0;
    while(1){
        // log_printf("first task main.");
        // sys_sleep(1000);
        print_msg("ababab%d",13);
        msleep(1000);
    }
}