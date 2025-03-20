__asm__(".code16gcc");  /*惨痛教训 这个指令必须放在文件第一行*/

#include "loader.h"


// __asm__(".code16gcc");

boot_info_t boot_info;

int print_string(const char *string) {
    char c;
    while((c = *string++)!='\0') {
        asm volatile(                            /*这一段内联汇编其实都不太了解*/ /*如果考虑兼容性问题请使用 __asm__ __volatile__*/
            "mov $0x0E, %%ah\n\t"         /*mov $0x0E , %ah  中断的要求，设置ah*/
            "int $0x10\n"                 /*调用中断号0x10 的bios中断*/
            :                             /* ：冒号是内联汇编语法的一部分 */
            :"al"(c)                      /*在执行以上两条语句之前，把变量c的值放入al寄存器*/
            :"%ah", "%al"                 /*告诉编译器，这两个寄存器的值已经被改变*/
        );  
    }
    return c == '\0' ? 0 : 1;
}

static void  detect_memory(void) {
	uint32_t contID = 0;
	SMAP_entry_t smap_entry;
	int signature, bytes;

    print_string("Detecting memory...");

	// 初次：EDX=0x534D4150,EAX=0xE820,ECX=24,INT 0x15, EBX=0（初次）
	// 后续：EAX=0xE820,ECX=24,
	// 结束判断：EBX=0
	boot_info.ram_region_count = 0;
	for (int i = 0; i < BOOT_RAM_REGION_MAX; i++) {
		SMAP_entry_t * entry = &smap_entry;

		asm volatile("int  $0x15"
			: "=a"(signature), "=c"(bytes), "=b"(contID)
			: "a"(0xE820), "b"(contID), "c"(24), "d"(0x534D4150), "D"(entry));
		if (signature != 0x534D4150) {
            print_string("failed.\r\n");
			return;
		}

		// todo: 20字节
		if (bytes > 20 && (entry->ACPI & 0x0001) == 0){
			continue;
		}

        // 保存RAM信息，只取32位，空间有限无需考虑更大容量的情况
        if (entry->Type == 1) {
            boot_info.ram_region_cfg[boot_info.ram_region_count].start = entry->BaseL;
            boot_info.ram_region_cfg[boot_info.ram_region_count].size = entry->LengthL;
            boot_info.ram_region_count++;
        }

		if (contID == 0) {
			break;
		}
	}
    print_string("ok.\r\n");
}

// GDT表。临时用，后面内容会替换成自己的
uint16_t gdt_table[][4] = {
    {0x0000, 0x0000, 0x0000, 0x0000},
    {0xFFFF, 0x0000, 0x9A00, 0x00CF},
    {0xFFFF, 0x0000, 0x9200, 0x00CF},
};


static void into_protMode(){

    cli();
    uint8_t v = inb(0x92);
    outb(0x92, v | 0x2);

    lgdt((uint32_t)gdt_table,sizeof(gdt_table));    /*我已经对这个函数的功能有猜测了 第一个参数是地址，第二个单位是长度。装GDT寄存器里头*/
    // lgdt((uint32_t)gdt_table,16);


    uint32_t cr0 = read_cr0();
    cr0 |= 0x01;
    write_cr0(cr0);

    far_jump(8, (uint32_t)protected_mode_entry);

    return ;
}


void loader_entry(void) {

    print_string("Hello,x\n\r");
    detect_memory();
    into_protMode();

    while(1){
        int a = 1;
    };

}

