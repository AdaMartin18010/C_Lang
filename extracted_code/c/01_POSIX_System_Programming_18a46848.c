/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 2552
 * Language: c
 * Block ID: 18a46848
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_realtime_signals.c
 * @brief POSIX实时信号
 * @description 演示实时信号的特性：排队、带数据
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/**
 * @brief 实时信号处理器
 * @param sig 信号编号
 * @param info 信号信息
 * @param context 上下文
 */
void realtime_handler(int sig, siginfo_t *info, void *context) {
    (void)context;
    printf("实时信号: %d, 数据: %d, 来自PID: %d\n",
           sig, info->si_value.sival_int, info->si_pid);
}

/**
 * @brief 实时信号排队演示
 */
void realtime_signal_queue_demo(void) {
    struct sigaction sa;
    sigset_t set;

    printf("\n=== 实时信号排队演示 ===\n");

    /* 设置实时信号处理器 */
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = realtime_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    /* 安装SIGRTMIN处理器 */
    sigaction(SIGRTMIN, &sa, NULL);

    /* 阻塞实时信号 */
    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &set, NULL);

    /* 发送多个带数据的实时信号 */
    printf("发送5个SIGRTMIN信号...\n");
    for (int i = 1; i <= 5; i++) {
        union sigval value;
        value.sival_int = i * 100;

        if (sigqueue(getpid(), SIGRTMIN, value) == -1) {
            perror("sigqueue");
        }
    }

    printf("信号已发送，解除阻塞接收...\n");
    sigprocmask(SIG_UNBLOCK, &set, NULL);

    sleep(1);  /* 给处理器执行时间 */

    printf("所有实时信号已处理\n");
}

/**
 * @brief 比较标准信号和实时信号
 */
void compare_signals(void) {
    struct sigaction sa;
    sigset_t set;
    volatile int count = 0;

    printf("\n=== 标准信号vs实时信号 ===\n");

    /* 标准信号处理器 */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = simple_counter;
    sigaction(SIGUSR1, &sa, NULL);

    /* 阻塞信号 */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);

    /* 发送多个标准信号 */
    printf("发送5个SIGUSR1（标准信号）...\n");
    for (int i = 0; i < 5; i++) {
        kill(getpid(), SIGUSR1);
    }

    sigprocmask(SIG_UNBLOCK, &set, NULL);
    printf("标准信号不会排队，可能只处理一次\n");

    sleep(1);
}

void simple_counter(int sig) {
    static int count = 0;
    printf("标准信号 %d 处理次数: %d\n", sig, ++count);
}

int main(void) {
    printf("=== POSIX实时信号演示 ===\n");

    printf("SIGRTMIN = %d\n", SIGRTMIN);
    printf("SIGRTMAX = %d\n", SIGRTMAX);
    printf("可用实时信号数量: %d\n", SIGRTMAX - SIGRTMIN + 1);

    realtime_signal_queue_demo();
    /* compare_signals(); */

    printf("\n=== 实时信号演示完成 ===\n");
    return 0;
}
