/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2373
 * Language: c
 * Block ID: f37d5629
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * zero_copy.c - 零拷贝传输实现
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>

// ========== sendfile实现文件传输 ==========
void send_file_sendfile(int client_fd, const char *filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        return;
    }

    struct stat st;
    fstat(file_fd, &st);
    off_t offset = 0;

    // sendfile: 内核态直接传输,无需用户态缓冲
    // 参数: out_fd(必须是socket), in_fd(必须是支持mmap的文件), offset, count
    ssize_t sent = sendfile(client_fd, file_fd, &offset, st.st_size);

    if (sent < 0) {
        perror("sendfile");
    } else {
        printf("Sent %zd bytes using sendfile\n", sent);
    }

    close(file_fd);
}

// ========== splice实现管道零拷贝 ==========
#include <fcntl.h>

void transfer_with_splice(int in_fd, int out_fd, size_t len) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return;
    }

    size_t total = 0;
    while (total < len) {
        // 从in_fd splice到管道
        ssize_t n = splice(in_fd, NULL, pipefd[1], NULL,
                           len - total, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (n <= 0) break;

        // 从管道splice到out_fd
        ssize_t m = splice(pipefd[0], NULL, out_fd, NULL,
                           n, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (m <= 0) break;

        total += m;
    }

    close(pipefd[0]);
    close(pipefd[1]);

    printf("Transferred %zu bytes using splice\n", total);
}

// ========== mmap + write 实现 ==========
#include <sys/mman.h>

void send_file_mmap(int client_fd, const char *filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        return;
    }

    struct stat st;
    fstat(file_fd, &st);

    // mmap文件到内存
    void *mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(file_fd);
        return;
    }

    // 发送数据(仍然需要用户态到内核态拷贝,但比read/write少一次)
    ssize_t sent = send(client_fd, mapped, st.st_size, MSG_NOSIGNAL);

    if (sent < 0) {
        perror("send");
    } else {
        printf("Sent %zd bytes using mmap\n", sent);
    }

    munmap(mapped, st.st_size);
    close(file_fd);
}

// ========== 传统方法对比 ==========
void send_file_traditional(int client_fd, const char *filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        return;
    }

    char buffer[8192];
    ssize_t n;
    size_t total = 0;

    // 传统方法: 磁盘 -> 内核缓冲区 -> 用户缓冲区 -> 内核Socket缓冲区 -> 网卡
    while ((n = read(file_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t sent = send(client_fd, buffer, n, MSG_NOSIGNAL);
        if (sent < 0) {
            perror("send");
            break;
        }
        total += sent;
    }

    printf("Sent %zu bytes using traditional method\n", total);
    close(file_fd);
}

/**
 * 零拷贝方法对比:
 *
 * 传统方法(read/write):
 *   磁盘 -> 页缓存 -> 用户缓冲区 -> Socket缓冲区 -> 网卡
 *   [4次数据拷贝, 4次上下文切换]
 *
 * mmap + send:
 *   磁盘 -> 页缓存 -> Socket缓冲区 -> 网卡
 *   [3次数据拷贝, 4次上下文切换]
 *   注意: 如果页缓存命中,实际是2次拷贝
 *
 * sendfile:
 *   磁盘 -> 页缓存 -> Socket缓冲区 -> 网卡
 *   [2次数据拷贝(如果网卡支持SG-DMA则1次), 2次上下文切换]
 *
 * splice:
 *   管道两端都是内核缓冲区,完全零拷贝
 *   [0次数据拷贝, 2次上下文切换]
 */
