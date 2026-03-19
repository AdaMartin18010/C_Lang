/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 186
 * Language: c
 * Block ID: 5c9d1dc1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
