/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 328
 * Language: c
 * Block ID: d0e4c02e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define EI_NIDENT 16

typedef struct {
    unsigned char e_ident[EI_NIDENT];  // 魔数、字长、字节序等
    uint16_t      e_type;              // 文件类型 (可执行/共享库/重定位)
    uint16_t      e_machine;           // 目标架构 (x86-64=62, ARM64=183)
    uint32_t      e_version;           // 版本
    uint64_t      e_entry;             // 入口点地址
    uint64_t      e_phoff;             // 程序头表偏移
    uint64_t      e_shoff;             // 节头表偏移
    uint32_t      e_flags;             // 处理器特定标志
    uint16_t      e_ehsize;            // ELF头大小
    uint16_t      e_phentsize;         // 程序头表项大小
    uint16_t      e_phnum;             // 程序头表项数
    uint16_t      e_shentsize;         // 节头表项大小
    uint16_t      e_shnum;             // 节头表项数
    uint16_t      e_shstrndx;          // 节名字符串表索引
} Elf64_Ehdr;
