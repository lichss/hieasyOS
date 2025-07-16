/**
 */
#include "comm/cpu_instr.h"
#include "cpu/cpu.h"
#include "os_cfg.h"
#include "cpu/irq.h"
#include "ipc/mutex.h"

/* 你看 GDT table就是segment descript 组成的数组 */
static segment_desc_t gdt_table[GDT_TABLE_SIZE];
static mutex_t mutex;
/**
 * 设置段描述符
 */
void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr) {
    segment_desc_t * desc = gdt_table + selector  / sizeof(segment_desc_t);

	// 如果界限比较长，将长度单位换成4KB
	if (limit > 0xfffff) {
		attr |= 0x8000;
		limit /= 0x1000;
	}
	desc->limit15_0 = limit & 0xffff;
	desc->base15_0 = base & 0xffff;
	desc->base23_16 = (base >> 16) & 0xff;
	desc->attr = attr | (((limit >> 16) & 0xf) << 8);
	desc->base31_24 = (base >> 24) & 0xff;
}

/*
 * 初始化GDT
 */


void gate_desc_set(gate_desc_t* desc,uint16_t selector,uint32_t offset, uint16_t attr){
    desc->offset15_0 = offset & 0xffff;
    desc->selector = selector;
    desc->attr = attr;
    desc->offset31_16 = (offset >> 16) & 0xffff;
}

int gdt_alloc_desc(){
    mutex_lock(&mutex);
    /* 我操这段代码不是很理解 */
    /*  就由来自未来的我来解答你的疑惑 */
    /** 
     * 轮询查找可用的全局描述符表项，超过界限则返回错误
     * 想要理解这些 就要明白段描述符 全局描述符表
     */
    for(int i=1;i<GDT_TABLE_SIZE;i++){
        segment_desc_t * desc = gdt_table + i;
        if(desc->attr == 0){
            return i * sizeof(segment_desc_t);
        }
    }

    mutex_unlock(&mutex);
    return -1;
}
 
void init_gdt(void) {
	// 全部清空
    for (int i = 0; i < GDT_TABLE_SIZE; i++) {
        segment_desc_set(i * sizeof(segment_desc_t), 0, 0, 0);
    }

    //数据段
    segment_desc_set(KERNEL_SELECTOR_DS, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_DATA
                     | SEG_TYPE_RW | SEG_D);

    // 只能用非一致代码段，以便通过调用门更改当前任务的CPL执行关键的资源访问操作
    segment_desc_set(KERNEL_SELECTOR_CS, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_CODE
                     | SEG_TYPE_RW | SEG_D);


    // 加载gdt
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

/**
 * CPU初始化
 */
void cpu_init (void) {
    mutex_init(&mutex);
    init_gdt();

}


void switch_to_tss(int tss_sel){
    far_jump(tss_sel,0);

}

void gdt_free_sel(int sel){
    mutex_lock(&mutex);
    gdt_table[sel / sizeof(segment_desc_t)].attr = 0;
    mutex_unlock(&mutex);

}