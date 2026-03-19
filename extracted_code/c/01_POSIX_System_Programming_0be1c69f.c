/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 60
 * Language: c
 * Block ID: 0be1c69f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_version_check.c
 * @brief POSIX版本检查示例
 * @description 演示如何检查系统支持的POSIX版本
 */

#define _POSIX_C_SOURCE 202405L  /* POSIX.1-2024 */
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>

/**
 * @brief 打印POSIX系统信息
 */
void print_posix_info(void) {
    struct utsname sys_info;

    printf("=== POSIX系统信息 ===\n");

    /* 检查POSIX版本 */
#ifdef _POSIX_VERSION
    printf("POSIX版本: %ld\n", (long)_POSIX_VERSION);
    if (_POSIX_VERSION >= 202405L) {
        printf("系统支持POSIX.1-2024标准\n");
    } else if (_POSIX_VERSION >= 200809L) {
        printf("系统支持POSIX.1-2008标准\n");
    }
#else
    printf("未定义_POSIX_VERSION\n");
#endif

    /* 检查系统配置 */
    printf("\n=== 系统限制 ===\n");
    printf("最大打开文件数: %ld\n", sysconf(_SC_OPEN_MAX));
    printf("每用户最大进程数: %ld\n", sysconf(_SC_CHILD_MAX));
    printf("系统时钟频率: %ld ticks/second\n", sysconf(_SC_CLK_TCK));
    printf("页面大小: %ld bytes\n", sysconf(_SC_PAGESIZE));
    printf("物理页数: %ld\n", sysconf(_SC_PHYS_PAGES));
    printf("可用物理页数: %ld\n", sysconf(_SC_AVPHYS_PAGES));

    /* 获取系统信息 */
    if (uname(&sys_info) == 0) {
        printf("\n=== Uname信息 ===\n");
        printf("操作系统: %s\n", sys_info.sysname);
        printf("节点名: %s\n", sys_info.nodename);
        printf("发布版本: %s\n", sys_info.release);
        printf("版本: %s\n", sys_info.version);
        printf("硬件: %s\n", sys_info.machine);
    }
}

/**
 * @brief 检查各种POSIX选项支持
 */
void check_posix_options(void) {
    printf("\n=== POSIX选项支持 ===\n");

    /* 实时信号 */
#ifdef _POSIX_REALTIME_SIGNALS
    printf("实时信号: 支持\n");
#else
    printf("实时信号: 不支持\n");
#endif

    /* 优先级调度 */
#ifdef _POSIX_PRIORITY_SCHEDULING
    printf("优先级调度: 支持\n");
#else
    printf("优先级调度: 不支持\n");
#endif

    /* 共享内存 */
#ifdef _POSIX_SHARED_MEMORY_OBJECTS
    printf("共享内存对象: 支持\n");
#else
    printf("共享内存对象: 不支持\n");
#endif

    /* 同步I/O */
#ifdef _POSIX_SYNCHRONIZED_IO
    printf("同步I/O: 支持\n");
#else
    printf("同步I/O: 不支持\n");
#endif

    /* 异步I/O */
#ifdef _POSIX_ASYNCHRONOUS_IO
    printf("异步I/O: 支持\n");
#else
    printf("异步I/O: 不支持\n");
#endif

    /* 内存锁定 */
#ifdef _POSIX_MEMLOCK
    printf("内存锁定: 支持\n");
#else
    printf("内存锁定: 不支持\n");
#endif
}

int main(void) {
    print_posix_info();
    check_posix_options();
    return 0;
}
