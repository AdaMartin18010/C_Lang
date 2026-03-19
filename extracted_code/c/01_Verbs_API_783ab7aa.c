/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API.md
 * Line: 182
 * Language: c
 * Block ID: 783ab7aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 创建保护域和完成队列 */
int rdma_create_resources(RDMACtx *rdma, size_t buf_size, int cq_size) {
    /* 创建保护域（PD）- 所有资源的容器 */
    rdma->pd = ibv_alloc_pd(rdma->ctx);
    if (!rdma->pd) {
        perror("ibv_alloc_pd");
        return -1;
    }

    /* 分配并注册内存区域 */
    rdma->buf_size = buf_size;
    rdma->buf = aligned_alloc(4096, buf_size);
    if (!rdma->buf) {
        perror("aligned_alloc");
        return -1;
    }

    /* 注册内存区域（MR） */
    rdma->mr = ibv_reg_mr(rdma->pd, rdma->buf, buf_size,
                          IBV_ACCESS_LOCAL_WRITE |
                          IBV_ACCESS_REMOTE_READ |
                          IBV_ACCESS_REMOTE_WRITE);
    if (!rdma->mr) {
        perror("ibv_reg_mr");
        return -1;
    }

    printf("Registered MR: addr=%p, length=%zu, rkey=0x%x\n",
           rdma->buf, buf_size, rdma->mr->rkey);

    /* 创建完成队列（CQ） */
    rdma->cq = ibv_create_cq(rdma->ctx, cq_size, NULL, NULL, 0);
    if (!rdma->cq) {
        perror("ibv_create_cq");
        return -1;
    }

    return 0;
}
