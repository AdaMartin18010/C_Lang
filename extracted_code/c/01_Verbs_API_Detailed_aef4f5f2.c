/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 2178
 * Language: c
 * Block ID: aef4f5f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 性能优化示例
 */

#include <infiniband/verbs.h>

#define BATCH_SIZE 32
#define INLINE_THRESHOLD 256

/**
 * 批量发送优化
 */
int batch_post_send(struct ibv_qp *qp, struct ibv_sge *sges,
                    int count, uint64_t remote_addr, uint32_t rkey,
                    int signal_interval) {
    struct ibv_send_wr wrs[BATCH_SIZE];
    struct ibv_send_wr *bad_wr;
    int i;

    if (count > BATCH_SIZE) count = BATCH_SIZE;

    /* 构建WR链 */
    for (i = 0; i < count; i++) {
        memset(&wrs[i], 0, sizeof(wrs[i]));
        wrs[i].wr_id = i;
        wrs[i].opcode = IBV_WR_RDMA_WRITE;
        wrs[i].sg_list = &sges[i];
        wrs[i].num_sge = 1;
        wrs[i].wr.rdma.remote_addr = remote_addr + i * sges[i].length;
        wrs[i].wr.rdma.rkey = rkey;

        /* 选择性信号：每N个WR产生一个完成 */
        if ((i % signal_interval) == (signal_interval - 1)) {
            wrs[i].send_flags = IBV_SEND_SIGNALED;
        }

        /* 小数据内联优化 */
        if (sges[i].length <= INLINE_THRESHOLD) {
            wrs[i].send_flags |= IBV_SEND_INLINE;
        }

        wrs[i].next = (i < count - 1) ? &wrs[i + 1] : NULL;
    }

    return ibv_post_send(qp, wrs, &bad_wr);
}

/**
 * 批量完成处理
 */
int batch_poll_completion(struct ibv_cq *cq, int batch_size) {
    struct ibv_wc wc[64];
    int total = 0;
    int num;

    do {
        num = ibv_poll_cq(cq, batch_size > 64 ? 64 : batch_size, wc);
        if (num > 0) {
            /* 批量处理完成事件 */
            for (int i = 0; i < num; i++) {
                if (wc[i].status != IBV_WC_SUCCESS) {
                    handle_wc_error(&wc[i]);
                }
                /* 处理完成的WR... */
            }
            total += num;
        }
    } while (num > 0 && total < batch_size);

    return total;
}

/**
 * 零拷贝接收优化
 */
int zero_copy_recv(struct ibv_qp *qp, rdma_buffer_t **bufs, int count) {
    /* 使用预注册的缓冲区，避免内存拷贝 */
    return post_recv_batch(qp, bufs, count);
}

/**
 * 忙等待轮询（最低延迟）
 */
int busy_poll_cq(struct ibv_cq *cq, struct ibv_wc *wc, int max_wc) {
    int num;
    /* 忙等待直到有完成 */
    do {
        num = ibv_poll_cq(cq, max_wc, wc);
    } while (num == 0);

    return num;
}
