/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 279
 * Language: c
 * Block ID: 6d74a698
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
        // 13: 加载页面错误
        // 15: 存储/AMO页面错误

        uint64 va = r_stval();  // 错误虚拟地址

        // 尝试处理COW页面错误
        if (cow_handler(va) == 0) {
            // 成功处理
        } else {
            printf("usertrap(): unexpected page fault pa=%p\n", va);
            setkilled(p);
        }
    } else if ((which_dev = devintr()) != 0) {
        // 设备中断
        // ...
    } else {
        // 其他异常
        // ...
    }

    // ...
}
