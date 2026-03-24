# POSIX.1-2024 系统接口编程完全指南

## 目录

- [POSIX.1-2024 系统接口编程完全指南](#posix1-2024-系统接口编程完全指南)
  - [目录](#目录)
  - [1. POSIX概述和标准演进](#1-posix概述和标准演进)
    - [1.1 什么是POSIX](#11-什么是posix)
    - [1.2 POSIX标准演进历程](#12-posix标准演进历程)
    - [1.3 POSIX合规性检查宏](#13-posix合规性检查宏)
    - [1.4 编译和运行](#14-编译和运行)
  - [2. 文件I/O操作](#2-文件io操作)
    - [2.1 基础文件操作](#21-基础文件操作)
    - [2.2 高级文件I/O技术](#22-高级文件io技术)
  - [3. 文件系统操作](#3-文件系统操作)
    - [3.1 目录操作](#31-目录操作)
  - [4. 进程控制](#4-进程控制)
    - [4.1 进程创建与管理](#41-进程创建与管理)
  - [5. 进程间通信](#5-进程间通信)
    - [5.1 管道通信](#51-管道通信)
    - [5.2 System V IPC](#52-system-v-ipc)
    - [5.3 POSIX共享内存和信号量](#53-posix共享内存和信号量)
  - [6. 信号处理](#6-信号处理)
    - [6.1 基础信号处理](#61-基础信号处理)
    - [6.2 实时信号](#62-实时信号)
  - [7. 线程管理](#7-线程管理)
    - [7.1 POSIX线程基础](#71-posix线程基础)
  - [8. 同步原语](#8-同步原语)
    - [8.1 互斥锁](#81-互斥锁)
    - [8.2 条件变量和读写锁](#82-条件变量和读写锁)
  - [9. 网络Socket编程](#9-网络socket编程)
    - [9.1 TCP Socket编程](#91-tcp-socket编程)
    - [9.2 UDP Socket编程](#92-udp-socket编程)
  - [10. 实时扩展](#10-实时扩展)
    - [10.1 POSIX实时特性](#101-posix实时特性)
  - [11. 实际项目: POSIX兼容Shell实现](#11-实际项目-posix兼容shell实现)
    - [11.1 项目概述](#111-项目概述)
    - [11.2 完整Shell实现代码](#112-完整shell实现代码)
    - [11.3 Shell测试用例](#113-shell测试用例)
    - [11.4 Makefile](#114-makefile)
    - [11.5 使用示例](#115-使用示例)
  - [12. 附录](#12-附录)
    - [12.1 POSIX版本宏定义](#121-posix版本宏定义)
    - [12.2 常用头文件](#122-常用头文件)
    - [12.3 编译选项](#123-编译选项)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心概念](#核心概念)
    - [实践应用](#实践应用)
    - [学习建议](#学习建议)

---

## 1. POSIX概述和标准演进

### 1.1 什么是POSIX

POSIX（Portable Operating System Interface）是一系列IEEE标准，定义了操作系统应该为应用程序提供的接口。
POSIX.1-2024是最新的基础标准，定义了C语言应用程序接口。

### 1.2 POSIX标准演进历程

| 版本 | 年份 | 主要特性 |
|------|------|----------|
| POSIX.1 | 1988 | 初始标准，基本系统调用 |
| POSIX.1b | 1993 | 实时扩展 |
| POSIX.1c | 1995 | 线程支持（pthreads） |
| POSIX.1-2001 | 2001 | 合并多个标准 |
| POSIX.1-2008 | 2008 | 技术勘误和更新 |
| POSIX.1-2017 | 2017 | 主要更新版本 |
| POSIX.1-2024 | 2024 | 最新版本，包含现代扩展 |

### 1.3 POSIX合规性检查宏

```c
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
```

### 1.4 编译和运行

```bash
# Linux/gcc
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
gcc -o posix_version_check posix_version_check.c -D_POSIX_C_SOURCE=202405L

# 运行
./posix_version_check
```

---

## 2. 文件I/O操作

### 2.1 基础文件操作

POSIX提供了一套低层次的文件I/O接口，这些接口直接对应系统调用。

```c
/**
 * @file posix_file_io.c
 * @brief POSIX基础文件I/O操作示例
 * @description 包含open/read/write/close等核心系统调用的完整演示
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define TEST_FILE "posix_test_file.txt"

/**
 * @brief 错误处理宏
 */
#define HANDLE_ERROR(msg) do { \
    perror(msg); \
    exit(EXIT_FAILURE); \
} while(0)

/**
 * @brief 创建并写入文件
 * @param filename 文件名
 * @param content 要写入的内容
 * @return 成功返回0，失败返回-1
 */
int create_and_write_file(const char *filename, const char *content) {
    int fd;
    ssize_t bytes_written;
    size_t len = strlen(content);

    /**
     * open()系统调用
     * flags参数说明:
     * - O_WRONLY: 只写模式
     * - O_CREAT: 如果不存在则创建
     * - O_TRUNC: 如果存在则截断
     * - O_APPEND: 追加模式
     * mode参数: 文件权限(0644 = rw-r--r--)
     */
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        HANDLE_ERROR("open");
    }

    printf("文件描述符: %d\n", fd);

    /* write()系统调用 */
    bytes_written = write(fd, content, len);
    if (bytes_written == -1) {
        close(fd);
        HANDLE_ERROR("write");
    }

    printf("写入 %zd 字节\n", bytes_written);

    /* 使用fsync确保数据写入磁盘 */
    if (fsync(fd) == -1) {
        perror("fsync");
    }

    /* close()系统调用 */
    if (close(fd) == -1) {
        HANDLE_ERROR("close");
    }

    return 0;
}

/**
 * @brief 从文件读取内容
 * @param filename 文件名
 * @return 成功返回0，失败返回-1
 */
int read_file_content(const char *filename) {
    int fd;
    ssize_t bytes_read;
    char buffer[BUFFER_SIZE];

    /* O_RDONLY: 只读模式 */
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        HANDLE_ERROR("open");
    }

    printf("\n文件内容:\n");
    printf("----------\n");

    /* 循环读取直到文件末尾 */
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }

    if (bytes_read == -1) {
        close(fd);
        HANDLE_ERROR("read");
    }

    printf("\n----------\n");
    printf("读取完成\n");

    close(fd);
    return 0;
}

/**
 * @brief 使用lseek进行随机访问
 * @param filename 文件名
 */
void demonstrate_lseek(const char *filename) {
    int fd;
    off_t offset;
    char buffer[100];
    ssize_t n;

    fd = open(filename, O_RDWR);
    if (fd == -1) {
        HANDLE_ERROR("open");
    }

    printf("\n=== lseek演示 ===\n");

    /* 获取文件当前位置 */
    offset = lseek(fd, 0, SEEK_CUR);
    printf("当前偏移: %ld\n", (long)offset);

    /* 移动到文件末尾 */
    offset = lseek(fd, 0, SEEK_END);
    printf("文件大小: %ld 字节\n", (long)offset);

    /* 移动到文件开头 */
    lseek(fd, 0, SEEK_SET);
    printf("已移动到文件开头\n");

    /* 从第10个字节开始读取 */
    lseek(fd, 10, SEEK_SET);
    n = read(fd, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("从偏移10读取: %s\n", buffer);
    }

    /* 从当前位置向后移动5个字节 */
    offset = lseek(fd, 5, SEEK_CUR);
    printf("当前新偏移: %ld\n", (long)offset);

    close(fd);
}

/**
 * @brief 演示文件控制操作fcntl
 * @param filename 文件名
 */
void demonstrate_fcntl(const char *filename) {
    int fd;
    int flags;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        HANDLE_ERROR("open");
    }

    printf("\n=== fcntl演示 ===\n");

    /* 获取文件状态标志 */
    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        HANDLE_ERROR("fcntl F_GETFL");
    }

    printf("文件状态标志: 0x%x\n", flags);
    printf("访问模式: ");
    switch (flags & O_ACCMODE) {
        case O_RDONLY: printf("只读\n"); break;
        case O_WRONLY: printf("只写\n"); break;
        case O_RDWR: printf("读写\n"); break;
        default: printf("未知\n");
    }

    if (flags & O_APPEND) printf("O_APPEND 已设置\n");
    if (flags & O_NONBLOCK) printf("O_NONBLOCK 已设置\n");
    if (flags & O_SYNC) printf("O_SYNC 已设置\n");

    close(fd);
}

/**
 * @brief 原子性文件追加写入演示
 * @param filename 文件名
 * @param data 要追加的数据
 */
void atomic_append_demo(const char *filename, const char *data) {
    int fd;
    ssize_t n;
    size_t len = strlen(data);

    /**
     * O_APPEND标志确保写入操作是原子的
     * 每次写入都会自动定位到文件末尾
     */
    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        HANDLE_ERROR("open");
    }

    printf("\n=== 原子追加写入 ===\n");
    n = write(fd, data, len);
    if (n == -1) {
        close(fd);
        HANDLE_ERROR("write");
    }
    printf("追加写入 %zd 字节\n", n);

    close(fd);
}

/**
 * @brief 使用pread/pwrite进行位置独立的I/O
 * @param filename 文件名
 */
void demonstrate_pread_pwrite(const char *filename) {
    int fd;
    char write_buf[] = "PWRITE TEST DATA";
    char read_buf[100];
    ssize_t n;

    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        HANDLE_ERROR("open");
    }

    printf("\n=== pread/pwrite演示 ===\n");

    /* pwrite: 在指定偏移处写入，不改变文件偏移 */
    n = pwrite(fd, write_buf, strlen(write_buf), 100);
    if (n == -1) {
        close(fd);
        HANDLE_ERROR("pwrite");
    }
    printf("在偏移100处写入 %zd 字节\n", n);

    /* 验证文件偏移未改变 */
    off_t current = lseek(fd, 0, SEEK_CUR);
    printf("当前文件偏移: %ld\n", (long)current);

    /* pread: 在指定偏移处读取，不改变文件偏移 */
    n = pread(fd, read_buf, sizeof(read_buf) - 1, 100);
    if (n == -1) {
        close(fd);
        HANDLE_ERROR("pread");
    }
    read_buf[n] = '\0';
    printf("在偏移100处读取: %s\n", read_buf);

    close(fd);
}

int main(void) {
    const char *test_content =
        "This is a POSIX file I/O test file.\n"
        "Line 1: Introduction to POSIX.\n"
        "Line 2: File operations in C.\n"
        "Line 3: Using open, read, write, close.\n"
        "Line 4: Advanced operations like lseek.\n"
        "Line 5: End of test content.\n";

    printf("=== POSIX文件I/O操作演示 ===\n\n");

    /* 创建并写入文件 */
    if (create_and_write_file(TEST_FILE, test_content) == -1) {
        fprintf(stderr, "创建文件失败\n");
        return EXIT_FAILURE;
    }

    /* 读取文件内容 */
    if (read_file_content(TEST_FILE) == -1) {
        fprintf(stderr, "读取文件失败\n");
        return EXIT_FAILURE;
    }

    /* lseek演示 */
    demonstrate_lseek(TEST_FILE);

    /* fcntl演示 */
    demonstrate_fcntl(TEST_FILE);

    /* 原子追加 */
    atomic_append_demo(TEST_FILE, "\nAtomic appended line.\n");

    /* pread/pwrite演示 */
    demonstrate_pread_pwrite("posix_pread_test.dat");

    /* 清理 */
    unlink(TEST_FILE);
    unlink("posix_pread_test.dat");

    printf("\n=== 演示完成 ===\n");
    return 0;
}
```

### 2.2 高级文件I/O技术

```c
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
```

---

## 3. 文件系统操作

### 3.1 目录操作

```c
/**
 * @file posix_filesystem.c
 * @brief POSIX文件系统操作
 * @description 包含目录遍历、文件属性、符号链接等操作
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

/**
 * @brief 打印文件权限字符串
 * @param mode 文件模式
 */
void print_permissions(mode_t mode) {
    /* 文件类型 */
    if (S_ISREG(mode)) printf("-");
    else if (S_ISDIR(mode)) printf("d");
    else if (S_ISLNK(mode)) printf("l");
    else if (S_ISCHR(mode)) printf("c");
    else if (S_ISBLK(mode)) printf("b");
    else if (S_ISFIFO(mode)) printf("p");
    else if (S_ISSOCK(mode)) printf("s");
    else printf("?");

    /* 所有者权限 */
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ?
           ((mode & S_ISUID) ? "s" : "x") :
           ((mode & S_ISUID) ? "S" : "-"));

    /* 组权限 */
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ?
           ((mode & S_ISGID) ? "s" : "x") :
           ((mode & S_ISGID) ? "S" : "-"));

    /* 其他用户权限 */
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ?
           ((mode & S_ISVTX) ? "t" : "x") :
           ((mode & S_ISVTX) ? "T" : "-"));
}

/**
 * @brief 获取用户名
 * @param uid 用户ID
 * @return 用户名（静态缓冲区）
 */
const char *get_username(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

/**
 * @brief 获取组名
 * @param gid 组ID
 * @return 组名（静态缓冲区）
 */
const char *get_groupname(gid_t gid) {
    struct group *gr = getgrgid(gid);
    return gr ? gr->gr_name : "unknown";
}

/**
 * @brief 列出目录内容（类似ls -l）
 * @param path 目录路径
 */
void list_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char fullpath[PATH_MAX];

    printf("\n=== 目录列表: %s ===\n", path);
    printf("%-10s %3s %8s %8s %10s %20s %s\n",
           "权限", "链接", "所有者", "组", "大小", "修改时间", "名称");
    printf("--------------------------------------------------------------------------------\n");

    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        /* 构造完整路径 */
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        /* 获取文件信息 */
        if (lstat(fullpath, &st) == -1) {
            perror("lstat");
            continue;
        }

        /* 打印权限 */
        print_permissions(st.st_mode);
        printf(" %3ld", (long)st.st_nlink);
        printf(" %8s", get_username(st.st_uid));
        printf(" %8s", get_groupname(st.st_gid));
        printf(" %10ld", (long)st.st_size);

        /* 格式化时间 */
        char timebuf[20];
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S",
                 localtime(&st.st_mtime));
        printf(" %s", timebuf);

        /* 打印文件名 */
        printf(" %s", entry->d_name);

        /* 如果是符号链接，显示指向 */
        if (S_ISLNK(st.st_mode)) {
            char linktarget[PATH_MAX];
            ssize_t len = readlink(fullpath, linktarget, sizeof(linktarget) - 1);
            if (len != -1) {
                linktarget[len] = '\0';
                printf(" -> %s", linktarget);
            }
        }

        printf("\n");
    }

    closedir(dir);
}

/**
 * @brief 递归遍历目录树
 * @param path 起始路径
 * @param level 当前层级（用于缩进）
 */
void traverse_directory(const char *path, int level) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char fullpath[PATH_MAX];

    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        /* 跳过.和.. */
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        /* 打印缩进 */
        for (int i = 0; i < level; i++) {
            printf("  ");
        }

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (lstat(fullpath, &st) == -1) {
            printf("%s (无法访问)\n", entry->d_name);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            printf("[%s/]\n", entry->d_name);
            /* 递归遍历子目录 */
            traverse_directory(fullpath, level + 1);
        } else if (S_ISLNK(st.st_mode)) {
            char linktarget[PATH_MAX];
            ssize_t len = readlink(fullpath, linktarget, sizeof(linktarget) - 1);
            if (len != -1) {
                linktarget[len] = '\0';
                printf("%s -> %s\n", entry->d_name, linktarget);
            }
        } else {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

/**
 * @brief 创建目录结构
 * @param path 目录路径
 * @param mode 权限模式
 * @return 成功返回0，失败返回-1
 */
int create_directory_recursive(const char *path, mode_t mode) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    /* 移除末尾的斜杠 */
    if (tmp[len - 1] == '/')
        tmp[len - 1] = '\0';

    /* 逐级创建目录 */
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }

    /* 创建最后一级目录 */
    if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
        return -1;
    }

    return 0;
}

/**
 * @brief 改变文件权限
 * @param path 文件路径
 * @param mode 新权限
 */
void change_permissions(const char *path, mode_t mode) {
    printf("\n修改 %s 的权限为 %04o\n", path, mode);

    if (chmod(path, mode) == -1) {
        perror("chmod");
        return;
    }

    /* 验证 */
    struct stat st;
    if (stat(path, &st) == 0) {
        printf("新权限: ");
        print_permissions(st.st_mode);
        printf(" (%04o)\n", st.st_mode & 07777);
    }
}

/**
 * @brief 符号链接操作演示
 */
void symlink_demo(void) {
    const char *target = "symlink_target.txt";
    const char *linkname = "symlink_link";
    char buf[PATH_MAX];
    ssize_t len;

    printf("\n=== 符号链接操作 ===\n");

    /* 创建目标文件 */
    FILE *fp = fopen(target, "w");
    if (fp) {
        fprintf(fp, "This is the target file.\n");
        fclose(fp);
    }

    /* 创建符号链接 */
    if (symlink(target, linkname) == -1) {
        if (errno == EEXIST) {
            printf("符号链接已存在\n");
        } else {
            perror("symlink");
        }
    } else {
        printf("创建符号链接: %s -> %s\n", linkname, target);
    }

    /* 读取符号链接内容（不是目标文件） */
    len = readlink(linkname, buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        printf("符号链接指向: %s\n", buf);
    }

    /* 清理 */
    unlink(linkname);
    unlink(target);
}

/**
 * @brief 获取和修改文件时间
 */
void file_times_demo(void) {
    const char *filename = "time_test.txt";
    struct stat st;
    struct timespec times[2];

    printf("\n=== 文件时间操作 ===\n");

    /* 创建文件 */
    FILE *fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "Test\n");
        fclose(fp);
    }

    /* 获取当前时间 */
    if (stat(filename, &st) == 0) {
        printf("访问时间: %s", ctime(&st.st_atime));
        printf("修改时间: %s", ctime(&st.st_mtime));
    }

    /* 修改访问和修改时间 */
    times[0].tv_sec = 1609459200;  /* 2021-01-01 00:00:00 */
    times[0].tv_nsec = 0;
    times[1].tv_sec = 1609459200;
    times[1].tv_nsec = 0;

    if (utimensat(AT_FDCWD, filename, times, 0) == -1) {
        perror("utimensat");
    } else {
        printf("已修改文件时间\n");
        if (stat(filename, &st) == 0) {
            printf("新访问时间: %s", ctime(&st.st_atime));
            printf("新修改时间: %s", ctime(&st.st_mtime));
        }
    }

    unlink(filename);
}

int main(void) {
    printf("=== POSIX文件系统操作演示 ===\n");

    /* 列出当前目录 */
    list_directory(".");

    /* 目录树遍历 */
    printf("\n=== 目录树遍历 ===\n");
    traverse_directory(".", 0);

    /* 创建目录 */
    const char *test_dir = "posix_test_dir/subdir1/subdir2";
    if (create_directory_recursive(test_dir, 0755) == 0) {
        printf("\n创建目录: %s\n", test_dir);
    }

    /* 权限修改演示 */
    const char *perm_file = "posix_test_dir/perm_test.txt";
    FILE *fp = fopen(perm_file, "w");
    if (fp) {
        fprintf(fp, "test\n");
        fclose(fp);
    }
    change_permissions(perm_file, 0640);

    /* 符号链接演示 */
    symlink_demo();

    /* 文件时间演示 */
    file_times_demo();

    /* 清理 */
    remove(perm_file);
    rmdir("posix_test_dir/subdir1/subdir2");
    rmdir("posix_test_dir/subdir1");
    rmdir("posix_test_dir");

    printf("\n=== 文件系统演示完成 ===\n");
    return 0;
}
```



---

## 4. 进程控制

### 4.1 进程创建与管理

```c
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
```

---

## 5. 进程间通信

### 5.1 管道通信

```c
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
```

### 5.2 System V IPC

```c
/**
 * @file posix_sysv_ipc.c
 * @brief System V IPC机制
 * @description 包含消息队列、共享内存、信号量
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define MSG_KEY 1234
#define SHM_KEY 5678
#define SEM_KEY 9012
#define MSG_SIZE 256

/**
 * @brief 消息结构
 */
struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

/**
 * @brief 消息队列演示
 */
void message_queue_demo(void) {
    int msgid;
    pid_t pid;
    struct msg_buffer message;

    printf("\n=== System V 消息队列 ===\n");

    /* 创建消息队列 */
    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程：接收消息 */
        sleep(1);  /* 确保父进程先发送 */

        /* 接收类型为1的消息 */
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("子进程收到消息: %s\n", message.msg_text);

        /* 发送回复 */
        message.msg_type = 2;  /* 不同类型 */
        strcpy(message.msg_text, "Reply from child process");
        msgsnd(msgid, &message, strlen(message.msg_text) + 1, 0);

        exit(0);
    } else {
        /* 父进程：发送消息 */
        message.msg_type = 1;
        strcpy(message.msg_text, "Hello from parent process");

        if (msgsnd(msgid, &message, strlen(message.msg_text) + 1, 0) == -1) {
            perror("msgsnd");
        } else {
            printf("父进程发送消息: %s\n", message.msg_text);
        }

        /* 等待子进程回复 */
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 2, 0) != -1) {
            printf("父进程收到回复: %s\n", message.msg_text);
        }

        wait(NULL);

        /* 删除消息队列 */
        msgctl(msgid, IPC_RMID, NULL);
    }
}

/**
 * @brief 共享内存演示
 */
void shared_memory_demo(void) {
    int shmid;
    pid_t pid;
    char *shm_addr;
    const char *msg = "Hello from shared memory!";

    printf("\n=== System V 共享内存 ===\n");

    /* 创建共享内存段 */
    shmid = shmget(SHM_KEY, 1024, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        sleep(1);

        /* 附加共享内存 */
        shm_addr = (char *)shmat(shmid, NULL, 0);
        if (shm_addr == (char *)-1) {
            perror("shmat");
            exit(1);
        }

        printf("子进程从共享内存读取: %s\n", shm_addr);

        /* 写入回复 */
        strcpy(shm_addr, "Child was here!");

        /* 分离共享内存 */
        shmdt(shm_addr);
        exit(0);
    } else {
        /* 父进程 */
        shm_addr = (char *)shmat(shmid, NULL, 0);
        if (shm_addr == (char *)-1) {
            perror("shmat");
            return;
        }

        /* 写入数据 */
        strcpy(shm_addr, msg);
        printf("父进程写入共享内存: %s\n", msg);

        wait(NULL);

        printf("父进程读取共享内存: %s\n", shm_addr);

        /* 分离并删除共享内存 */
        shmdt(shm_addr);
        shmctl(shmid, IPC_RMID, NULL);
    }
}

/**
 * @brief 信号量操作（PV操作）
 */
void semaphore_demo(void) {
    int semid;
    pid_t pid;
    struct sembuf sb;
    int shared_value = 0;  /* 注意：实际应该在共享内存中 */

    printf("\n=== System V 信号量 ===\n");

    /* 创建信号量集，包含1个信号量 */
    semid = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        return;
    }

    /* 初始化信号量值为1（二进制信号量/互斥锁） */
    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl SETVAL");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        sleep(1);

        /* P操作：等待信号量 */
        sb.sem_num = 0;
        sb.sem_op = -1;  /* 减1 */
        sb.sem_flg = 0;

        printf("子进程: 等待信号量...\n");
        if (semop(semid, &sb, 1) == -1) {
            perror("semop P");
            exit(1);
        }

        printf("子进程: 获得信号量，访问临界区\n");
        sleep(2);  /* 模拟临界区操作 */
        printf("子进程: 离开临界区\n");

        /* V操作：释放信号量 */
        sb.sem_op = 1;  /* 加1 */
        semop(semid, &sb, 1);

        exit(0);
    } else {
        /* 父进程 */
        /* P操作 */
        sb.sem_num = 0;
        sb.sem_op = -1;
        sb.sem_flg = 0;

        printf("父进程: 等待信号量...\n");
        semop(semid, &sb, 1);

        printf("父进程: 获得信号量，访问临界区\n");
        sleep(2);
        printf("父进程: 离开临界区\n");

        /* V操作 */
        sb.sem_op = 1;
        semop(semid, &sb, 1);

        wait(NULL);

        /* 删除信号量集 */
        semctl(semid, 0, IPC_RMID);
    }

    printf("信号量同步完成\n");
}

int main(void) {
    printf("=== System V IPC演示 ===\n");

    message_queue_demo();
    shared_memory_demo();
    semaphore_demo();

    printf("\n=== System V IPC演示完成 ===\n");
    return 0;
}
```

### 5.3 POSIX共享内存和信号量

```c
/**
 * @file posix_ipc.c
 * @brief POSIX IPC机制
 * @description 使用POSIX命名的共享内存和信号量
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>

#define SHM_NAME "/posix_shm_demo"
#define SEM_NAME "/posix_sem_demo"
#define SHM_SIZE 1024

/**
 * @brief POSIX共享内存演示
 */
void posix_shm_demo(void) {
    int shm_fd;
    pid_t pid;
    char *shm_ptr;
    const char *msg = "Hello from POSIX shared memory!";

    printf("\n=== POSIX 共享内存 ===\n");

    /* 删除可能存在的旧共享内存 */
    shm_unlink(SHM_NAME);

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        sleep(1);

        /* 打开共享内存对象 */
        shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
        if (shm_fd == -1) {
            perror("shm_open (child)");
            exit(1);
        }

        /* 映射到地址空间 */
        shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        if (shm_ptr == MAP_FAILED) {
            perror("mmap");
            close(shm_fd);
            exit(1);
        }

        printf("子进程读取: %s\n", shm_ptr);

        munmap(shm_ptr, SHM_SIZE);
        close(shm_fd);
        exit(0);
    } else {
        /* 父进程 */
        /* 创建共享内存对象 */
        shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) {
            perror("shm_open (parent)");
            return;
        }

        /* 设置共享内存大小 */
        if (ftruncate(shm_fd, SHM_SIZE) == -1) {
            perror("ftruncate");
            close(shm_fd);
            return;
        }

        /* 映射到地址空间 */
        shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shm_ptr == MAP_FAILED) {
            perror("mmap");
            close(shm_fd);
            return;
        }

        /* 写入数据 */
        strcpy(shm_ptr, msg);
        printf("父进程写入: %s\n", msg);

        wait(NULL);

        munmap(shm_ptr, SHM_SIZE);
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }
}

/**
 * @brief POSIX命名信号量演示
 */
void posix_named_semaphore_demo(void) {
    sem_t *sem;
    pid_t pid;

    printf("\n=== POSIX 命名信号量 ===\n");

    /* 删除可能存在的旧信号量 */
    sem_unlink(SEM_NAME);

    /* 创建信号量，初始值为1 */
    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        sleep(1);

        printf("子进程: 等待信号量...\n");
        sem_wait(sem);  /* P操作 */

        printf("子进程: 进入临界区\n");
        sleep(2);
        printf("子进程: 离开临界区\n");

        sem_post(sem);  /* V操作 */

        sem_close(sem);
        exit(0);
    } else {
        /* 父进程 */
        printf("父进程: 等待信号量...\n");
        sem_wait(sem);

        printf("父进程: 进入临界区\n");
        sleep(2);
        printf("父进程: 离开临界区\n");

        sem_post(sem);

        wait(NULL);

        sem_close(sem);
        sem_unlink(SEM_NAME);
    }
}

/**
 * @brief POSIX无名信号量演示（用于线程）
 */
void posix_unnamed_semaphore_demo(void) {
    sem_t sem;

    printf("\n=== POSIX 无名信号量（进程内）===\n");

    /* 初始化无名信号量，进程内共享 */
    if (sem_init(&sem, 0, 1) == -1) {
        perror("sem_init");
        return;
    }

    printf("获取信号量...\n");
    sem_wait(&sem);

    printf("在临界区内操作...\n");

    sem_post(&sem);
    printf("释放信号量\n");

    /* 销毁信号量 */
    sem_destroy(&sem);
}

int main(void) {
    printf("=== POSIX IPC演示 ===\n");

    posix_shm_demo();
    posix_named_semaphore_demo();
    posix_unnamed_semaphore_demo();

    printf("\n=== POSIX IPC演示完成 ===\n");
    return 0;
}
```



---

## 6. 信号处理

### 6.1 基础信号处理

```c
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
```

### 6.2 实时信号

```c
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
```

---

## 7. 线程管理

### 7.1 POSIX线程基础

```c
/**
 * @file posix_threads.c
 * @brief POSIX线程(pthread)编程
 * @description 线程创建、终止、连接、分离等操作
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

/**
 * @brief 简单线程函数
 * @param arg 线程参数
 * @return 返回值
 */
void *simple_thread(void *arg) {
    int num = *(int *)arg;
    printf("线程 %d: 启动, TID=%lu\n", num, (unsigned long)pthread_self());

    /* 模拟工作 */
    for (int i = 0; i < 3; i++) {
        printf("线程 %d: 工作中... %d\n", num, i + 1);
        usleep(500000);  /* 500ms */
    }

    printf("线程 %d: 结束\n", num);

    /* 返回状态 */
    int *result = malloc(sizeof(int));
    *result = num * 10;
    pthread_exit(result);
}

/**
 * @brief 基本线程创建演示
 */
void basic_thread_demo(void) {
    pthread_t threads[3];
    int thread_nums[3] = {1, 2, 3};
    void *status;

    printf("\n=== 基本线程创建 ===\n");
    printf("主线程: TID=%lu\n", (unsigned long)pthread_self());

    /* 创建多个线程 */
    for (int i = 0; i < 3; i++) {
        int ret = pthread_create(&threads[i], NULL, simple_thread, &thread_nums[i]);
        if (ret != 0) {
            fprintf(stderr, "创建线程失败: %s\n", strerror(ret));
            exit(1);
        }
        printf("主线程: 已创建线程 %d, TID=%lu\n",
               i + 1, (unsigned long)threads[i]);
    }

    /* 等待所有线程完成 */
    for (int i = 0; i < 3; i++) {
        int ret = pthread_join(threads[i], &status);
        if (ret != 0) {
            fprintf(stderr, "等待线程失败: %s\n", strerror(ret));
        } else {
            if (status != NULL) {
                printf("主线程: 线程 %d 返回 %d\n", i + 1, *(int *)status);
                free(status);
            }
        }
    }

    printf("主线程: 所有线程已结束\n");
}

/**
 * @brief 线程分离演示
 */
void detached_thread_demo(void) {
    pthread_t thread;
    pthread_attr_t attr;

    printf("\n=== 分离线程 ===\n");

    /* 初始化线程属性 */
    pthread_attr_init(&attr);
    /* 设置分离状态 */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /* 创建分离线程 */
    int ret = pthread_create(&thread, &attr, simple_thread, &(int){4});
    if (ret != 0) {
        fprintf(stderr, "创建线程失败: %s\n", strerror(ret));
    } else {
        printf("分离线程已创建，不能join\n");
    }

    /* 销毁属性 */
    pthread_attr_destroy(&attr);

    /* 等待分离线程完成 */
    sleep(3);
    printf("分离线程演示结束\n");
}

/**
 * @brief 线程取消演示
 */
void *cancelable_thread(void *arg) {
    int oldstate;

    printf("线程: 启动\n");

    /* 设置取消状态为启用 */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
    /* 设置取消类型为延迟取消 */
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    /* 设置取消点 */
    pthread_testcancel();

    printf("线程: 进入长时间操作...\n");

    /* 模拟长时间操作，包含取消点 */
    for (int i = 0; i < 10; i++) {
        sleep(1);  /* sleep是一个取消点 */
        printf("线程: 工作中 %d/10\n", i + 1);
    }

    printf("线程: 正常结束\n");
    return NULL;
}

void thread_cancel_demo(void) {
    pthread_t thread;
    void *status;

    printf("\n=== 线程取消 ===\n");

    pthread_create(&thread, NULL, cancelable_thread, NULL);

    /* 2秒后取消线程 */
    sleep(2);
    printf("主线程: 请求取消工作线程\n");
    pthread_cancel(thread);

    int ret = pthread_join(thread, &status);
    if (ret == 0) {
        if (status == PTHREAD_CANCELED) {
            printf("主线程: 工作线程已被取消\n");
        } else {
            printf("主线程: 工作线程正常返回\n");
        }
    }
}

/**
 * @brief 线程特定数据（TLS）
 */
static pthread_key_t thread_key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

void make_key(void) {
    pthread_key_create(&thread_key, free);
}

void *tls_thread(void *arg) {
    int num = *(int *)arg;

    /* 确保key只创建一次 */
    pthread_once(&key_once, make_key);

    /* 分配线程特定数据 */
    int *data = malloc(sizeof(int));
    *data = num * 100;
    pthread_setspecific(thread_key, data);

    printf("线程 %d: 设置TLS值 = %d\n", num, *data);

    /* 读取TLS */
    int *tls_value = pthread_getspecific(thread_key);
    printf("线程 %d: 读取TLS值 = %d\n", num, *tls_value);

    return NULL;
}

void thread_specific_data_demo(void) {
    pthread_t threads[3];
    int nums[3] = {1, 2, 3};

    printf("\n=== 线程特定数据(TLS) ===\n");

    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, tls_thread, &nums[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_key_delete(thread_key);
}

/**
 * @brief 线程栈大小设置
 */
void thread_stack_demo(void) {
    pthread_t thread;
    pthread_attr_t attr;
    size_t stacksize;

    printf("\n=== 线程栈大小 ===\n");

    pthread_attr_init(&attr);

    /* 获取默认栈大小 */
    pthread_attr_getstacksize(&attr, &stacksize);
    printf("默认栈大小: %zu KB\n", stacksize / 1024);

    /* 设置新栈大小（最小1MB） */
    size_t new_stacksize = 2 * 1024 * 1024;  /* 2MB */
    pthread_attr_setstacksize(&attr, new_stacksize);

    pthread_attr_getstacksize(&attr, &stacksize);
    printf("新栈大小: %zu KB\n", stacksize / 1024);

    pthread_attr_destroy(&attr);
}

int main(void) {
    printf("=== POSIX线程管理演示 ===\n");

    basic_thread_demo();
    detached_thread_demo();
    thread_cancel_demo();
    thread_specific_data_demo();
    thread_stack_demo();

    printf("\n=== 线程管理演示完成 ===\n");
    return 0;
}
```

---

## 8. 同步原语

### 8.1 互斥锁

```c
/**
 * @file posix_mutex.c
 * @brief POSIX互斥锁
 * @description 互斥锁的创建、锁定、解锁、属性设置
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static int shared_counter = 0;
static pthread_mutex_t mutex;

/**
 * @brief 基本互斥锁演示
 */
void *mutex_thread(void *arg) {
    int num = *(int *)arg;

    for (int i = 0; i < 1000; i++) {
        /* 锁定互斥锁 */
        pthread_mutex_lock(&mutex);

        /* 临界区：访问共享资源 */
        int temp = shared_counter;
        temp++;
        usleep(1);  /* 模拟一些工作 */
        shared_counter = temp;

        /* 解锁 */
        pthread_mutex_unlock(&mutex);
    }

    printf("线程 %d 完成\n", num);
    return NULL;
}

void basic_mutex_demo(void) {
    pthread_t threads[4];
    int nums[4] = {1, 2, 3, 4};

    printf("\n=== 基本互斥锁 ===\n");

    /* 初始化互斥锁（默认属性） */
    pthread_mutex_init(&mutex, NULL);

    shared_counter = 0;

    /* 创建多个线程 */
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, mutex_thread, &nums[i]);
    }

    /* 等待所有线程 */
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("最终计数器值: %d (期望: 4000)\n", shared_counter);

    pthread_mutex_destroy(&mutex);
}

/**
 * @brief 递归互斥锁演示
 */
void recursive_mutex_demo(void) {
    pthread_mutex_t rec_mutex;
    pthread_mutexattr_t attr;

    printf("\n=== 递归互斥锁 ===\n");

    /* 初始化属性 */
    pthread_mutexattr_init(&attr);
    /* 设置为递归类型 */
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    /* 用属性初始化互斥锁 */
    pthread_mutex_init(&rec_mutex, &attr);

    /* 同一线程多次锁定 */
    printf("第一次锁定...\n");
    pthread_mutex_lock(&rec_mutex);

    printf("第二次锁定（递归）...\n");
    pthread_mutex_lock(&rec_mutex);

    printf("第三次锁定（递归）...\n");
    pthread_mutex_lock(&rec_mutex);

    /* 需要相同次数的解锁 */
    pthread_mutex_unlock(&rec_mutex);
    pthread_mutex_unlock(&rec_mutex);
    pthread_mutex_unlock(&rec_mutex);

    printf("递归互斥锁解锁完成\n");

    pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&rec_mutex);
}

/**
 * @brief 带超时的互斥锁
 */
void timed_mutex_demo(void) {
    pthread_mutex_t timed_mutex;
    struct timespec timeout;

    printf("\n=== 超时互斥锁 ===\n");

    pthread_mutex_init(&timed_mutex, NULL);
    pthread_mutex_lock(&timed_mutex);  /* 先锁定 */

    /* 计算超时时间 */
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 2;  /* 2秒超时 */

    printf("尝试在2秒内获取已锁定的互斥锁...\n");
    int ret = pthread_mutex_timedlock(&timed_mutex, &timeout);

    if (ret == ETIMEDOUT) {
        printf("超时，未能获取互斥锁\n");
    }

    pthread_mutex_unlock(&timed_mutex);
    pthread_mutex_destroy(&timed_mutex);
}

/**
 * @brief 优先级继承协议
 */
void priority_inheritance_demo(void) {
    pthread_mutex_t pi_mutex;
    pthread_mutexattr_t attr;
    int protocol;

    printf("\n=== 优先级继承协议 ===\n");

    pthread_mutexattr_init(&attr);

    /* 设置优先级继承协议 */
    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

    /* 获取并验证 */
    pthread_mutexattr_getprotocol(&attr, &protocol);
    printf("互斥锁协议: ");
    switch (protocol) {
        case PTHREAD_PRIO_NONE:     printf("无\n"); break;
        case PTHREAD_PRIO_INHERIT:  printf("优先级继承\n"); break;
        case PTHREAD_PRIO_PROTECT:  printf("优先级保护\n"); break;
        default: printf("未知\n");
    }

    pthread_mutex_init(&pi_mutex, &attr);

    pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&pi_mutex);
}

int main(void) {
    printf("=== POSIX互斥锁演示 ===\n");

    basic_mutex_demo();
    recursive_mutex_demo();
    timed_mutex_demo();
    priority_inheritance_demo();

    printf("\n=== 互斥锁演示完成 ===\n");
    return 0;
}
```

### 8.2 条件变量和读写锁

```c
/**
 * @file posix_condition_rwlock.c
 * @brief POSIX条件变量和读写锁
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int ready = 0;

/**
 * @brief 条件变量基本使用
 */
void *condition_wait_thread(void *arg) {
    printf("等待线程: 等待条件...\n");

    pthread_mutex_lock(&mutex);

    /* 等待条件，自动释放互斥锁 */
    while (!ready) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("等待线程: 条件满足，继续执行\n");

    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *condition_signal_thread(void *arg) {
    sleep(2);

    printf("信号线程: 准备发送信号...\n");

    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_mutex_unlock(&mutex);

    /* 发送信号唤醒等待线程 */
    pthread_cond_signal(&cond);
    printf("信号线程: 信号已发送\n");

    return NULL;
}

void condition_variable_demo(void) {
    pthread_t wait_thread, signal_thread;

    printf("\n=== 条件变量 ===\n");

    ready = 0;

    pthread_create(&wait_thread, NULL, condition_wait_thread, NULL);
    pthread_create(&signal_thread, NULL, condition_signal_thread, NULL);

    pthread_join(wait_thread, NULL);
    pthread_join(signal_thread, NULL);
}

/**
 * @brief 生产者消费者模型
 */
#define BUFFER_SIZE 10

static int buffer[BUFFER_SIZE];
static int count = 0;
static int produce_index = 0;
static int consume_index = 0;

static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&buffer_mutex);

        /* 等待缓冲区不满 */
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&not_full, &buffer_mutex);
        }

        /* 生产数据 */
        buffer[produce_index] = id * 100 + i;
        printf("生产者 %d: 生产 %d\n", id, buffer[produce_index]);
        produce_index = (produce_index + 1) % BUFFER_SIZE;
        count++;

        /* 通知消费者 */
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&buffer_mutex);

        usleep(100000);  /* 模拟生产时间 */
    }

    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&buffer_mutex);

        /* 等待缓冲区不空 */
        while (count == 0) {
            pthread_cond_wait(&not_empty, &buffer_mutex);
        }

        /* 消费数据 */
        int data = buffer[consume_index];
        printf("  消费者 %d: 消费 %d\n", id, data);
        consume_index = (consume_index + 1) % BUFFER_SIZE;
        count--;

        /* 通知生产者 */
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&buffer_mutex);

        usleep(200000);  /* 模拟消费时间 */
    }

    return NULL;
}

void producer_consumer_demo(void) {
    pthread_t producers[2], consumers[2];
    int p_ids[2] = {1, 2};
    int c_ids[2] = {1, 2};

    printf("\n=== 生产者消费者模型 ===\n");

    /* 创建生产者 */
    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer, &p_ids[i]);
    }

    /* 创建消费者 */
    for (int i = 0; i < 2; i++) {
        pthread_create(&consumers[i], NULL, consumer, &c_ids[i]);
    }

    /* 等待所有线程 */
    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(consumers[i], NULL);
    }
}

/**
 * @brief 读写锁演示
 */
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
static int shared_data = 0;

void *reader_thread(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&rwlock);  /* 获取读锁 */
        printf("读者 %d: 读取数据 = %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        usleep(100000);
    }

    return NULL;
}

void *writer_thread(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 3; i++) {
        pthread_rwlock_wrlock(&rwlock);  /* 获取写锁 */
        shared_data++;
        printf("写者 %d: 写入数据 = %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        usleep(300000);
    }

    return NULL;
}

void read_write_lock_demo(void) {
    pthread_t readers[3], writers[2];
    int r_ids[3] = {1, 2, 3};
    int w_ids[2] = {1, 2};

    printf("\n=== 读写锁 ===\n");

    shared_data = 0;

    /* 创建读者线程 */
    for (int i = 0; i < 3; i++) {
        pthread_create(&readers[i], NULL, reader_thread, &r_ids[i]);
    }

    /* 创建写者线程 */
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer_thread, &w_ids[i]);
    }

    /* 等待所有线程 */
    for (int i = 0; i < 3; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    printf("最终数据值: %d\n", shared_data);
}

/**
 * @brief 屏障同步
 */
static pthread_barrier_t barrier;

void *barrier_thread(void *arg) {
    int id = *(int *)arg;

    printf("线程 %d: 到达屏障\n", id);

    /* 等待所有线程到达屏障 */
    int result = pthread_barrier_wait(&barrier);

    if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("线程 %d: 是序列化线程（最后一个到达）\n", id);
    } else {
        printf("线程 %d: 通过屏障\n", id);
    }

    return NULL;
}

void barrier_demo(void) {
    pthread_t threads[4];
    int ids[4] = {1, 2, 3, 4};

    printf("\n=== 屏障同步 ===\n");

    /* 初始化屏障，计数为4 */
    pthread_barrier_init(&barrier, NULL, 4);

    /* 创建线程 */
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, barrier_thread, &ids[i]);
    }

    /* 等待所有线程 */
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    printf("所有线程已通过屏障\n");
}

int main(void) {
    printf("=== POSIX条件变量和读写锁演示 ===\n");

    condition_variable_demo();
    producer_consumer_demo();
    read_write_lock_demo();
    barrier_demo();

    printf("\n=== 同步原语演示完成 ===\n");
    return 0;
}
```



---

## 9. 网络Socket编程

### 9.1 TCP Socket编程

```c
/**
 * @file posix_socket_tcp.c
 * @brief POSIX TCP Socket编程
 * @description TCP客户端/服务器实现
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define BACKLOG 10

/**
 * @brief 创建TCP服务器
 */
void tcp_server_demo(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    int opt = 1;

    printf("\n=== TCP服务器 ===\n");

    /* 创建socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return;
    }

    /* 设置socket选项 */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_fd);
        return;
    }

    /* 绑定地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return;
    }

    /* 监听连接 */
    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        close(server_fd);
        return;
    }

    printf("TCP服务器监听端口 %d...\n", PORT);
    printf("服务器演示：等待5秒客户端连接...\n");

    /* 设置接收超时 */
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("超时：没有客户端连接\n");
        } else {
            perror("accept");
        }
        close(server_fd);
        return;
    }

    printf("客户端已连接: %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    /* 接收数据 */
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("收到: %s\n", buffer);

        /* 发送响应 */
        const char *response = "Hello from TCP server!";
        send(client_fd, response, strlen(response), 0);
    }

    close(client_fd);
    close(server_fd);
}

/**
 * @brief 创建TCP客户端
 */
void tcp_client_demo(void) {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    printf("\n=== TCP客户端 ===\n");

    /* 创建socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    /* 设置服务器地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* 连接服务器 */
    if (connect(sock_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("connect");
        close(sock_fd);
        return;
    }

    printf("已连接到服务器\n");

    /* 发送数据 */
    const char *message = "Hello from TCP client!";
    send(sock_fd, message, strlen(message), 0);
    printf("发送: %s\n", message);

    /* 接收响应 */
    int bytes_received = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("收到响应: %s\n", buffer);
    }

    close(sock_fd);
}

/**
 * @brief 多进程TCP服务器
 */
void tcp_fork_server_demo(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    pid_t pid;

    printf("\n=== 多进程TCP服务器 ===\n");

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT + 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return;
    }

    listen(server_fd, BACKLOG);
    printf("多进程服务器监听端口 %d...\n", PORT + 1);
    printf("演示模式：等待3秒...\n");

    /* 设置超时 */
    struct timeval timeout = {3, 0};
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd == -1) {
        printf("没有连接，继续演示...\n");
        close(server_fd);
        return;
    }

    /* 创建子进程处理连接 */
    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        close(server_fd);

        char buffer[BUFFER_SIZE];
        int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("子进程处理: %s\n", buffer);
            send(client_fd, "Processed by child", 18, 0);
        }

        close(client_fd);
        exit(0);
    } else {
        /* 父进程 */
        close(client_fd);
        wait(NULL);
    }

    close(server_fd);
}

/**
 * @brief 使用select的I/O多路复用服务器
 */
void select_server_demo(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    fd_set read_fds, master_fds;
    int max_fd;

    printf("\n=== Select I/O多路复用 ===\n");

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT + 2);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, BACKLOG);

    printf("Select服务器监听端口 %d...\n", PORT + 2);
    printf("演示模式，立即退出...\n");

    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);
    max_fd = server_fd;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    read_fds = master_fds;
    int ret = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

    if (ret == 0) {
        printf("Select超时（正常）\n");
    }

    close(server_fd);
}

int main(void) {
    printf("=== POSIX TCP Socket编程演示 ===\n");

    tcp_server_demo();
    /* tcp_client_demo(); */  /* 需要在服务器运行时执行 */
    tcp_fork_server_demo();
    select_server_demo();

    printf("\n=== TCP Socket演示完成 ===\n");
    return 0;
}
```

### 9.2 UDP Socket编程

```c
/**
 * @file posix_socket_udp.c
 * @brief POSIX UDP Socket编程
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define UDP_PORT 9999
#define BUFFER_SIZE 1024

/**
 * @brief UDP服务器
 */
void udp_server_demo(void) {
    int sock_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];

    printf("\n=== UDP服务器 ===\n");

    /* 创建UDP socket */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    /* 绑定地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_PORT);

    if (bind(sock_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(sock_fd);
        return;
    }

    printf("UDP服务器监听端口 %d...\n", UDP_PORT);
    printf("等待数据（2秒超时）...\n");

    /* 设置接收超时 */
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    addr_len = sizeof(client_addr);
    int bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);

    if (bytes == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("超时：没有收到数据\n");
        } else {
            perror("recvfrom");
        }
    } else {
        buffer[bytes] = '\0';
        printf("从 %s:%d 收到: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);

        /* 发送响应 */
        const char *response = "UDP response from server";
        sendto(sock_fd, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, addr_len);
    }

    close(sock_fd);
}

/**
 * @brief UDP客户端
 */
void udp_client_demo(void) {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;

    printf("\n=== UDP客户端 ===\n");

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    /* 设置服务器地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* 发送数据 */
    const char *message = "Hello UDP server!";
    sendto(sock_fd, message, strlen(message), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("发送: %s\n", message);

    /* 接收响应 */
    addr_len = sizeof(server_addr);
    struct timeval timeout = {2, 0};
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    int bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&server_addr, &addr_len);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("收到响应: %s\n", buffer);
    } else {
        printf("未收到响应（服务器可能未运行）\n");
    }

    close(sock_fd);
}

/**
 * @brief UDP广播
 */
void udp_broadcast_demo(void) {
    int sock_fd;
    int broadcast_enable = 1;
    struct sockaddr_in broadcast_addr;

    printf("\n=== UDP广播 ===\n");

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    /* 启用广播 */
    if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST,
                   &broadcast_enable, sizeof(broadcast_enable)) == -1) {
        perror("setsockopt");
        close(sock_fd);
        return;
    }

    /* 设置广播地址 */
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(UDP_PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    const char *message = "Broadcast message!";
    int ret = sendto(sock_fd, message, strlen(message), 0,
                     (struct sockaddr *)&broadcast_addr,
                     sizeof(broadcast_addr));

    if (ret == -1) {
        perror("sendto");
    } else {
        printf("广播消息已发送: %s\n", message);
    }

    close(sock_fd);
}

/**
 * @brief Unix域套接字（本地套接字）
 */
#include <sys/un.h>

#define SOCKET_PATH "/tmp/posix_unix_socket"

void unix_domain_socket_demo(void) {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    printf("\n=== Unix域套接字 ===\n");

    /* 删除旧socket文件 */
    unlink(SOCKET_PATH);

    /* 创建Unix域socket */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return;
    }

    /* 绑定到文件系统路径 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return;
    }

    listen(server_fd, 5);
    printf("Unix域服务器监听 %s\n", SOCKET_PATH);
    printf("（演示模式，不等待连接）\n");

    close(server_fd);
    unlink(SOCKET_PATH);
}

int main(void) {
    printf("=== POSIX UDP Socket编程演示 ===\n");

    udp_server_demo();
    /* udp_client_demo(); */
    udp_broadcast_demo();
    unix_domain_socket_demo();

    printf("\n=== UDP Socket演示完成 ===\n");
    return 0;
}
```

---

## 10. 实时扩展

### 10.1 POSIX实时特性

```c
/**
 * @file posix_realtime.c
 * @brief POSIX实时扩展
 * @description 实时信号、内存锁定、异步I/O等
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sched.h>
#include <errno.h>

/**
 * @brief 高精度时钟演示
 */
void high_precision_clock_demo(void) {
    struct timespec ts;

    printf("\n=== 高精度时钟 ===\n");

    /* 获取实时时钟 */
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime CLOCK_REALTIME");
    } else {
        printf("实时时钟: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);
        printf("转换为本地时间: %s", ctime(&ts.tv_sec));
    }

    /* 获取单调时钟（不受系统时间调整影响） */
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        perror("clock_gettime CLOCK_MONOTONIC");
    } else {
        printf("单调时钟: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);
    }

    /* 获取CPU时间 */
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == -1) {
        perror("clock_gettime CLOCK_PROCESS_CPUTIME_ID");
    } else {
        printf("进程CPU时间: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);
    }

    /* 获取时钟分辨率 */
    if (clock_getres(CLOCK_REALTIME, &ts) == 0) {
        printf("实时时钟分辨率: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);
    }
}

/**
 * @brief 高精度睡眠
 */
void high_precision_sleep_demo(void) {
    struct timespec req, rem;

    printf("\n=== 高精度睡眠 ===\n");

    /* 睡眠100毫秒 + 500纳秒 */
    req.tv_sec = 0;
    req.tv_nsec = 100500000;  /* 100.5毫秒 */

    printf("睡眠 %ld.%09ld 秒...\n", (long)req.tv_sec, req.tv_nsec);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (nanosleep(&req, &rem) == -1) {
        if (errno == EINTR) {
            printf("睡眠被中断，剩余: %ld.%09ld\n",
                   (long)rem.tv_sec, rem.tv_nsec);
            req = rem;
        } else {
            perror("nanosleep");
            break;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 +
                      (end.tv_nsec - start.tv_nsec) / 1000000;
    printf("实际睡眠: %ld ms\n", elapsed_ms);
}

/**
 * @brief 内存锁定演示
 */
void memory_lock_demo(void) {
    printf("\n=== 内存锁定 ===\n");

    /* 锁定当前进程的所有内存页面 */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        if (errno == EPERM) {
            printf("需要特权才能锁定内存\n");
        } else {
            perror("mlockall");
        }
    } else {
        printf("所有内存页面已锁定（不会被交换）\n");

        /* 分配并锁定特定区域 */
        char *buffer = malloc(4096);
        if (buffer && mlock(buffer, 4096) == 0) {
            printf("特定内存区域已锁定\n");
            munlock(buffer, 4096);
        }

        /* 解锁所有内存 */
        munlockall();
        printf("内存已解锁\n");
    }
}

/**
 * @brief 调度策略和优先级
 */
void scheduling_demo(void) {
    int policy;
    struct sched_param param;

    printf("\n=== 调度策略 ===\n");

    /* 获取当前调度策略 */
    policy = sched_getscheduler(0);
    if (policy == -1) {
        perror("sched_getscheduler");
        return;
    }

    printf("当前调度策略: ");
    switch (policy) {
        case SCHED_OTHER:  printf("SCHED_OTHER (普通)\n"); break;
        case SCHED_FIFO:   printf("SCHED_FIFO (先进先出)\n"); break;
        case SCHED_RR:     printf("SCHED_RR (轮询)\n"); break;
#ifdef SCHED_BATCH
        case SCHED_BATCH:  printf("SCHED_BATCH (批处理)\n"); break;
#endif
        default:           printf("未知 (%d)\n", policy);
    }

    /* 获取当前优先级 */
    if (sched_getparam(0, &param) == 0) {
        printf("当前优先级: %d\n", param.sched_priority);
    }

    /* 获取优先级范围 */
    int min_prio = sched_get_priority_min(SCHED_FIFO);
    int max_prio = sched_get_priority_max(SCHED_FIFO);
    printf("SCHED_FIFO优先级范围: %d - %d\n", min_prio, max_prio);

    /* 尝试设置为实时调度（需要特权） */
    param.sched_priority = min_prio + (max_prio - min_prio) / 2;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        if (errno == EPERM) {
            printf("需要特权才能设置实时调度\n");
        } else {
            perror("sched_setscheduler");
        }
    }
}

/**
 * @brief 消息队列（POSIX）
 */
#include <mqueue.h>

#define MQ_NAME "/posix_mq_demo"
#define MQ_MAX_MSG 10
#define MQ_MSG_SIZE 256

void posix_message_queue_demo(void) {
    mqd_t mq;
    struct mq_attr attr = {0, MQ_MAX_MSG, MQ_MSG_SIZE, 0, {0}};
    char buffer[MQ_MSG_SIZE];
    unsigned int prio;

    printf("\n=== POSIX消息队列 ===\n");

    /* 删除旧的消息队列 */
    mq_unlink(MQ_NAME);

    /* 创建消息队列 */
    mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return;
    }

    /* 发送消息 */
    const char *msg = "Hello from POSIX message queue!";
    if (mq_send(mq, msg, strlen(msg) + 1, 0) == -1) {
        perror("mq_send");
    } else {
        printf("消息已发送: %s\n", msg);
    }

    /* 接收消息 */
    ssize_t bytes = mq_receive(mq, buffer, MQ_MSG_SIZE, &prio);
    if (bytes > 0) {
        printf("消息已接收: %s (优先级: %u)\n", buffer, prio);
    }

    /* 获取消息队列属性 */
    struct mq_attr current_attr;
    if (mq_getattr(mq, &current_attr) == 0) {
        printf("队列属性: max_msg=%ld, msg_size=%ld, cur_msgs=%ld\n",
               (long)current_attr.mq_maxmsg,
               (long)current_attr.mq_msgsize,
               (long)current_attr.mq_curmsgs);
    }

    mq_close(mq);
    mq_unlink(MQ_NAME);
}

/**
 * @brief 异步I/O演示
 */
#include <aio.h>

void async_io_demo(void) {
    printf("\n=== 异步I/O ===\n");

    /* 创建测试文件 */
    const char *filename = "aio_test.txt";
    FILE *fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "Async I/O test content.\n");
        fclose(fp);
    }

    /* 打开文件用于异步读取 */
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    /* 设置异步I/O控制块 */
    struct aiocb cb;
    char buffer[256];

    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes = fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer) - 1;
    cb.aio_offset = 0;

    /* 发起异步读请求 */
    if (aio_read(&cb) == -1) {
        perror("aio_read");
        close(fd);
        return;
    }

    printf("异步读请求已提交\n");

    /* 等待I/O完成 */
    while (aio_error(&cb) == EINPROGRESS) {
        printf("I/O进行中...\n");
        usleep(100000);
    }

    /* 获取结果 */
    ssize_t bytes = aio_return(&cb);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("异步读取完成: %ld bytes\n", (long)bytes);
        printf("内容: %s", buffer);
    }

    close(fd);
    unlink(filename);
}

int main(void) {
    printf("=== POSIX实时扩展演示 ===\n");

    high_precision_clock_demo();
    high_precision_sleep_demo();
    memory_lock_demo();
    scheduling_demo();
    posix_message_queue_demo();
    async_io_demo();

    printf("\n=== 实时扩展演示完成 ===\n");
    return 0;
}
```


---

## 11. 实际项目: POSIX兼容Shell实现

### 11.1 项目概述

本章节实现一个简化但功能完整的POSIX兼容Shell，演示以下核心概念：

- 命令解析和执行
- 输入输出重定向
- 管道
- 环境变量
- 作业控制
- 信号处理

### 11.2 完整Shell实现代码

```c
/**
 * @file posix_shell.c
 * @brief POSIX兼容Shell实现
 * @description 一个简化但功能完整的Shell，展示POSIX系统编程实践
 *
 * 功能特性:
 * - 命令执行（带参数）
 * - 输入/输出重定向 (<, >, >>)
 * - 管道 (|)
 * - 后台执行 (&)
 * - 环境变量管理
 * - 信号处理
 * - 内置命令 (cd, pwd, exit, export, jobs, fg, bg)
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>

/* 常量定义 */
#define MAX_INPUT 4096
#define MAX_ARGS 256
#define MAX_PIPES 16
#define MAX_JOBS 32
#define MAX_ENV 256
#define PROMPT_SIZE 256

/* 作业状态 */
typedef enum {
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} JobStatus;

/* 作业结构 */
typedef struct {
    pid_t pgid;
    char command[MAX_INPUT];
    JobStatus status;
    int background;
} Job;

/* 全局变量 */
static Job jobs[MAX_JOBS];
static int num_jobs = 0;
static int shell_terminal;
static pid_t shell_pgid;
static volatile sig_atomic_t got_sigchld = 0;

/**
 * @brief 初始化Shell环境
 */
void init_shell(void) {
    /* 确保Shell在交互式模式下运行在前台 */
    shell_terminal = STDIN_FILENO;
    shell_pgid = getpgrp();

    /* 如果Shell不是进程组长，则设置为进程组长 */
    while (tcgetpgrp(shell_terminal) != shell_pgid) {
        kill(-shell_pgid, SIGTTIN);
    }

    /* 忽略交互式作业控制信号 */
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    /* 将自己放入新的进程组，并获取终端控制 */
    shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid) < 0) {
        perror("Couldn't put the shell in its own process group");
        exit(1);
    }

    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);

    /* 初始化作业表 */
    memset(jobs, 0, sizeof(jobs));
}

/**
 * @brief 创建提示符
 */
void make_prompt(char *prompt, size_t size) {
    char cwd[1024];
    const char *home = getenv("HOME");

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        /* 用~替换HOME目录 */
        if (home && strstr(cwd, home) == cwd) {
            snprintf(prompt, size, "[POSIX-Shell] ~%s$ ", cwd + strlen(home));
        } else {
            snprintf(prompt, size, "[POSIX-Shell] %s$ ", cwd);
        }
    } else {
        snprintf(prompt, size, "[POSIX-Shell] $ ");
    }
}

/**
 * @brief 解析命令行参数
 * @param line 输入行
 * @param args 参数数组
 * @return 参数数量
 */
int parse_args(char *line, char **args) {
    int argc = 0;
    char *p = line;

    while (*p && argc < MAX_ARGS - 1) {
        /* 跳过空白 */
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;

        /* 处理引号 */
        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            args[argc] = p;
            while (*p && *p != quote) p++;
            if (*p) *p++ = '\0';
        } else {
            args[argc] = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) *p++ = '\0';
        }
        argc++;
    }

    args[argc] = NULL;
    return argc;
}

/**
 * @brief 查找重定向符号
 * @param args 参数数组
 * @param symbol 重定向符号
 * @return 重定向目标文件名，NULL表示没有找到
 */
char *find_redirect(char **args, const char *symbol) {
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], symbol) == 0) {
            if (args[i + 1]) {
                /* 移除重定向部分 */
                char *file = strdup(args[i + 1]);
                args[i] = NULL;
                return file;
            }
        }
    }
    return NULL;
}

/**
 * @brief 处理输入重定向
 */
int handle_input_redirect(char *filename) {
    if (!filename) return 0;

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror(filename);
        free(filename);
        return -1;
    }

    dup2(fd, STDIN_FILENO);
    close(fd);
    free(filename);
    return 0;
}

/**
 * @brief 处理输出重定向
 */
int handle_output_redirect(char *filename, int append) {
    if (!filename) return 0;

    int flags = O_WRONLY | O_CREAT;
    if (append) {
        flags |= O_APPEND;
    } else {
        flags |= O_TRUNC;
    }

    int fd = open(filename, flags, 0644);
    if (fd < 0) {
        perror(filename);
        free(filename);
        return -1;
    }

    dup2(fd, STDOUT_FILENO);
    close(fd);
    free(filename);
    return 0;
}

/**
 * @brief 执行外部命令
 */
int execute_command(char **args, int background) {
    pid_t pid;
    int status;

    /* 处理重定向 */
    char *infile = find_redirect(args, "<");
    char *outfile = find_redirect(args, ">");
    char *appendfile = find_redirect(args, ">>");

    if (outfile && appendfile) {
        fprintf(stderr, "Cannot use both > and >>\n");
        free(infile);
        free(outfile);
        free(appendfile);
        return 1;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

        /* 处理重定向 */
        if (infile && handle_input_redirect(infile) < 0) {
            exit(1);
        }
        if (outfile && handle_output_redirect(outfile, 0) < 0) {
            exit(1);
        }
        if (appendfile && handle_output_redirect(appendfile, 1) < 0) {
            exit(1);
        }

        /* 执行命令 */
        execvp(args[0], args);
        perror(args[0]);
        exit(127);
    } else if (pid < 0) {
        perror("fork");
        return 1;
    }

    /* 父进程 */
    if (!background) {
        waitpid(pid, &status, WUNTRACED);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    } else {
        printf("[%d] %d\n", num_jobs + 1, pid);
        /* 添加到作业表 */
        if (num_jobs < MAX_JOBS) {
            jobs[num_jobs].pgid = pid;
            strncpy(jobs[num_jobs].command, args[0], MAX_INPUT - 1);
            jobs[num_jobs].status = JOB_RUNNING;
            jobs[num_jobs].background = 1;
            num_jobs++;
        }
        return 0;
    }
}

/**
 * @brief 执行管道命令
 */
int execute_pipeline(char **commands, int num_commands) {
    int pipes[MAX_PIPES][2];
    pid_t pids[MAX_PIPES];
    char *args[MAX_ARGS];

    /* 创建所有管道 */
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return 1;
        }
    }

    /* 创建子进程 */
    for (int i = 0; i < num_commands; i++) {
        parse_args(commands[i], args);
        if (!args[0]) continue;

        pids[i] = fork();
        if (pids[i] == 0) {
            /* 子进程 */
            signal(SIGINT, SIG_DFL);

            /* 如果不是第一个命令，从上一个管道读取 */
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            /* 如果不是最后一个命令，写入下一个管道 */
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            /* 关闭所有管道描述符 */
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(args[0], args);
            perror(args[0]);
            exit(127);
        } else if (pids[i] < 0) {
            perror("fork");
            return 1;
        }
    }

    /* 父进程：关闭所有管道 */
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /* 等待所有子进程 */
    int status;
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], &status, 0);
    }

    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

/**
 * @brief 内置命令: cd
 */
int builtin_cd(char **args) {
    const char *dir = args[1];
    if (!dir) {
        dir = getenv("HOME");
        if (!dir) {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }

    if (chdir(dir) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

/**
 * @brief 内置命令: pwd
 */
int builtin_pwd(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s\n", cwd);
        return 0;
    }
    perror("pwd");
    return 1;
}

/**
 * @brief 内置命令: export
 */
int builtin_export(char **args) {
    if (!args[1]) {
        /* 打印所有环境变量 */
        extern char **environ;
        for (char **env = environ; *env; env++) {
            printf("%s\n", *env);
        }
        return 0;
    }

    /* 设置环境变量 */
    char *eq = strchr(args[1], '=');
    if (eq) {
        *eq = '\0';
        setenv(args[1], eq + 1, 1);
    } else {
        /* 没有值，只设置为空 */
        setenv(args[1], "", 1);
    }
    return 0;
}

/**
 * @brief 内置命令: jobs
 */
int builtin_jobs(void) {
    for (int i = 0; i < num_jobs; i++) {
        int status;
        pid_t result = waitpid(jobs[i].pgid, &status, WNOHANG);

        if (result == 0) {
            /* 仍在运行 */
            printf("[%d] %s %s\n", i + 1,
                   jobs[i].status == JOB_RUNNING ? "Running" : "Stopped",
                   jobs[i].command);
        } else {
            jobs[i].status = JOB_DONE;
        }
    }
    return 0;
}

/**
 * @brief 内置命令: echo
 */
int builtin_echo(char **args) {
    for (int i = 1; args[i]; i++) {
        printf("%s", args[i]);
        if (args[i + 1]) printf(" ");
    }
    printf("\n");
    return 0;
}

/**
 * @brief 检查并执行内置命令
 * @return 1表示执行了内置命令，0表示不是内置命令
 */
int try_builtin(char **args) {
    if (!args[0]) return 1;

    if (strcmp(args[0], "cd") == 0) {
        builtin_cd(args);
        return 1;
    }
    if (strcmp(args[0], "pwd") == 0) {
        builtin_pwd();
        return 1;
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(args[1] ? atoi(args[1]) : 0);
    }
    if (strcmp(args[0], "export") == 0) {
        builtin_export(args);
        return 1;
    }
    if (strcmp(args[0], "jobs") == 0) {
        builtin_jobs();
        return 1;
    }
    if (strcmp(args[0], "echo") == 0) {
        builtin_echo(args);
        return 1;
    }

    return 0;
}

/**
 * @brief 解析管道
 * @param line 输入行
 * @param commands 命令数组
 * @return 命令数量
 */
int parse_pipes(char *line, char **commands) {
    int num = 0;
    char *token = strtok(line, "|");

    while (token && num < MAX_PIPES) {
        /* 去掉前后空白 */
        while (*token && isspace((unsigned char)*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) *end-- = '\0';

        commands[num++] = token;
        token = strtok(NULL, "|");
    }

    return num;
}

/**
 * @brief SIGCHLD信号处理
 */
void sigchld_handler(int sig) {
    (void)sig;
    got_sigchld = 1;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/**
 * @brief 主循环
 */
void shell_loop(void) {
    char *input;
    char prompt[PROMPT_SIZE];
    char *commands[MAX_PIPES];
    char *args[MAX_ARGS];

    /* 设置SIGCHLD处理 */
    signal(SIGCHLD, sigchld_handler);

    while (1) {
        make_prompt(prompt, sizeof(prompt));

        /* 读取输入 */
        input = readline(prompt);
        if (!input) {
            /* EOF */
            printf("\nexit\n");
            break;
        }

        /* 忽略空行 */
        char *p = input;
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) {
            free(input);
            continue;
        }

        /* 添加到历史 */
        add_history(input);

        /* 检查后台执行 */
        int background = 0;
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '&') {
            background = 1;
            input[len - 1] = '\0';
            /* 去掉末尾空白 */
            len--;
            while (len > 0 && isspace((unsigned char)input[len - 1])) {
                input[--len] = '\0';
            }
        }

        /* 解析管道 */
        int num_commands = parse_pipes(input, commands);

        if (num_commands == 1) {
            /* 单个命令 */
            parse_args(commands[0], args);
            if (!try_builtin(args)) {
                execute_command(args, background);
            }
        } else if (num_commands > 1) {
            /* 管道命令 */
            execute_pipeline(commands, num_commands);
        }

        /* 处理完成的作业 */
        if (got_sigchld) {
            got_sigchld = 0;
            for (int i = 0; i < num_jobs; i++) {
                int status;
                pid_t result = waitpid(jobs[i].pgid, &status, WNOHANG);
                if (result > 0) {
                    printf("[%d]+ Done\t%s\n", i + 1, jobs[i].command);
                    jobs[i].status = JOB_DONE;
                }
            }
        }

        free(input);
    }
}

/**
 * @brief 主函数
 */
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════╗\n");
    printf("║     POSIX Shell - 系统编程示例           ║\n");
    printf("║  演示: fork, exec, pipe, signal, job     ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");

    init_shell();
    shell_loop();

    return 0;
}
```

### 11.3 Shell测试用例

```c
/**
 * @file test_shell.c
 * @brief Shell功能测试
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

/* 测试辅助函数 */

/**
 * @brief 测试命令解析
 */
void test_parse_args(void) {
    printf("测试命令解析...\n");

    char line[] = "ls -la /home";
    char *args[256];
    int argc = 0;

    char *p = line;
    while (*p && argc < 255) {
        while (*p && *p == ' ') p++;
        if (!*p) break;
        args[argc] = p;
        while (*p && *p != ' ') p++;
        if (*p) *p++ = '\0';
        argc++;
    }
    args[argc] = NULL;

    assert(argc == 3);
    assert(strcmp(args[0], "ls") == 0);
    assert(strcmp(args[1], "-la") == 0);
    assert(strcmp(args[2], "/home") == 0);

    printf("命令解析测试通过\n");
}

/**
 * @brief 测试管道解析
 */
void test_parse_pipes(void) {
    printf("测试管道解析...\n");

    char line[] = "cat file.txt | grep pattern | wc -l";
    char *commands[16];
    int num = 0;

    char *token = strtok(line, "|");
    while (token && num < 16) {
        commands[num++] = token;
        token = strtok(NULL, "|");
    }

    assert(num == 3);
    printf("管道解析测试通过\n");
}

/**
 * @brief 测试重定向检测
 */
void test_redirect_detection(void) {
    printf("测试重定向检测...\n");

    char line[] = "cat < input.txt > output.txt";
    assert(strstr(line, "<") != NULL);
    assert(strstr(line, ">") != NULL);

    printf("重定向检测测试通过\n");
}

/**
 * @brief 测试环境变量处理
 */
void test_environment(void) {
    printf("测试环境变量...\n");

    setenv("TEST_VAR", "test_value", 1);
    char *val = getenv("TEST_VAR");
    assert(val != NULL);
    assert(strcmp(val, "test_value") == 0);

    unsetenv("TEST_VAR");
    assert(getenv("TEST_VAR") == NULL);

    printf("环境变量测试通过\n");
}

/**
 * @brief 运行所有测试
 */
void run_tests(void) {
    printf("\n=== 运行Shell功能测试 ===\n\n");

    test_parse_args();
    test_parse_pipes();
    test_redirect_detection();
    test_environment();

    printf("\n=== 所有测试通过 ===\n");
}

int main(void) {
    run_tests();
    return 0;
}
```

### 11.4 Makefile

```makefile
# POSIX Shell Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=202405L
LDFLAGS = -lreadline

TARGET = posix_shell
TEST_TARGET = test_shell

.PHONY: all clean test

all: $(TARGET) $(TEST_TARGET)

$(TARGET): posix_shell.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(TEST_TARGET): test_shell.c
 $(CC) $(CFLAGS) -o $@ $<

test: $(TEST_TARGET)
 ./$(TEST_TARGET)

clean:
 rm -f $(TARGET) $(TEST_TARGET)
```

### 11.5 使用示例

```bash
# 编译Shell
make

# 运行Shell
./posix_shell

# 基本命令
ls -la
pwd
cd /tmp

# 重定向
echo "Hello World" > test.txt
cat < test.txt
echo "Appended" >> test.txt

# 管道
ps aux | grep bash | wc -l

# 后台执行
sleep 10 &
jobs

# 环境变量
export MY_VAR=hello
echo $MY_VAR
export

# 退出
exit
```

---

## 12. 附录

### 12.1 POSIX版本宏定义

| 宏定义 | 值 | 说明 |
|--------|-----|------|
| _POSIX_VERSION | 202405L | POSIX.1-2024版本 |
| _POSIX_C_SOURCE | 202405L | 启用POSIX.1-2024特性 |
| _XOPEN_SOURCE | 700 | XPG7/SUSv4 |
| _GNU_SOURCE | 1 | GNU扩展 |
| _BSD_SOURCE | 1 | BSD扩展 |

### 12.2 常用头文件

```c
#include <unistd.h>      /* 系统调用 */
#include <fcntl.h>       /* 文件控制 */
#include <sys/types.h>   /* 基本类型 */
#include <sys/stat.h>    /* 文件状态 */
#include <sys/wait.h>    /* 进程等待 */
#include <signal.h>      /* 信号处理 */
#include <pthread.h>     /* 线程 */
#include <semaphore.h>   /* 信号量 */
#include <sys/mman.h>    /* 内存映射 */
#include <sys/socket.h>  /* Socket */
#include <netinet/in.h>  /* 网络地址 */
#include <arpa/inet.h>   /* 网络转换 */
```

### 12.3 编译选项

```bash
# Linux/gcc
gcc -std=c99 -D_POSIX_C_SOURCE=202405L -o program program.c

# Linux/gcc with pthread
gcc -std=c99 -D_POSIX_C_SOURCE=202405L -pthread -o program program.c

# Linux/gcc with realtime
gcc -std=c99 -D_POSIX_C_SOURCE=202405L -lrt -o program program.c
```

---

## 总结

本指南全面介绍了POSIX.1-2024系统接口编程的核心内容：

1. **POSIX基础**: 版本演进、特性检测、系统限制
2. **文件I/O**: 基础操作、内存映射、非阻塞I/O、文件锁定
3. **文件系统**: 目录操作、权限管理、符号链接
4. **进程控制**: fork/exec、进程组、守护进程
5. **IPC机制**: 管道、System V IPC、POSIX IPC
6. **信号处理**: 信号安装、阻塞、等待、实时信号
7. **线程管理**: 创建、同步、TLS、取消
8. **同步原语**: 互斥锁、条件变量、读写锁、屏障
9. **网络编程**: TCP/UDP Socket、I/O多路复用
10. **实时扩展**: 高精度时钟、内存锁定、调度策略
11. **实际项目**: 完整的POSIX兼容Shell实现

掌握这些知识将使你能够开发跨平台的、符合POSIX标准的系统级应用程序。

---

*文档版本: 1.0*
*最后更新: 2024年*
*基于: POSIX.1-2024 (IEEE Std 1003.1-2024)*


---

## 深入理解

### 核心概念

本主题的核心概念包括：基础理论、实现机制、实际应用。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 学习建议

1. 先理解基础概念
2. 再进行实践练习
3. 最后深入源码

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
