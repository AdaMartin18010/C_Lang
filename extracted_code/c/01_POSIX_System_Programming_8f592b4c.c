/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 1518
 * Language: c
 * Block ID: 8f592b4c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_pipe.c
 * @brief POSIX管道通信
 * @description 包含匿名管道、命名管道(FIFO)的使用
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 256

/**
 * @brief 匿名管道基本使用
 * @description 父子进程通过管道通信
 */
void anonymous_pipe_demo(void) {
    int pipefd[2];  /* pipefd[0]读取端, pipefd[1]写入端 */
    pid_t pid;
    char buffer[BUFFER_SIZE];
    const char *message = "Hello from parent process!";

    printf("\n=== 匿名管道演示 ===\n");

    /* 创建管道 */
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        /* 子进程：从管道读取 */
        close(pipefd[1]);  /* 关闭写入端 */

        ssize_t n = read(pipefd[0], buffer, BUFFER_SIZE - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("子进程收到: %s\n", buffer);
        }

        close(pipefd[0]);
        exit(0);
    } else {
        /* 父进程：向管道写入 */
        close(pipefd[0]);  /* 关闭读取端 */

        write(pipefd[1], message, strlen(message));
        printf("父进程发送: %s\n", message);

        close(pipefd[1]);
        wait(NULL);
    }
}

/**
 * @brief 双向管道通信
 */
void bidirectional_pipe_demo(void) {
    int parent_to_child[2];
    int child_to_parent[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    printf("\n=== 双向管道通信 ===\n");

    /* 创建两个管道 */
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        close(parent_to_child[1]);  /* 关闭P2C写端 */
        close(child_to_parent[0]);  /* 关闭C2P读端 */

        /* 从父进程读取 */
        ssize_t n = read(parent_to_child[0], buffer, BUFFER_SIZE - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("子进程收到: %s\n", buffer);
        }

        /* 向父进程发送回复 */
        const char *reply = "Message received by child!";
        write(child_to_parent[1], reply, strlen(reply));

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(0);
    } else {
        /* 父进程 */
        close(parent_to_child[0]);  /* 关闭P2C读端 */
        close(child_to_parent[1]);  /* 关闭C2P写端 */

        /* 向子进程发送消息 */
        const char *msg = "Hello child, this is parent!";
        write(parent_to_child[1], msg, strlen(msg));
        printf("父进程发送: %s\n", msg);

        /* 从子进程读取回复 */
        ssize_t n = read(child_to_parent[0], buffer, BUFFER_SIZE - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("父进程收到: %s\n", buffer);
        }

        close(parent_to_child[1]);
        close(child_to_parent[0]);
        wait(NULL);
    }
}

/**
 * @brief 管道实现进程间文件传输
 */
void pipe_file_transfer_demo(void) {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    const char *testfile = "pipe_transfer_test.txt";

    printf("\n=== 管道文件传输 ===\n");

    /* 创建测试文件 */
    FILE *fp = fopen(testfile, "w");
    if (fp) {
        fprintf(fp, "Line 1: Test data for pipe transfer.\n");
        fprintf(fp, "Line 2: This demonstrates IPC.\n");
        fprintf(fp, "Line 3: End of test data.\n");
        fclose(fp);
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程：读取管道并输出 */
        close(pipefd[1]);

        printf("子进程输出:\n");
        ssize_t n;
        while ((n = read(pipefd[0], buffer, BUFFER_SIZE - 1)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }

        close(pipefd[0]);
        exit(0);
    } else {
        /* 父进程：读取文件并写入管道 */
        close(pipefd[0]);

        fp = fopen(testfile, "r");
        if (fp) {
            size_t n;
            while ((n = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
                write(pipefd[1], buffer, n);
            }
            fclose(fp);
        }

        close(pipefd[1]);
        wait(NULL);
    }

    remove(testfile);
}

/**
 * @brief 命名管道(FIFO)演示
 */
void named_pipe_demo(void) {
    const char *fifo_path = "/tmp/posix_fifo_demo";
    pid_t pid;
    char buffer[BUFFER_SIZE];

    printf("\n=== 命名管道(FIFO)演示 ===\n");

    /* 删除可能存在的旧FIFO */
    unlink(fifo_path);

    /* 创建FIFO */
    if (mkfifo(fifo_path, 0666) == -1) {
        perror("mkfifo");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程：读取FIFO */
        int fd = open(fifo_path, O_RDONLY);
        if (fd == -1) {
            perror("open (child)");
            exit(1);
        }

        ssize_t n = read(fd, buffer, BUFFER_SIZE - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("FIFO读取端收到: %s\n", buffer);
        }

        close(fd);
        exit(0);
    } else {
        /* 父进程：写入FIFO */
        int fd = open(fifo_path, O_WRONLY);
        if (fd == -1) {
            perror("open (parent)");
            wait(NULL);
            unlink(fifo_path);
            return;
        }

        const char *msg = "Hello through named pipe!";
        write(fd, msg, strlen(msg));
        printf("FIFO写入端发送: %s\n", msg);

        close(fd);
        wait(NULL);
    }

    unlink(fifo_path);
}

/**
 * @brief popen/pclose使用
 */
void popen_demo(void) {
    FILE *fp;
    char buffer[BUFFER_SIZE];

    printf("\n=== popen/pclose演示 ===\n");

    /* 执行命令并读取输出 */
    fp = popen("ls -la", "r");
    if (fp == NULL) {
        perror("popen");
        return;
    }

    printf("命令输出:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    int status = pclose(fp);
    if (status == -1) {
        perror("pclose");
    } else {
        printf("命令退出状态: %d\n", WEXITSTATUS(status));
    }
}

int main(void) {
    printf("=== POSIX管道通信演示 ===\n");

    anonymous_pipe_demo();
    bidirectional_pipe_demo();
    pipe_file_transfer_demo();
    named_pipe_demo();
    popen_demo();

    printf("\n=== 管道通信演示完成 ===\n");
    return 0;
}
