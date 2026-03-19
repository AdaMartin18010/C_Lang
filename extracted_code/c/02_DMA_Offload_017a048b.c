/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 694
 * Language: c
 * Block ID: 017a048b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * Scatter-Gather DMA实现 - 支持非连续内存传输
 * 用于5G基带LDPC码块分散收集
 * ============================================================================ */

/* Scatter-Gather列表节点 */
typedef struct sg_node {
    void *virt_addr;                    /* 虚拟地址 */
    uint64_t phys_addr;                 /* 物理地址 */
    uint32_t len;                       /* 长度 */
    struct sg_node *next;               /* 下一节点 */
} sg_node_t;

/* SG传输请求 */
typedef struct sg_xfer_request {
    sg_node_t *src_list;                /* 源散列表 */
    sg_node_t *dst_list;                /* 目的散列表 */
    uint32_t src_nents;                 /* 源条目数 */
    uint32_t dst_nents;                 /* 目的条目数 */
    dma_xfer_type_t xfer_type;          /* 传输类型 */
    void (*complete_cb)(struct sg_xfer_request *, int status);
    void *cb_ctx;
} sg_xfer_request_t;

/* 构建SG描述符链 */
dma_descriptor_t *dma_build_sg_desc_chain(sg_xfer_request_t *req,
                                           dma_descriptor_t *pool,
                                           uint32_t pool_size)
{
    if (!req || !pool || !req->src_list || !req->dst_list) {
        return NULL;
    }

    /* 计算总传输大小 */
    uint32_t total_src_len = 0, total_dst_len = 0;
    sg_node_t *node;

    for (node = req->src_list; node; node = node->next) {
        total_src_len += node->len;
    }
    for (node = req->dst_list; node; node = node->next) {
        total_dst_len += node->len;
    }

    if (total_src_len != total_dst_len) {
        return NULL;  /* 源目长度不匹配 */
    }

    /* 计算需要的描述符数量 */
    uint32_t desc_count = 0;
    sg_node_t *src_node = req->src_list;
    sg_node_t *dst_node = req->dst_list;
    uint32_t src_offset = 0, dst_offset = 0;

    while (src_node && dst_node) {
        desc_count++;

        uint32_t src_remain = src_node->len - src_offset;
        uint32_t dst_remain = dst_node->len - dst_offset;
        uint32_t xfer_len = (src_remain < dst_remain) ? src_remain : dst_remain;

        src_offset += xfer_len;
        dst_offset += xfer_len;

        if (src_offset >= src_node->len) {
            src_node = src_node->next;
            src_offset = 0;
        }
        if (dst_offset >= dst_node->len) {
            dst_node = dst_node->next;
            dst_offset = 0;
        }
    }

    if (desc_count > pool_size) {
        return NULL;  /* 描述符池不足 */
    }

    /* 构建描述符链 */
    dma_descriptor_t *first_desc = &pool[0];
    dma_descriptor_t *curr_desc = first_desc;

    src_node = req->src_list;
    dst_node = req->dst_list;
    src_offset = 0;
    dst_offset = 0;
    uint32_t desc_idx = 0;

    while (src_node && dst_node) {
        uint32_t src_remain = src_node->len - src_offset;
        uint32_t dst_remain = dst_node->len - dst_offset;
        uint32_t xfer_len = (src_remain < dst_remain) ? src_remain : dst_remain;

        /* 填充描述符 */
        curr_desc->ctrl = DMA_DESC_CTRL_SRC_INC | DMA_DESC_CTRL_DST_INC |
                          DMA_DESC_CTRL_IRQ_EN;
        curr_desc->xfer_size = xfer_len;
        curr_desc->xfer_type = req->xfer_type;
        curr_desc->status = 0;
        curr_desc->src_addr = src_node->phys_addr + src_offset;
        curr_desc->dst_addr = dst_node->phys_addr + dst_offset;
        curr_desc->user_data = (uint64_t)req;

        /* 更新偏移 */
        src_offset += xfer_len;
        dst_offset += xfer_len;

        if (src_offset >= src_node->len) {
            src_node = src_node->next;
            src_offset = 0;
        }
        if (dst_offset >= dst_node->len) {
            dst_node = dst_node->next;
            dst_offset = 0;
        }

        /* 设置链表指针 */
        if (src_node && dst_node) {
            curr_desc->next_desc_phys = (uint64_t)&pool[desc_idx + 1];
            curr_desc = &pool[++desc_idx];
        } else {
            curr_desc->ctrl |= DMA_DESC_CTRL_EOL;  /* 结束标记 */
            curr_desc->next_desc_phys = 0;
        }
    }

    return first_desc;
}

/* 提交SG传输请求 */
int dma_submit_sg_xfer(dma_channel_t *ch, sg_xfer_request_t *req)
{
    if (!ch || !req) {
        return -1;
    }

    /* 从通道描述符池分配 */
    dma_descriptor_t *desc_chain = dma_build_sg_desc_chain(
        req, ch->desc_pool, ch->desc_pool_size);

    if (!desc_chain) {
        return -2;
    }

    /* 设置回调 */
    ch->complete_cb = dma_sg_complete_handler;
    ch->cb_ctx = req;

    /* 启动链式传输 */
    return dma_start_xfer_chain(ch, desc_chain);
}
