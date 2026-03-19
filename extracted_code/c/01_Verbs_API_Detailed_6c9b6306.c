/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 1610
 * Language: c
 * Block ID: 6c9b6306
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RDMA原子操作实现
 */

#include <infiniband/verbs.h>
#include <stdint.h>
#include <string.h>

/**
 * Fetch_and_Add操作
 * 原子地读取远程地址的值，并加上add_val
 */
int post_fetch_and_add(struct ibv_qp *qp, uint64_t *local_addr,
                       uint32_t lkey, uint64_t remote_addr, uint32_t rkey,
                       uint64_t add_val) {
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    sge.addr = (uint64_t)local_addr;
    sge.length = sizeof(uint64_t);
    sge.lkey = lkey;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 20;
    wr.opcode = IBV_WR_FETCH_AND_ADD;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.wr.atomic.remote_addr = remote_addr;
    wr.wr.atomic.compare_add = add_val;
    wr.wr.atomic.rkey = rkey;
    wr.send_flags = IBV_SEND_SIGNALED;

    int ret = ibv_post_send(qp, &wr, &bad_wr);
    if (ret) {
        fprintf(stderr, "Fetch_and_Add failed: %d\n", ret);
        return ret;
    }

    printf("发起Fetch_and_Add: remote=0x%lx, add=0x%lx\n",
           remote_addr, add_val);
    return 0;
}

/**
 * Compare_and_Swap操作
 * 如果远程值等于compare，则替换为swap_val
 * 返回原始值
 */
int post_compare_and_swap(struct ibv_qp *qp, uint64_t *local_addr,
                          uint32_t lkey, uint64_t remote_addr, uint32_t rkey,
                          uint64_t compare, uint64_t swap_val) {
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    sge.addr = (uint64_t)local_addr;
    sge.length = sizeof(uint64_t);
    sge.lkey = lkey;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 21;
    wr.opcode = IBV_WR_CMP_AND_SWP;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.wr.atomic.remote_addr = remote_addr;
    wr.wr.atomic.compare_add = compare;
    wr.wr.atomic.swap = swap_val;
    wr.wr.atomic.rkey = rkey;
    wr.send_flags = IBV_SEND_SIGNALED;

    int ret = ibv_post_send(qp, &wr, &bad_wr);
    if (ret) {
        fprintf(stderr, "Compare_and_Swap failed: %d\n", ret);
        return ret;
    }

    printf("发起Compare_and_Swap: remote=0x%lx, compare=0x%lx, swap=0x%lx\n",
           remote_addr, compare, swap_val);
    return 0;
}

/**
 * 基于原子操作的自旋锁
 */
typedef struct {
    uint64_t lock_value;  /* 0=解锁, 1=锁定 */
} rdma_spinlock_t;

/**
 * 尝试获取RDMA锁
 */
int rdma_trylock(struct ibv_qp *qp, uint64_t *local_result, uint32_t lkey,
                 uint64_t remote_lock_addr, uint32_t rkey) {
    /* CAS: 如果值为0（未锁定），设置为1（锁定） */
    return post_compare_and_swap(qp, local_result, lkey,
                                 remote_lock_addr, rkey, 0, 1);
}

/**
 * 释放RDMA锁
 */
int rdma_unlock(struct ibv_qp *qp, uint64_t *local_result, uint32_t lkey,
                uint64_t remote_lock_addr, uint32_t rkey) {
    /* 直接写0即可释放 */
    /* 或者使用CAS确保安全性 */
    return post_compare_and_swap(qp, local_result, lkey,
                                 remote_lock_addr, rkey, 1, 0);
}

/**
 * 原子计数器递增
 */
int atomic_counter_inc(struct ibv_qp *qp, uint64_t *local_result,
                       uint32_t lkey, uint64_t counter_addr, uint32_t rkey,
                       uint64_t increment) {
    return post_fetch_and_add(qp, local_result, lkey,
                              counter_addr, rkey, increment);
}

/**
 * 等待原子操作完成
 */
int wait_for_atomic_completion(struct ibv_cq *cq, uint64_t expected_wr_id,
                               uint64_t *result) {
    struct ibv_wc wc;
    int ret;

    do {
        ret = ibv_poll_cq(cq, 1, &wc);
    } while (ret == 0);

    if (ret < 0) {
        return ret;
    }

    if (wc.status != IBV_WC_SUCCESS) {
        fprintf(stderr, "Atomic operation failed: %s\n",
                ibv_wc_status_str(wc.status));
        return -1;
    }

    if (wc.wr_id == expected_wr_id) {
        if (result) {
            *result = wc.byte_len;  /* 实际结果在本地缓冲区 */
        }
        return 0;
    }

    return -1;
}
