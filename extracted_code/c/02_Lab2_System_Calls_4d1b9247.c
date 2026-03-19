/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 308
 * Language: c
 * Block ID: 4d1b9247
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sysproc.c
#include "sysinfo.h"

uint64
sys_sysinfo(void)
{
    uint64 addr;
    struct sysinfo info;

    // 获取用户态地址参数
    if (argaddr(0, &addr) < 0)
        return -1;

    // 填充信息
    info.freemem = count_freemem();
    info.nproc = count_nproc();

    // 复制到用户态
    if (copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0)
        return -1;

    return 0;
}
