/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2825
 * Language: c
 * Block ID: 5071e125
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例: 产生僵尸进程
void bad_fork_server() {
    while (1) {
        int client = accept(listen_fd, ...);
        if (fork() == 0) {
            // 子进程
            close(listen_fd);
            handle_client(client);
            close(client);
            exit(0);  // 子进程终止
        }
        close(client);
        // 父进程没有wait,子进程变成僵尸进程!
    }
}

// ✅ 正确做法1: signal处理SIGCHLD
#include <sys/wait.h>

void sigchld_handler(int sig) {
    // WNOHANG: 非阻塞
    // -1: 等待任何子进程
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void good_sigchld_handler() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // 被中断的系统调用自动重启
    sigaction(SIGCHLD, &sa, NULL);

    // 或者使用signal(较简单但不适用于所有情况)
    // signal(SIGCHLD, sigchld_handler);
}

// ✅ 正确做法2: 忽略SIGCHLD (Linux特有)
void good_ignore_sigchld() {
    // Linux中忽略SIGCHLD会自动处理僵尸进程
    signal(SIGCHLD, SIG_IGN);
}

// ✅ 正确做法3: 双fork避免僵尸进程
void good_double_fork() {
    while (1) {
        int client = accept(listen_fd, ...);

        pid_t pid = fork();
        if (pid == 0) {
            // 第一层子进程
            if (fork() == 0) {
                // 第二层子进程(孤儿进程,被init收养)
                close(listen_fd);
                handle_client(client);
                exit(0);
            }
            // 第一层子进程立即退出
            exit(0);
        }

        close(client);
        waitpid(pid, NULL, 0);  // 收割第一层子进程
    }
}
