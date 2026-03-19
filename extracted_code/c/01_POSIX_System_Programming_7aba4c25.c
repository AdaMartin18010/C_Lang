/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 3928
 * Language: c
 * Block ID: 7aba4c25
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
