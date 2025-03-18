其实对ELF 文件格式一无所知
有必要了解下
我花了十分钟完全了解了ELF 文件

首先 elf可以算作是一种文件格式
按照这种格式的文件可以执行 可以连接 elf文件还可以充当链接库

ELF Header   
``` C
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
/*这个结构体用来描述elf文件头 Elf Header
```


``` C
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
/* 这个结构体用来描述elf文件中的 段segmentation */ 
```   

这样的结构体构成程序头表 Program Header Table 的元素，
程序头表（Program Header Table） 是由 Elf32_Phdr 或 Elf64_Phdr 结构体组成的数组。


