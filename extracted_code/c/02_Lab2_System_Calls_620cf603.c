/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 137
 * Language: c
 * Block ID: 620cf603
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sysproc.c
uint64
sys_trace(void)
{
    int mask;

    // 从用户态获取参数
    if (argint(0, &mask) < 0)
        return -1;

    // 设置当前进程的跟踪掩码
    myproc()->trace_mask = mask;

    return 0;
}
