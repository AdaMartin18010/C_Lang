/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 498
 * Language: c
 * Block ID: 7b05d74d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file advanced_file_io.c
 * @brief 高级POSIX文件I/O技术
 * @description 包含内存映射、文件锁定、非阻塞I/O、I/O多路复用
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

#define FILE_SIZE 4096

/**
 * @brief 内存映射文件示例
 * @description 使用mmap将文件映射到进程地址空间
 */
void memory_mapped_io_demo(void) {
    int fd;
    char *mapped;
    struct stat sb;
    const char *filename = "mmap_test.dat";
    const char *test_data = "Memory mapped I/O test data from POSIX.1-2024.";

    printf("\n=== 内存映射I/O (mmap) ===\n");

    /* 创建测试文件 */
    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return;
    }

    /* 扩展文件到指定大小 */
    if (ftruncate(fd, FILE_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        return;
    }

    /* 将文件映射到内存 */
    mapped = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return;
    }

    /* 关闭文件描述符，映射仍然有效 */
    close(fd);

    /* 通过内存写入数据 */
    strcpy(mapped, test_data);
    printf("写入内存映射区域: %s\n", mapped);

    /* 修改数据 */
    mapped[0] = toupper(mapped[0]);
    printf("修改后: %s\n", mapped);

    /* 同步映射到文件 */
    if (msync(mapped, FILE_SIZE, MS_SYNC) == -1) {
        perror("msync");
    }

    /* 解除映射 */
    if (munmap(mapped, FILE_SIZE) == -1) {
        perror("munmap");
    }

    /* 清理 */
    unlink(filename);
    printf("内存映射演示完成\n");
}

/**
 * @brief 文件锁演示
 * @description 使用fcntl进行建议性文件锁定
 */
void file_locking_demo(void) {
    int fd;
    struct flock fl;
    const char *filename = "lock_test.dat";

    printf("\n=== 文件锁定 (fcntl) ===\n");

    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return;
    }

    /* 写入测试数据 */
    write(fd, "Test data for locking", 22);

    /* 设置文件锁结构 */
    fl.l_type = F_WRLCK;    /* 写锁（独占锁） */
    fl.l_whence = SEEK_SET; /* 从文件开头 */
    fl.l_start = 0;         /* 从偏移0开始 */
    fl.l_len = 10;          /* 锁定前10个字节 */

    /* 尝试获取锁 */
    printf("尝试获取写锁...\n");
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            printf("无法获取锁（已被其他进程持有）\n");
        } else {
            perror("fcntl F_SETLK");
        }
    } else {
        printf("成功获取写锁（偏移0-10）\n");

        /* 演示：将锁转换为读锁 */
        fl.l_type = F_RDLCK;
        if (fcntl(fd, F_SETLK, &fl) == -1) {
            perror("fcntl F_SETLK (转换)");
        } else {
            printf("锁已转换为读锁\n");
        }

        /* 释放锁 */
        fl.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLK, &fl) == -1) {
            perror("fcntl F_SETLK (解锁)");
        } else {
            printf("锁已释放\n");
        }
    }

    close(fd);
    unlink(filename);
}

/**
 * @brief 非阻塞I/O演示
 */
void nonblocking_io_demo(void) {
    int fd;
    int flags;
    char buffer[256];
    ssize_t n;

    printf("\n=== 非阻塞I/O ===\n");

    /* 打开标准输入为非阻塞模式 */
    fd = STDIN_FILENO;

    /* 获取当前标志 */
    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return;
    }

    /* 设置非阻塞标志 */
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return;
    }

    printf("尝试非阻塞读取（5次）...\n");
    for (int i = 0; i < 5; i++) {
        n = read(fd, buffer, sizeof(buffer) - 1);
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("第%d次: 无数据可用 (EAGAIN)\n", i + 1);
            } else {
                perror("read");
            }
        } else if (n == 0) {
            printf("第%d次: EOF\n", i + 1);
        } else {
            buffer[n] = '\0';
            printf("第%d次: 读取到 '%s'\n", i + 1, buffer);
        }
        usleep(500000);  /* 等待500ms */
    }

    /* 恢复阻塞模式 */
    fcntl(fd, F_SETFL, flags);
    printf("恢复阻塞模式\n");
}

/**
 * @brief 使用select进行I/O多路复用
 */
void select_demo(void) {
    fd_set readfds;
    struct timeval timeout;
    int retval;
    char buffer[256];

    printf("\n=== I/O多路复用 (select) ===\n");
    printf("等待标准输入5秒...\n");

    /* 清除fd_set */
    FD_ZERO(&readfds);
    /* 添加标准输入到集合 */
    FD_SET(STDIN_FILENO, &readfds);

    /* 设置超时时间为5秒 */
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    /* 等待I/O就绪 */
    retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

    if (retval == -1) {
        perror("select");
    } else if (retval == 0) {
        printf("超时：5秒内无输入\n");
    } else {
        /* 检查是否是标准输入就绪 */
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                printf("读取到: %s", buffer);
            }
        }
    }
}

/**
 * @brief 文件描述符复制演示
 */
void dup_demo(void) {
    int fd, fd_dup, fd_dup2;
    const char *filename = "dup_test.dat";

    printf("\n=== 文件描述符复制 (dup/dup2) ===\n");

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return;
    }
    printf("原始fd: %d\n", fd);

    /* dup: 复制到最小的可用fd */
    fd_dup = dup(fd);
    if (fd_dup == -1) {
        perror("dup");
        close(fd);
        return;
    }
    printf("dup后fd: %d\n", fd_dup);

    /* dup2: 复制到指定的fd */
    fd_dup2 = dup2(fd, 100);  /* 复制到fd 100 */
    if (fd_dup2 == -1) {
        perror("dup2");
    } else {
        printf("dup2后fd: %d\n", fd_dup2);
        close(fd_dup2);
    }

    close(fd);
    close(fd_dup);
    unlink(filename);
}

int main(void) {
    printf("=== POSIX高级文件I/O技术演示 ===\n");

    memory_mapped_io_demo();
    file_locking_demo();
    /* nonblocking_io_demo(); */  /* 需要交互，暂时注释 */
    /* select_demo(); */          /* 需要交互，暂时注释 */
    dup_demo();

    printf("\n=== 高级I/O演示完成 ===\n");
    return 0;
}
