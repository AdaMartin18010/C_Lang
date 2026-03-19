/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\03_Kernel_Bypass.md
 * Line: 71
 * Language: c
 * Block ID: cadc6ee7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RDMA基本流程
// 1. 建立连接
// 2. 注册内存区域
// 3. 执行RDMA操作
// 4. 完成通知

// RDMA Verbs API
#include <infiniband/verbs.h>

// 创建RDMA连接
struct ibv_context* create_rdma_context(void) {
    struct ibv_device** dev_list = ibv_get_device_list(NULL);
    if (!dev_list) return NULL;

    struct ibv_context* ctx = ibv_open_device(dev_list[0]);
    ibv_free_device_list(dev_list);

    return ctx;
}

// 注册内存区域
struct ibv_mr* register_memory(struct ibv_pd* pd, void* addr, size_t len) {
    return ibv_reg_mr(
        pd,
        addr,
        len,
        IBV_ACCESS_LOCAL_WRITE |
        IBV_ACCESS_REMOTE_READ |
        IBV_ACCESS_REMOTE_WRITE
    );
}
