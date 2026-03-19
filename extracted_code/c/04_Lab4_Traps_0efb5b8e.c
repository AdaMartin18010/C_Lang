/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 287
 * Language: c
 * Block ID: 0efb5b8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/printf.c

#include "kernel/riscv.h"
#include "kernel/defs.h"

// 获取当前帧指针
static inline uint64
r_fp()
{
    uint64 x;
    asm volatile("mv %0, s0" : "=r" (x));
    return x;
}

void
backtrace(void)
{
    printf("backtrace:\n");

    // 获取当前帧指针
    uint64 fp = r_fp();

    // 栈范围检查 (确保不越界)
    // PGROUNDUP用于获取栈顶，栈向下增长
    uint64 stack_top = PGROUNDUP(fp);

    while (fp < stack_top && fp > stack_top - PGSIZE) {
        // 获取返回地址: 保存在fp + 8处
        uint64 ra = *(uint64*)(fp - 8);

        printf("%p\n", ra);

        // 移动到上一帧
        fp = *(uint64*)(fp - 16);
    }
}
