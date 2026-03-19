/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 702
 * Language: c
 * Block ID: 7bc68d7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 完成队列轮询与事件处理
 */

#include <infiniband/verbs.h>
#include <stdio.h>
#include <poll.h>

#define MAX_WC 16

/**
 * 轮询CQ（非阻塞）
 */
int poll_cq_batch(struct ibv_cq *cq, int max_wc) {
    struct ibv_wc wc[MAX_WC];
    int num_wc, i;

    num_wc = ibv_poll_cq(cq, max_wc > MAX_WC ? MAX_WC : max_wc, wc);

    if (num_wc < 0) {
        fprintf(stderr, "ibv_poll_cq failed: %d\n", num_wc);
        return -1;
    }

    for (i = 0; i < num_wc; i++) {
        if (wc[i].status != IBV_WC_SUCCESS) {
            fprintf(stderr, "WC failed: wr_id=%lu, status=%s\n",
                    (unsigned long)wc[i].wr_id,
                    ibv_wc_status_str(wc[i].status));
            continue;
        }

        /* 处理成功的完成 */
        printf("WC success: wr_id=%lu, opcode=%s, bytes=%u\n",
               (unsigned long)wc[i].wr_id,
               ibv_wc_opcode_str(wc[i].opcode),
               wc[i].byte_len);

        /* 根据操作类型处理 */
        switch (wc[i].opcode) {
            case IBV_WC_SEND:
                printf("  Send完成\n");
                break;
            case IBV_WC_RECV:
                printf("  Recv完成，接收 %u 字节\n", wc[i].byte_len);
                break;
            case IBV_WC_RDMA_WRITE:
                printf("  RDMA Write完成\n");
                break;
            case IBV_WC_RDMA_READ:
                printf("  RDMA Read完成，读取 %u 字节\n", wc[i].byte_len);
                break;
            case IBV_WC_COMP_SWAP:
            case IBV_WC_FETCH_ADD:
                printf("  原子操作完成\n");
                break;
            default:
                printf("  其他操作完成\n");
                break;
        }
    }

    return num_wc;
}

/**
 * 等待CQ事件（阻塞方式，使用完成通道）
 */
int wait_cq_event(struct ibv_cq *cq, struct ibv_comp_channel *channel) {
    struct ibv_cq *ev_cq;
    void *ev_ctx;
    int ret;

    /* 等待事件通知 */
    ret = ibv_get_cq_event(channel, &ev_cq, &ev_ctx);
    if (ret) {
        perror("ibv_get_cq_event");
        return -1;
    }

    /* 确认事件 */
    ibv_ack_cq_events(ev_cq, 1);

    /* 重新请求通知 */
    ret = ibv_req_notify_cq(cq, 0);
    if (ret) {
        perror("ibv_req_notify_cq");
        return -1;
    }

    /* 处理所有完成的WR */
    poll_cq_batch(cq, MAX_WC);

    return 0;
}

/**
 * CQ处理主循环（事件驱动）
 */
void cq_event_loop(struct ibv_cq *cq, struct ibv_comp_channel *channel,
                   volatile int *stop_flag) {
    struct pollfd pfd;

    pfd.fd = channel->fd;
    pfd.events = POLLIN;

    while (!(*stop_flag)) {
        int ret = poll(&pfd, 1, 100);  /* 100ms超时 */

        if (ret < 0) {
            perror("poll");
            break;
        }

        if (ret > 0) {
            /* 有事件到来 */
            wait_cq_event(cq, channel);
        }
    }
}

/**
 * 带超时的CQ轮询
 */
int poll_cq_timeout(struct ibv_cq *cq, int timeout_ms) {
    int elapsed = 0;
    const int poll_interval = 1;  /* 1ms */

    while (elapsed < timeout_ms) {
        int num = ibv_poll_cq(cq, 1, NULL);
        if (num > 0) {
            return num;
        }

        /* 短暂休眠避免忙等待 */
        struct timespec ts = {0, poll_interval * 1000000};
        nanosleep(&ts, NULL);
        elapsed += poll_interval;
    }

    return 0;  /* 超时 */
}
