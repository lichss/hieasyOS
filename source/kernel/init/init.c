#include "init.h"
#include "comm/boot_info.h"
#include "kernel/cpu/cpu.h"


void kernel_init(boot_info_t* bootInfoPtr){
    // int ram_region_count = bootInfoPtr->ram_region_count;
    cpu_init();

}  

void init_main(){
    int a=2;
    while(1);;

}