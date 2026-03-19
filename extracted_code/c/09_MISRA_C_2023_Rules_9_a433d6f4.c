/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 212
 * Language: c
 * Block ID: a433d6f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 安全的信号处理 */
#include <signal.h>
#include <stdatomic.h>

/* 使用原子标志 */
static volatile sig_atomic_t signal_received = 0;

/* 信号处理程序 - 只做最少的操作 */
void safe_signal_handler(int sig)
{
    (void)sig;
    signal_received = 1;  /* 只设置标志 */
}

/* 主循环中处理 */
void main_loop(void)
{
    signal(SIGTERM, safe_signal_handler);
    signal(SIGINT, safe_signal_handler);

    while (!signal_received) {
        /* 正常工作 */
        process_tasks();
    }

    /* 优雅退出 */
    cleanup_and_exit();
}
