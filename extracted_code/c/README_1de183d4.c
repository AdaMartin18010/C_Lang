/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 381
 * Language: c
 * Block ID: 1de183d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    uint32_t p_type;    // 段类型 (LOAD/DYNAMIC/INTERP)
    uint32_t p_flags;   // 权限标志 (R/W/X)
    uint64_t p_offset;  // 文件偏移
    uint64_t p_vaddr;   // 虚拟地址
    uint64_t p_paddr;   // 物理地址 (通常未使用)
    uint64_t p_filesz;  // 文件大小
    uint64_t p_memsz;   // 内存大小
    uint64_t p_align;   // 对齐要求
} Elf64_Phdr;

// 常见段类型
#define PT_NULL     0   // 未使用
#define PT_LOAD     1   // 可加载段
#define PT_DYNAMIC  2   // 动态链接信息
#define PT_INTERP   3   // 解释器路径
#define PT_NOTE     4   // 辅助信息
