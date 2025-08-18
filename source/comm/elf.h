/*
* 实际上linux各个发行版 /usr/include 目录下应该有类似这个的头文件 这个
* 这个头文件也基本就是照抄。
* 可以使用 man 指令查看elf相关
* 所以这个头文件全抄也有一点道理
*/

#ifndef _ELF_H
#define _ELF_H

#include "types.h"

#define EI_NIDENT 16
#define ELF_MAGIC       0x7F

#define ET_EXEC         2   // 可执行文件
#define ET_386          3   // 80386处理器

#define PT_LOAD         1   // 可加载类型

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

/*
结构体 Elf32_Ehdr 是 ELF文件头（ELF Header） 的定义。
ELF 文件头是 ELF 文件的起始部分，包含了描述整个文件的关键信息，例如文件类型、目标架构、入口点地址、程序头表和节头表的位置等。
*/

#pragma pack(1)     /*设置结构体内存对齐方式*/
typedef struct {
    char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
}Elf32_Ehdr;

#define PT_LOAD     1
typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;
/* 这个结构体就是 程序头表 ，程序头表就是指这个结构体
 * Elf32_Phdr结构体用于描述 ELF 文件中的一个段（Segment）。
 * 程序头表（Program Header Table）由多个 Elf32_Phdr 条目组成，每个条目对应一个段。
 * 操作系统或加载器会根据这些条目将 ELF 文件的段加载到内存中，并设置适当的内存权限。
*/
#pragma pack()

#endif