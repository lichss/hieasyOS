### 总结和GDT相关的内容
#### Global Descriptor Table 全局描述符表
GDT 表中存不同的描述符表有三种，分别是 Segment Descriptor 段描述符 TSS Segment Descriptor 段描述符 Gate Descriptor 门描述符.


CPU(x86-32)中有一专门的gdtr寄存器用来存放GDT的地址，GDT存放在内存中。 正常情况下gdtr是唯一的，所以GDT也是唯一的。
与GDT有关的一个概念是 Selector 选择子，事实上是GDT的表索引，引来查找各类描述符。

与GDT有关的另一个概念是 平坦模型 ，现代操作系统包括本系统大多基于平坦模型进行内存管理，简单来说，平坦模型区别于曾经的分段式内存管理，不再区分各个内存段(例如数据段 代码段等)，所有段重叠在从0x0000 0000 到 0xFFFF FFFF的最大寻址范围。
对于x86-32来说，有一个从实模式跳到保护模式的过程，在保护模式中完全使用平坦模型管理内存。
在实模式下，各个段寄存器不再装段基址，改为装 Selector选择子。 由于是平坦模型，需要通过这样的方式来实现特权级分割。 
---


``` C

typedef struct _segment_desc_t {
	uint16_t limit15_0;
	uint16_t base15_0;
	uint8_t base23_16;
	uint16_t attr;
	uint8_t base31_24;
}segment_desc_t;

```
### 关于任务切换的疑惑现在得到解答

为什么 `switch_to_tss` 具有很简洁的形式但能完成任务切换 其实是因为底层的ljmpl指令是复合指令

```C
/* cpu.c */
void switch_to_tss(int tss_sel){
    far_jump(tss_sel,0);
}

/* cpu_instr.h */
static void far_jump(uint32_t selector, uint32_t offset) {
	uint32_t addr[] = {offset, selector };
	asm volatile("ljmpl *(%[a])"::[a]"r"(addr));
}

```


``` C
/**
 * ljmpl 是复合指令 不仅仅跳转，cpu寄存器的存取也集成在这条指令当中
 * 我尝试观察这条指令在反汇编中的状态 。
 * 因为使用inline关键字，但反汇编代码中仍然使用 `call` 导致实际上的 ljmpl被隐藏了
 * 但ljmpl的三个作用是不会错的 
 * 1 保存当前任务状态
 * 2 读取新任务状态
 * 3 更新cpu寄存器 (完成跳转)
 * /
```

### 以下是部分反汇编

```
0001106d <switch_to_tss>:
void switch_to_tss(int tss_sel){
   1106d:	55                   	push   %ebp
   1106e:	89 e5                	mov    %esp,%ebp
    far_jump(tss_sel,0);
   11070:	8b 45 08             	mov    0x8(%ebp),%eax
   11073:	6a 00                	push   $0x0
   11075:	50                   	push   %eax
   11076:	e8 f6 fd ff ff       	call   10e71 <far_jump>
   1107b:	83 c4 08             	add    $0x8,%esp
}
```

### 通过袪掉 inline 关键字 在反汇编文件中找到了ljmpl对应的反汇编码

```
000118fe <far_jump>:
   118fe:	55                   	push   %ebp
   118ff:	89 e5                	mov    %esp,%ebp
   11901:	83 ec 10             	sub    $0x10,%esp
   11904:	8b 45 0c             	mov    0xc(%ebp),%eax
   11907:	89 45 f8             	mov    %eax,-0x8(%ebp)
   1190a:	8b 45 08             	mov    0x8(%ebp),%eax
   1190d:	89 45 fc             	mov    %eax,-0x4(%ebp)
   11910:	8d 45 f8             	lea    -0x8(%ebp),%eax
   11913:	ff 28                	ljmp   *(%eax)		就是这一行了 ljmp == ljmpl 上面是far_jump函数的传参过程
   11915:	90                   	nop
   11916:	c9                   	leave
   11917:	c3                   	ret```

``` 