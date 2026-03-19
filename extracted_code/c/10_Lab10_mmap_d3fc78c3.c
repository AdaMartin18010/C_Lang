/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 252
 * Language: c
 * Block ID: d3fc78c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/trap.c

void
usertrap(void)
{
    // ... 原有代码 ...

    uint64 cause = r_scause();

    if (cause == 8) {
        // 系统调用
        // ...
    } else if (cause == 13 || cause == 15) {
        // 页面错误
        uint64 va = r_stval();

        // 先尝试COW处理 (Lab 5)
        if (cow_handler(va) == 0) {
            // COW处理成功
        } else if (mmap_handler(va) == 0) {
            // mmap处理成功
        } else {
            printf("page fault: va=%p\n", va);
            setkilled(p);
        }
    }

    // ...
}
