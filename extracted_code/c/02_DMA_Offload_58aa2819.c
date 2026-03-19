/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 1400
 * Language: c
 * Block ID: 58aa2819
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 内存屏障使用模式 - 确保DMA操作顺序
 * ============================================================================ */

/* 模式1: 描述符写入 -> 启动DMA */
void pattern_desc_to_dma(dma_descriptor_t *desc, dma_channel_t *ch)
{
    /* 1. 写入描述符内容 */
    desc->src_addr = 0x10000000;
    desc->dst_addr = 0x20000000;
    desc->xfer_size = 4096;
    desc->ctrl = DMA_DESC_CTRL_SRC_INC | DMA_DESC_CTRL_DST_INC;

    /* 2. 写内存屏障 - 确保描述符写入完成 */
    DMA_WMB();

    /* 3. 启动DMA */
    REG_WRITE64(DMA_CH_DESC_ADDR(ch->id), (uint64_t)desc);
    DMA_MB();  /* 确保寄存器写入完成 */

    REG_WRITE32(DMA_CH_CTRL(ch->id), CH_CTRL_ENABLE);
}

/* 模式2: DMA完成 -> CPU读取数据 */
int pattern_dma_to_cpu(dma_channel_t *ch, void *buffer, size_t len)
{
    /* 1. 等待DMA完成 */
    while (ch->state == DMA_CH_STATE_ACTIVE) {
        __asm__ __volatile__("yield");
    }

    /* 2. 读内存屏障 - 确保看到最新的状态 */
    DMA_RMB();

    /* 3. 失效缓存 */
    cache_invalidate_range(buffer, len);

    /* 4. 读取数据 */
    return (ch->state == DMA_CH_STATE_IDLE) ? 0 : -1;
}

/* 模式3: 多描述符链表更新 */
void pattern_chain_update(dma_descriptor_t *desc, dma_descriptor_t *next)
{
    /* 1. 准备下一个描述符 */
    next->src_addr = 0x30000000;
    next->dst_addr = 0x40000000;
    next->xfer_size = 8192;
    next->ctrl = DMA_DESC_CTRL_SRC_INC | DMA_DESC_CTRL_DST_INC |
                 DMA_DESC_CTRL_EOL;
    next->next_desc_phys = 0;

    /* 2. 写屏障 */
    DMA_WMB();

    /* 3. 更新当前描述符的next指针 */
    desc->ctrl &= ~DMA_DESC_CTRL_EOL;  /* 清除结束标记 */
    desc->next_desc_phys = (uint64_t)next;

    /* 4. 写屏障 - 确保链表更新完成 */
    DMA_WMB();
}

/* 模式4: 环形缓冲区同步 */
void pattern_ring_producer(dma_ring_buffer_t *ring, dma_descriptor_t *desc)
{
    /* 1. 写入描述符 */
    uint32_t idx = atomic_load_explicit(&ring->producer_idx,
                                         memory_order_relaxed)
                   & DMA_RING_MASK;
    ring->desc_array[idx] = *desc;

    /* 2. 写屏障 */
    atomic_thread_fence(memory_order_release);

    /* 3. 更新生产者索引 */
    atomic_fetch_add_explicit(&ring->producer_idx, 1,
                              memory_order_release);
}

bool pattern_ring_consumer(dma_ring_buffer_t *ring, dma_descriptor_t *desc)
{
    /* 1. 读取生产者索引 */
    uint32_t prod = atomic_load_explicit(&ring->producer_idx,
                                          memory_order_acquire);
    uint32_t cons = atomic_load_explicit(&ring->consumer_idx,
                                          memory_order_relaxed);

    if (prod == cons) {
        return false;  /* 空 */
    }

    /* 2. 读屏障已在acquire中完成 */

    /* 3. 读取描述符 */
    uint32_t idx = cons & DMA_RING_MASK;
    *desc = ring->desc_array[idx];

    /* 4. 更新消费者索引 */
    atomic_fetch_add_explicit(&ring->consumer_idx, 1,
                              memory_order_release);

    return true;
}
