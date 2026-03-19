/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 429
 * Language: c
 * Block ID: 24054844
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 符号表项
typedef struct {
    uint32_t st_name;   // 符号名字符串表索引
    uint8_t  st_info;   // 类型和绑定属性
    uint8_t  st_other;  // 保留
    uint16_t st_shndx;  // 所在节索引
    uint64_t st_value;  // 符号值 (地址/偏移)
    uint64_t st_size;   // 符号大小
} Elf64_Sym;

// 重定位表项 (含加数)
typedef struct {
    uint64_t r_offset;  // 重定位位置
    uint64_t r_info;    // 符号索引和重定位类型
    int64_t  r_addend;  // 加数
} Elf64_Rela;

// 常用重定位类型 (x86-64)
#define R_X86_64_64    1   // 直接64位
#define R_X86_64_PC32  2   // PC相对32位
#define R_X86_64_PLT32 4   // PLT相对32位
#define R_X86_64_RELATIVE 8 // 基址相对
