/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 2815
 * Language: c
 * Block ID: 2589947c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file zero_copy.c
 * @brief 零拷贝I/O优化技术
 *
 * 传统I/O: 磁盘 -> 内核页缓存 -> 用户缓冲区 -> 内核socket缓冲区 -> 网卡
 * 零拷贝:   磁盘 -> 内核页缓存 -> 网卡 (通过sendfile)
 *
 * 减少数据复制次数，降低CPU使用和内存带宽
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>

#define FILE_SIZE (100 * 1024 * 1024)  /* 100MB */

/**
 * @brief 传统文件复制（read/write）
 */
long traditional_copy(int src_fd, int dst_fd, size_t size) {
    char *buffer = malloc(64 * 1024);  /* 64KB缓冲区 */
    if (!buffer) return -1;

    ssize_t total = 0;
    while (total < (ssize_t)size) {
        ssize_t n = read(src_fd, buffer, 64 * 1024);
        if (n <= 0) break;

        ssize_t written = write(dst_fd, buffer, n);
        if (written < 0) break;

        total += written;
    }

    free(buffer);
    return total;
}

/**
 * @brief 零拷贝文件传输（sendfile）
 */
long zero_copy_sendfile(int src_fd, int dst_fd, size_t size) {
    off_t offset = 0;
    ssize_t total = 0;

    while (total < (ssize_t)size) {
        ssize_t n = sendfile(dst_fd, src_fd, &offset, size - total);
        if (n <= 0) {
            if (errno == EINTR) continue;
            break;
        }
        total += n;
    }

    return total;
}

/**
 * @brief 使用mmap的零拷贝
 *
 * 文件直接映射到进程地址空间，无需read/write
 */
long mmap_copy(int src_fd, int dst_fd, size_t size) {
    void *src = mmap(NULL, size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src == MAP_FAILED) return -1;

    /* 扩展目标文件 */
    ftruncate(dst_fd, size);

    void *dst = mmap(NULL, size, PROT_WRITE, MAP_SHARED, dst_fd, 0);
    if (dst == MAP_FAILED) {
        munmap(src, size);
        return -1;
    }

    /* 直接内存复制（单拷贝） */
    memcpy(dst, src, size);

    munmap(src, size);
    munmap(dst, size);

    return size;
}

/**
 * @brief 使用splice的管道零拷贝
 *
 * splice: 在内核地址空间之间移动数据，无需用户空间参与
 */
long splice_copy(int src_fd, int dst_fd, size_t size) {
    int pipefd[2];
    if (pipe(pipefd) < 0) return -1;

    ssize_t total = 0;
    while (total < (ssize_t)size) {
        /* 从文件到管道 */
        ssize_t n = splice(src_fd, NULL, pipefd[1], NULL,
                          64 * 1024, SPLICE_F_MOVE);
        if (n <= 0) break;

        /* 从管道到目标 */
        ssize_t m = splice(pipefd[0], NULL, dst_fd, NULL, n, SPLICE_F_MOVE);
        if (m <= 0) break;

        total += m;
    }

    close(pipefd[0]);
    close(pipefd[1]);
    return total;
}

void create_test_file(const char *filename, size_t size) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return;

    /* 快速创建大文件 */
    ftruncate(fd, size);

    /* 写入一些数据 */
    char *buf = malloc(1024 * 1024);
    for (size_t i = 0; i < size; i += 1024 * 1024) {
        memset(buf, 'A' + (i / (1024 * 1024)) % 26, 1024 * 1024);
        write(fd, buf, 1024 * 1024);
    }

    free(buf);
    close(fd);
}

void benchmark_zero_copy(void) {
    printf("=== Zero Copy Benchmark (%d MB file) ===\n\n", FILE_SIZE / (1024*1024));

    const char *src_file = "/tmp/zero_copy_src.dat";
    const char *dst_file = "/tmp/zero_copy_dst.dat";

    /* 创建源文件 */
    printf("Creating test file...\n");
    create_test_file(src_file, FILE_SIZE);

    struct stat st;
    stat(src_file, &st);
    size_t file_size = st.st_size;

    /* 预热磁盘缓存 */
    int fd = open(src_file, O_RDONLY);
    char *tmp = malloc(file_size);
    read(fd, tmp, file_size);
    close(fd);
    free(tmp);

    /* 测试1: 传统复制 */
    printf("\n1. Traditional read/write:\n");
    fd = open(src_file, O_RDONLY);
    int dst_fd = open(dst_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    clock_t start = clock();
    traditional_copy(fd, dst_fd, file_size);
    clock_t trad_time = clock() - start;
    close(fd);
    close(dst_fd);
    printf("   Time: %ld ms (%.2f MB/s)\n",
           trad_time * 1000 / CLOCKS_PER_SEC,
           (file_size / (1024.0 * 1024)) / (trad_time / (double)CLOCKS_PER_SEC));

    /* 测试2: sendfile零拷贝 */
    printf("\n2. Zero-copy sendfile:\n");
    fd = open(src_file, O_RDONLY);
    dst_fd = open(dst_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    start = clock();
    zero_copy_sendfile(fd, dst_fd, file_size);
    clock_t sendfile_time = clock() - start;
    close(fd);
    close(dst_fd);
    printf("   Time: %ld ms (%.2f MB/s, %.2fx speedup)\n",
           sendfile_time * 1000 / CLOCKS_PER_SEC,
           (file_size / (1024.0 * 1024)) / (sendfile_time / (double)CLOCKS_PER_SEC),
           (double)trad_time / sendfile_time);

    /* 测试3: mmap复制 */
    printf("\n3. mmap copy:\n");
    fd = open(src_file, O_RDONLY);
    dst_fd = open(dst_file, O_RDWR | O_CREAT | O_TRUNC, 0644);
    start = clock();
    mmap_copy(fd, dst_fd, file_size);
    clock_t mmap_time = clock() - start;
    close(fd);
    close(dst_fd);
    printf("   Time: %ld ms (%.2f MB/s, %.2fx speedup)\n",
           mmap_time * 1000 / CLOCKS_PER_SEC,
           (file_size / (1024.0 * 1024)) / (mmap_time / (double)CLOCKS_PER_SEC),
           (double)trad_time / mmap_time);

    /* 清理 */
    unlink(src_file);
    unlink(dst_file);
}

int main(void) {
    benchmark_zero_copy();
    return 0;
}
