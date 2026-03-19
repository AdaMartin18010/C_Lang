/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CERT_C_2024\README.md
 * Line: 278
 * Language: c
 * Block ID: d3101016
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码 - 命令注入
void run_command(const char* user_input) {
    char cmd[256];
    sprintf(cmd, "ls %s", user_input);  // 注入风险!
    system(cmd);
}

// 合规代码 - 使用 exec 系列函数
#include <unistd.h>
#include <sys/wait.h>

bool run_command_safe(const char* path, char* const args[]) {
    pid_t pid = fork();
    if (pid == -1) return false;

    if (pid == 0) {
        // 子进程
        execv(path, args);
        _exit(127);  // 如果 exec 失败
    }

    // 父进程等待
    int status;
    return waitpid(pid, &status, 0) != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0;
}
