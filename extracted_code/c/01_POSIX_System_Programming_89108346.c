/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 1173
 * Language: c
 * Block ID: 89108346
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_process.c
 * @brief POSIX进程控制
 * @description 包含fork、exec、wait、进程控制等操作
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

/**
 * @brief 基本的fork示例
 * @description 演示如何创建子进程
 */
void fork_basic_demo(void) {
    pid_t pid;
    int x = 100;

    printf("\n=== 基本fork演示 ===\n");
    printf("父进程: PID=%d, x=%d\n", getpid(), x);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        /* 子进程 */
        printf("子进程: PID=%d, 父PID=%d, x=%d\n",
               getpid(), getppid(), x);
        x = 200;  /* 修改不影响父进程 */
        printf("子进程修改x后: x=%d\n", x);
        exit(0);
    } else {
        /* 父进程 */
        printf("父进程: 子PID=%d, x=%d\n", pid, x);
        wait(NULL);  /* 等待子进程结束 */
        printf("父进程: 子进程已结束, x=%d\n", x);
    }
}

/**
 * @brief 使用wait获取子进程退出状态
 */
void wait_status_demo(void) {
    pid_t pid;
    int status;

    printf("\n=== wait状态演示 ===\n");

    pid = fork();
    if (pid == 0) {
        /* 子进程 - 正常退出，返回42 */
        exit(42);
    }

    /* 父进程等待 */
    wait(&status);

    if (WIFEXITED(status)) {
        printf("子进程正常退出，退出码: %d\n", WEXITSTATUS(status));
    }

    /* 子进程被信号终止 */
    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        while (1) {
            sleep(1);
        }
    }

    sleep(1);
    kill(pid, SIGTERM);  /* 发送终止信号 */
    wait(&status);

    if (WIFSIGNALED(status)) {
        printf("子进程被信号终止，信号: %d (%s)\n",
               WTERMSIG(status), strsignal(WTERMSIG(status)));
    }
}

/**
 * @brief 创建多个子进程并等待
 */
void multiple_children_demo(void) {
    pid_t pids[5];
    int status;

    printf("\n=== 多子进程演示 ===\n");

    for (int i = 0; i < 5; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
            /* 子进程 */
            printf("子进程 %d 启动，PID=%d\n", i, getpid());
            sleep(1 + i);  /* 不同的睡眠时间 */
            printf("子进程 %d 结束\n", i);
            exit(i);  /* 返回索引作为退出码 */
        }
    }

    /* 父进程：等待所有子进程 */
    printf("父进程等待所有子进程...\n");

    for (int i = 0; i < 5; i++) {
        pid_t pid = wait(&status);
        if (WIFEXITED(status)) {
            printf("子进程 PID=%d 退出，退出码: %d\n",
                   pid, WEXITSTATUS(status));
        }
    }

    printf("所有子进程已结束\n");
}

/**
 * @brief exec系列函数演示
 */
void exec_demo(void) {
    pid_t pid;

    printf("\n=== exec系列函数演示 ===\n");

    /* execl: 参数逐个传递 */
    pid = fork();
    if (pid == 0) {
        printf("使用execl执行ls -l /tmp\n");
        execl("/bin/ls", "ls", "-l", "/tmp", NULL);
        perror("execl");  /* 只有出错才会到这里 */
        exit(1);
    }
    wait(NULL);

    /* execvp: 在PATH中搜索程序 */
    pid = fork();
    if (pid == 0) {
        char *args[] = {"echo", "Hello", "from", "execvp", NULL};
        execvp("echo", args);
        perror("execvp");
        exit(1);
    }
    wait(NULL);

    /* execv: 指定完整路径 */
    pid = fork();
    if (pid == 0) {
        char *args[] = {"date", NULL};
        execv("/bin/date", args);
        perror("execv");
        exit(1);
    }
    wait(NULL);
}

/**
 * @brief 使用execle传递环境变量
 */
void exec_environment_demo(void) {
    pid_t pid;
    char *envp[] = {
        "MY_VAR=Hello",
        "PATH=/bin:/usr/bin",
        NULL
    };

    printf("\n=== 环境变量传递演示 ===\n");

    pid = fork();
    if (pid == 0) {
        /* 使用execle传递自定义环境变量 */
        execle("/usr/bin/env", "env", NULL, envp);
        perror("execle");
        exit(1);
    }
    wait(NULL);
}

/**
 * @brief 使用vfork的注意事项
 * @description vfork会共享地址空间，子进程必须先执行exec或_exit
 */
void vfork_caution_demo(void) {
    pid_t pid;
    int x = 1;

    printf("\n=== vfork演示（注意：共享地址空间）===\n");

    pid = vfork();
    if (pid == 0) {
        /* 子进程：共享父进程的地址空间 */
        x = 100;  /* 这会修改父进程的x */
        printf("子进程中 x = %d\n", x);
        _exit(0);  /* 必须使用_exit，而不是exit */
    }

    /* 父进程 */
    printf("父进程中 x = %d (被子进程修改了)\n", x);
}

/**
 * @brief 进程组与会话控制
 */
void process_group_demo(void) {
    pid_t pid;

    printf("\n=== 进程组与会话 ===\n");
    printf("当前进程PID: %d\n", getpid());
    printf("当前进程组ID: %d\n", getpgrp());
    printf("当前会话ID: %d\n", getsid(0));

    pid = fork();
    if (pid == 0) {
        /* 子进程：创建新进程组 */
        if (setpgid(0, 0) == -1) {
            perror("setpgid");
        } else {
            printf("子进程新进程组ID: %d\n", getpgrp());
        }
        exit(0);
    }
    wait(NULL);
}

/**
 * @brief 使用system函数执行命令
 */
void system_demo(void) {
    int ret;

    printf("\n=== system函数演示 ===\n");

    ret = system("echo 'Hello from system()'");
    printf("system()返回值: %d\n", ret);

    /* 检查命令是否成功执行 */
    if (WIFEXITED(ret)) {
        printf("命令退出码: %d\n", WEXITSTATUS(ret));
    }
}

/**
 * @brief 守护进程创建示例
 * @description 演示如何创建标准的Unix守护进程
 */
void daemonize(void) {
    pid_t pid;
    int fd;

    printf("\n=== 守护进程创建 ===\n");

    /* 第一步：fork并退出父进程 */
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        /* 父进程退出 */
        printf("父进程退出，守护进程PID=%d\n", pid);
        exit(0);
    }

    /* 子进程继续 */

    /* 第二步：创建新会话 */
    if (setsid() == -1) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    /* 第三步：再次fork防止获取控制终端 */
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(0);
    }

    /* 第四步：更改工作目录到根目录 */
    if (chdir("/") == -1) {
        perror("chdir");
    }

    /* 第五步：设置文件权限掩码 */
    umask(0);

    /* 第六步：关闭标准文件描述符 */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* 可选：重定向到/dev/null */
    fd = open("/dev/null", O_RDWR);
    if (fd != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) {
            close(fd);
        }
    }

    printf("守护进程创建完成\n");
    /* 守护进程主循环 */
    /* while (1) { ... } */
}

int main(void) {
    printf("=== POSIX进程控制演示 ===\n");

    fork_basic_demo();
    wait_status_demo();
    multiple_children_demo();
    exec_demo();
    exec_environment_demo();
    /* vfork_caution_demo(); */  /* 谨慎使用 */
    process_group_demo();
    system_demo();
    /* daemonize(); */  /* 实际使用时取消注释 */

    printf("\n=== 进程控制演示完成 ===\n");
    return 0;
}
