/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 81
 * Language: c
 * Block ID: ac218821
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/proc.h

// 虚拟内存区域结构
struct vma {
    int valid;              // 是否有效
    uint64 addr;            // 起始虚拟地址
    int length;             // 映射长度
    int prot;               // 保护标志 (R/W/X)
    int flags;              // 映射标志 (PRIVATE/SHARED)
    struct file *file;      // 映射的文件 (可为NULL)
    int offset;             // 文件偏移
};

#define NVMA 16  // 每个进程最大VMA数量

// 在proc结构中添加
struct proc {
    // ... 原有字段 ...
    struct vma vmas[NVMA];   // VMA数组
};

// 保护标志
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4

// 映射标志
#define MAP_SHARED  0x01
#define MAP_PRIVATE 0x02
#define MAP_FIXED   0x10
#define MAP_ANONYMOUS 0x20
