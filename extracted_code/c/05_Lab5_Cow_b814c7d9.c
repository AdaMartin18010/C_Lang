/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 91
 * Language: c
 * Block ID: b814c7d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/kalloc.c

// 物理内存分为4096字节的页框
// 定义引用计数数组
#define PHYSTOP 0x88000000   // 物理内存上限
#define KERNBASE 0x80000000  // 内核基地址

// 物理页框数量
#define NPAGES ((PHYSTOP - KERNBASE) / PGSIZE)

// 引用计数数组 (每个物理页一个计数)
int ref_count[NPAGES];
struct spinlock ref_lock;  // 保护引用计数

// 物理地址转引用计数索引
#define PA2IDX(pa) (((uint64)(pa) - KERNBASE) / PGSIZE)

// 初始化引用计数
void
kinit()
{
    initlock(&ref_lock, "ref");
    for (int i = 0; i < NPAGES; i++) {
        ref_count[i] = 1;  // 初始为1 (被内核持有)
    }
    // ... 原有freerange代码 ...
}
