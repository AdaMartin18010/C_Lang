/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 403
 * Language: c
 * Block ID: f1062136
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    uint32_t sh_name;       // 节名字符串表索引
    uint32_t sh_type;       // 节类型
    uint64_t sh_flags;      // 节标志
    uint64_t sh_addr;       // 运行时地址
    uint64_t sh_offset;     // 文件偏移
    uint64_t sh_size;       // 节大小
    uint32_t sh_link;       // 链接信息
    uint32_t sh_info;       // 额外信息
    uint64_t sh_addralign;  // 对齐要求
    uint64_t sh_entsize;    // 固定大小表项的大小
} Elf64_Shdr;

// 常见节类型
#define SHT_NULL     0   // 未使用
#define SHT_PROGBITS 1   // 程序数据
#define SHT_SYMTAB   2   // 符号表
#define SHT_STRTAB   3   // 字符串表
#define SHT_RELA     4   // 重定位表 (含加数)
#define SHT_HASH     5   // 符号哈希表
#define SHT_DYNAMIC  6   // 动态链接信息
