/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\08_Debugging_Tools\03_Advanced_Debugging_Techniques.md
 * Line: 277
 * Language: c
 * Block ID: 6992b6aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * eBPF加载器 - 用户空间程序
 *
 * 负责加载eBPF程序到内核，并处理从内核传来的事件
 * 编译：gcc -o ebpf_loader ebpf_loader.c -lbpf -lelf -lz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include "trace_open.skel.h"  /* 由bpftool生成的骨架代码 */

/* 信号处理标志 */
static volatile sig_atomic_t exiting = 0;

void sig_handler(int sig)
{
    exiting = 1;
}

/**
 * libbpf日志回调函数
 * 用于调试eBPF加载过程中的问题
 */
static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
    /* 只显示警告和错误 */
    if (level > LIBBPF_WARN)
        return 0;
    return vfprintf(stderr, format, args);
}

/**
 * 处理从环形缓冲区接收的事件
 */
static int handle_event(void *ctx, void *data, size_t data_sz)
{
    struct event *e = data;

    /* 打印事件信息 */
    printf("[%llu] PID: %d, UID: %d, Comm: %s, File: %s\n",
           e->timestamp / 1000000000,  /* 转换为秒 */
           e->pid, e->uid, e->comm, e->filename);

    return 0;
}

int main(int argc, char **argv)
{
    struct trace_open_bpf *skel;
    struct ring_buffer *rb = NULL;
    int err;

    /* 设置libbpf日志回调 */
    libbpf_set_print(libbpf_print_fn);

    /* 提升资源限制（eBPF程序需要锁定内存） */
    struct rlimit rlim = {
        .rlim_cur = RLIM_INFINITY,
        .rlim_max = RLIM_INFINITY,
    };
    setrlimit(RLIMIT_MEMLOCK, &rlim);

    /* 设置信号处理 */
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    /* 打开并加载eBPF应用程序骨架 */
    skel = trace_open_bpf__open();
    if (!skel) {
        fprintf(stderr, "Failed to open BPF skeleton\n");
        return 1;
    }

    /* 可以在这里修改eBPF程序的全局变量 */
    // skel->rodata->target_uid = 1000;

    /* 加载并验证eBPF程序 */
    err = trace_open_bpf__load(skel);
    if (err) {
        fprintf(stderr, "Failed to load and verify BPF skeleton\n");
        goto cleanup;
    }

    /* 附加eBPF程序到追踪点 */
    err = trace_open_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach BPF skeleton\n");
        goto cleanup;
    }

    /* 设置环形缓冲区轮询 */
    rb = ring_buffer__new(bpf_map__fd(skel->maps.rb), handle_event, NULL, NULL);
    if (!rb) {
        err = -1;
        fprintf(stderr, "Failed to create ring buffer\n");
        goto cleanup;
    }

    printf("Successfully started! Press Ctrl+C to stop.\n");

    /* 主事件循环 */
    while (!exiting) {
        /* 轮询环形缓冲区，超时100ms */
        err = ring_buffer__poll(rb, 100);
        if (err == -EINTR) {
            /* 被信号中断 */
            err = 0;
            break;
        }
        if (err < 0) {
            fprintf(stderr, "Error polling ring buffer: %d\n", err);
            break;
        }
    }

cleanup:
    /* 清理资源 */
    ring_buffer__free(rb);
    trace_open_bpf__destroy(skel);

    return err < 0 ? -err : 0;
}
