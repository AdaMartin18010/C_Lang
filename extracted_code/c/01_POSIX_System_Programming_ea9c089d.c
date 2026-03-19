/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 2241
 * Language: c
 * Block ID: ea9c089d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_signals.c
 * @brief POSIX信号处理
 * @description 包含信号安装、发送、阻塞、等待等操作
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

static volatile sig_atomic_t signal_received = 0;

/**
 * @brief 简单的信号处理函数
 * @param sig 信号编号
 */
void simple_handler(int sig) {
    signal_received = sig;
    printf("\n收到信号 %d (%s)\n", sig, strsignal(sig));
}

/**
 * @brief 使用sigaction安装信号处理器
 */
void sigaction_demo(void) {
    struct sigaction sa;

    printf("\n=== sigaction信号处理 ===\n");

    /* 清空信号处理结构 */
    memset(&sa, 0, sizeof(sa));

    /* 设置信号处理函数 */
    sa.sa_handler = simple_handler;

    /* 设置信号掩码（处理信号时阻塞的信号） */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGTERM);

    /* 设置标志 */
    sa.sa_flags = SA_RESTART;  /* 系统调用自动重启 */

    /* 安装SIGUSR1处理器 */
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return;
    }

    printf("已安装SIGUSR1处理器\n");
    printf("发送SIGUSR1给自己...\n");
    raise(SIGUSR1);

    sleep(1);
}

/**
 * @brief 信号集操作
 */
void signal_set_operations(void) {
    sigset_t set, oldset;

    printf("\n=== 信号集操作 ===\n");

    /* 清空信号集 */
    sigemptyset(&set);
    printf("清空信号集\n");

    /* 添加信号到集合 */
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGTERM);
    printf("添加SIGINT, SIGQUIT, SIGTERM到集合\n");

    /* 检查信号是否在集合中 */
    if (sigismember(&set, SIGINT)) {
        printf("SIGINT在集合中\n");
    }

    /* 从集合中删除信号 */
    sigdelset(&set, SIGQUIT);
    printf("从集合中删除SIGQUIT\n");

    if (!sigismember(&set, SIGQUIT)) {
        printf("SIGQUIT已不在集合中\n");
    }

    /* 填充信号集（包含所有信号） */
    sigfillset(&set);
    printf("填充信号集（包含所有信号）\n");
}

/**
 * @brief 信号阻塞和等待
 */
void signal_blocking_demo(void) {
    sigset_t block_set, pending_set;

    printf("\n=== 信号阻塞演示 ===\n");

    /* 准备阻塞的信号集 */
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);
    sigaddset(&block_set, SIGUSR1);

    /* 阻塞信号 */
    printf("阻塞SIGINT和SIGUSR1...\n");
    if (sigprocmask(SIG_BLOCK, &block_set, NULL) == -1) {
        perror("sigprocmask");
        return;
    }

    /* 发送信号给自己 */
    printf("发送SIGUSR1（已被阻塞）...\n");
    raise(SIGUSR1);

    /* 检查待处理信号 */
    if (sigpending(&pending_set) == -1) {
        perror("sigpending");
    } else {
        if (sigismember(&pending_set, SIGUSR1)) {
            printf("SIGUSR1处于待处理状态\n");
        }
    }

    /* 解除阻塞 */
    printf("解除阻塞，信号将被传递...\n");
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);

    sleep(1);
}

/**
 * @brief 使用sigsuspend等待信号
 */
void sigsuspend_demo(void) {
    sigset_t mask, oldmask;

    printf("\n=== sigsuspend演示 ===\n");

    /* 设置SIGUSR1处理器 */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = simple_handler;
    sigaction(SIGUSR1, &sa, NULL);

    /* 准备掩码 */
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    /* 阻塞SIGUSR1 */
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    printf("SIGUSR1已被阻塞，准备sigsuspend...\n");
    printf("在另一个终端运行: kill -USR1 %d\n", getpid());
    printf("等待信号（10秒超时）...\n");

    /* 创建计时器子进程 */
    pid_t timer_pid = fork();
    if (timer_pid == 0) {
        sleep(10);
        kill(getppid(), SIGUSR1);  /* 超时发送信号 */
        exit(0);
    }

    /* 临时解除阻塞并暂停等待信号 */
    sigsuspend(&oldmask);

    printf("从sigsuspend返回\n");

    /* 恢复原始掩码 */
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    if (timer_pid > 0) {
        kill(timer_pid, SIGTERM);
        wait(NULL);
    }
}

/**
 * @brief 使用sigtimedwait同步等待信号
 */
void sigtimedwait_demo(void) {
    sigset_t set;
    siginfo_t info;
    struct timespec timeout;

    printf("\n=== sigtimedwait演示 ===\n");

    /* 准备等待的信号集 */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    /* 阻塞这些信号 */
    sigprocmask(SIG_BLOCK, &set, NULL);

    /* 设置超时 */
    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;

    printf("等待SIGUSR1或SIGUSR2（5秒超时）...\n");
    printf("发送命令: kill -USR1 %d\n", getpid());

    /* 发送信号的子进程 */
    pid_t pid = fork();
    if (pid == 0) {
        sleep(2);
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    /* 同步等待信号 */
    int sig = sigtimedwait(&set, &info, &timeout);

    if (sig == -1) {
        if (errno == EAGAIN) {
            printf("超时，未收到信号\n");
        } else {
            perror("sigtimedwait");
        }
    } else {
        printf("收到信号 %d (%s)\n", sig, strsignal(sig));
        printf("发送者PID: %d\n", info.si_pid);
        printf("信号代码: %d\n", info.si_code);
    }

    wait(NULL);

    /* 解除阻塞 */
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

/**
 * @brief 信号处理器安全函数示例
 */
volatile sig_atomic_t alarm_count = 0;

void alarm_handler(int sig) {
    /* 信号处理器中只能使用异步信号安全函数 */
    alarm_count++;
    /* write是异步信号安全的 */
    const char msg[] = "\n闹钟信号!\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

void alarm_demo(void) {
    struct sigaction sa;

    printf("\n=== 闹钟信号演示 ===\n");

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGALRM, &sa, NULL);

    printf("设置闹钟1秒后响铃...\n");
    alarm(1);

    /* 模拟工作 */
    for (int i = 0; i < 5; i++) {
        printf("工作中... %d\n", i + 1);
        sleep(1);
    }

    printf("闹钟响起次数: %d\n", alarm_count);
}

/**
 * @brief 忽略和默认处理
 */
void signal_ignore_default_demo(void) {
    printf("\n=== 忽略和默认处理 ===\n");

    /* 忽略SIGPIPE */
    signal(SIGPIPE, SIG_IGN);
    printf("SIGPIPE已忽略\n");

    /* 恢复默认处理 */
    signal(SIGPIPE, SIG_DFL);
    printf("SIGPIPE恢复默认处理\n");
}

int main(void) {
    printf("=== POSIX信号处理演示 ===\n");
    printf("当前进程PID: %d\n", getpid());

    sigaction_demo();
    signal_set_operations();
    signal_blocking_demo();
    /* sigsuspend_demo(); */  /* 需要交互，可选运行 */
    /* sigtimedwait_demo(); */  /* 需要交互，可选运行 */
    alarm_demo();
    signal_ignore_default_demo();

    printf("\n=== 信号处理演示完成 ===\n");
    return 0;
}
