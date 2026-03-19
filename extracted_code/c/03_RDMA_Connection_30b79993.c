/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 277
 * Language: c
 * Block ID: 30b79993
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 获取设备列表
struct ibv_device **dev_list = ibv_get_device_list(NULL);
struct ibv_device *device = dev_list[0];

// 2. 打开设备获取上下文
struct ibv_context *ctx = ibv_open_device(device);

// 3. 创建Protection Domain
struct ibv_pd *pd = ibv_alloc_pd(ctx);

// 4. 创建Completion Queue
struct ibv_cq *cq = ibv_create_cq(ctx, 100, NULL, NULL, 0);

// 5. 创建Queue Pair
struct ibv_qp_init_attr qp_init_attr = {
    .send_cq = cq,
    .recv_cq = cq,
    .qp_type = IBV_QPT_RC,  // Reliable Connection
    .cap = {
        .max_send_wr = 100,
        .max_recv_wr = 100,
        .max_send_sge = 1,
        .max_recv_sge = 1,
    }
};
struct ibv_qp *qp = ibv_create_qp(pd, &qp_init_attr);

// 6. 注册内存区域
void *buf = malloc(4096);
struct ibv_mr *mr = ibv_reg_mr(pd, buf, 4096,
    IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);
