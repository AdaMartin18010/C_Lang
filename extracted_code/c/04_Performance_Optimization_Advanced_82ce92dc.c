/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 3029
 * Language: c
 * Block ID: 82ce92dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file async_io.c
 * @brief 异步I/O与io_uring高性能实现
 *
 * io_uring: Linux 5.1+引入的高性能异步I/O接口
 * 优势：
 * - 无需系统调用提交请求（通过共享内存ring buffer）
 * - 支持批处理
 * - 低延迟、高吞吐
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <liburing.h>  /* 需要liburing库 */
#include <aio.h>       /* POSIX AIO */
#include <errno.h>

#define NUM_OPS 10000
#define BLOCK_SIZE 4096
#define QUEUE_DEPTH 256

/**
 * @brief 同步I/O基准
 */
long sync_io_bench(const char *filename) {
    int fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) return -1;

    void *buffer;
    posix_memalign(&buffer, 4096, BLOCK_SIZE);

    clock_t start = clock();
    off_t offset = 0;

    for (int i = 0; i < NUM_OPS; i++) {
        pread(fd, buffer, BLOCK_SIZE, offset);
        offset += BLOCK_SIZE;
    }

    clock_t elapsed = clock() - start;

    free(buffer);
    close(fd);
    return elapsed;
}

/**
 * @brief POSIX AIO基准
 */
long posix_aio_bench(const char *filename) {
    int fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) return -1;

    struct aiocb *cbs = calloc(NUM_OPS, sizeof(struct aiocb));
    void **buffers = calloc(NUM_OPS, sizeof(void*));

    for (int i = 0; i < NUM_OPS; i++) {
        posix_memalign(&buffers[i], 4096, BLOCK_SIZE);
    }

    clock_t start = clock();

    /* 提交所有请求 */
    for (int i = 0; i < NUM_OPS; i++) {
        memset(&cbs[i], 0, sizeof(struct aiocb));
        cbs[i].aio_fildes = fd;
        cbs[i].aio_buf = buffers[i];
        cbs[i].aio_nbytes = BLOCK_SIZE;
        cbs[i].aio_offset = i * BLOCK_SIZE;

        aio_read(&cbs[i]);
    }

    /* 等待完成 */
    for (int i = 0; i < NUM_OPS; i++) {
        struct aiocb *cb_list[1] = {&cbs[i]};
        aio_suspend(cb_list, 1, NULL);
    }

    clock_t elapsed = clock() - start;

    for (int i = 0; i < NUM_OPS; i++) {
        free(buffers[i]);
    }
    free(buffers);
    free(cbs);
    close(fd);

    return elapsed;
}

/**
 * @brief io_uring基准
 */
long io_uring_bench(const char *filename) {
    int fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) return -1;

    struct io_uring ring;
    if (io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0) {
        close(fd);
        return -1;
    }

    void **buffers = calloc(NUM_OPS, sizeof(void*));
    for (int i = 0; i < NUM_OPS; i++) {
        posix_memalign(&buffers[i], 4096, BLOCK_SIZE);
    }

    clock_t start = clock();

    /* 批量提交请求 */
    int submitted = 0;
    while (submitted < NUM_OPS) {
        int batch = (NUM_OPS - submitted < QUEUE_DEPTH) ?
                    (NUM_OPS - submitted) : QUEUE_DEPTH;

        for (int i = 0; i < batch; i++) {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
            io_uring_prep_read(sqe, fd, buffers[submitted],
                              BLOCK_SIZE, submitted * BLOCK_SIZE);
            submitted++;
        }

        io_uring_submit(&ring);

        /* 等待这批完成 */
        for (int i = 0; i < batch; i++) {
            struct io_uring_cqe *cqe;
            io_uring_wait_cqe(&ring, &cqe);
            io_uring_cqe_seen(&ring, cqe);
        }
    }

    clock_t elapsed = clock() - start;

    for (int i = 0; i < NUM_OPS; i++) {
        free(buffers[i]);
    }
    free(buffers);
    io_uring_queue_exit(&ring);
    close(fd);

    return elapsed;
}

/**
 * @brief io_uring高级用法 - 轮询模式
 *
 * IORING_SETUP_IOPOLL: 内核轮询完成
 * IORING_SETUP_SQPOLL: 内核轮询提交队列
 */
long io_uring_poll_bench(const char *filename) {
    int fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) return -1;

    struct io_uring ring;
    struct io_uring_params params = {};
    params.flags = IORING_SETUP_IOPOLL;  /* 内核轮询模式 */

    if (io_uring_queue_init_params(QUEUE_DEPTH, &ring, &params) < 0) {
        close(fd);
        return -1;
    }

    void **buffers = calloc(NUM_OPS, sizeof(void*));
    for (int i = 0; i < NUM_OPS; i++) {
        posix_memalign(&buffers[i], 4096, BLOCK_SIZE);
    }

    clock_t start = clock();

    /* 提交并立即轮询完成 */
    int submitted = 0;
    while (submitted < NUM_OPS) {
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_read(sqe, fd, buffers[submitted],
                          BLOCK_SIZE, submitted * BLOCK_SIZE);
        sqe->user_data = submitted;

        io_uring_submit(&ring);

        /* 轮询等待这个请求完成 */
        struct io_uring_cqe *cqe;
        do {
            io_uring_peek_cqe(&ring, &cqe);
        } while (!cqe);

        io_uring_cqe_seen(&ring, cqe);
        submitted++;
    }

    clock_t elapsed = clock() - start;

    for (int i = 0; i < NUM_OPS; i++) {
        free(buffers[i]);
    }
    free(buffers);
    io_uring_queue_exit(&ring);
    close(fd);

    return elapsed;
}

void create_test_file(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return;

    /* 创建足够大的文件 */
    ftruncate(fd, (off_t)NUM_OPS * BLOCK_SIZE);
    close(fd);
}

void benchmark_async_io(void) {
    printf("=== Async I/O Benchmark (%d ops) ===\n\n", NUM_OPS);

    const char *filename = "/tmp/async_io_test.dat";
    create_test_file(filename);

    /* 同步I/O */
    printf("1. Synchronous pread:\n");
    long sync_time = sync_io_bench(filename);
    printf("   Time: %ld ms\n", sync_time * 1000 / CLOCKS_PER_SEC);

    /* POSIX AIO */
    printf("\n2. POSIX AIO:\n");
    long aio_time = posix_aio_bench(filename);
    if (aio_time > 0) {
        printf("   Time: %ld ms (%.2fx)\n",
               aio_time * 1000 / CLOCKS_PER_SEC,
               (double)sync_time / aio_time);
    } else {
        printf("   Not supported or failed\n");
    }

    /* io_uring */
    printf("\n3. io_uring:\n");
    long uring_time = io_uring_bench(filename);
    if (uring_time > 0) {
        printf("   Time: %ld ms (%.2fx vs sync)\n",
               uring_time * 1000 / CLOCKS_PER_SEC,
               (double)sync_time / uring_time);
    } else {
        printf("   Not supported (requires liburing)\n");
    }

    unlink(filename);
}

int main(void) {
    benchmark_async_io();
    return 0;
}
