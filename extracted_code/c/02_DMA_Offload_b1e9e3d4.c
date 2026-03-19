/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 560
 * Language: c
 * Block ID: b1e9e3d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * DMA寄存器级操作 - 底层硬件控制
 * ============================================================================ */

/* 启动DMA传输 (单描述符模式) */
int dma_start_xfer_single(dma_channel_t *ch, dma_descriptor_t *desc)
{
    if (!ch || !desc || ch->state != DMA_CH_STATE_CONFIG) {
        return -1;
    }

    /* 确保描述符写入内存 */
    DMA_WMB();

    /* 设置描述符地址 */
    REG_WRITE64(DMA_CH_DESC_ADDR(ch->id), (uint64_t)desc);
    DMA_MB();

    /* 使能通道 */
    uint32_t ctrl = REG_READ32(DMA_CH_CTRL(ch->id));
    ctrl |= CH_CTRL_ENABLE;
    REG_WRITE32(DMA_CH_CTRL(ch->id), ctrl);
    DMA_MB();

    ch->state = DMA_CH_STATE_ACTIVE;
    atomic_fetch_add_explicit(&g_dma_ctrl.active_channels, 1,
                              memory_order_relaxed);

    return 0;
}

/* 启动DMA传输 (链模式) */
int dma_start_xfer_chain(dma_channel_t *ch, dma_descriptor_t *first_desc)
{
    if (!ch || !first_desc || ch->state != DMA_CH_STATE_CONFIG) {
        return -1;
    }

    /* 确保描述符链表写入内存 */
    DMA_WMB();

    /* 设置描述符地址 */
    REG_WRITE64(DMA_CH_DESC_ADDR(ch->id), (uint64_t)first_desc);

    /* 配置链模式 */
    uint32_t ctrl = REG_READ32(DMA_CH_CTRL(ch->id));
    ctrl |= CH_CTRL_CHAIN_MODE | CH_CTRL_ENABLE;
    REG_WRITE32(DMA_CH_CTRL(ch->id), ctrl);
    DMA_MB();

    ch->state = DMA_CH_STATE_ACTIVE;
    atomic_fetch_add_explicit(&g_dma_ctrl.active_channels, 1,
                              memory_order_relaxed);

    return 0;
}

/* 暂停DMA通道 */
int dma_channel_pause(dma_channel_t *ch)
{
    if (!ch || ch->state != DMA_CH_STATE_ACTIVE) {
        return -1;
    }

    uint32_t ctrl = REG_READ32(DMA_CH_CTRL(ch->id));
    ctrl |= CH_CTRL_PAUSE;
    REG_WRITE32(DMA_CH_CTRL(ch->id), ctrl);
    DMA_MB();

    /* 等待传输暂停 */
    int timeout = 1000;
    while ((REG_READ32(DMA_CH_STATUS(ch->id)) & CH_STAT_ACTIVE) && timeout-- > 0) {
        __asm__ __volatile__("nop");
    }

    ch->state = DMA_CH_STATE_PAUSED;
    return (timeout > 0) ? 0 : -1;
}

/* 恢复DMA通道 */
int dma_channel_resume(dma_channel_t *ch)
{
    if (!ch || ch->state != DMA_CH_STATE_PAUSED) {
        return -1;
    }

    uint32_t ctrl = REG_READ32(DMA_CH_CTRL(ch->id));
    ctrl &= ~CH_CTRL_PAUSE;
    REG_WRITE32(DMA_CH_CTRL(ch->id), ctrl);
    DMA_MB();

    ch->state = DMA_CH_STATE_ACTIVE;
    return 0;
}

/* 停止DMA通道 */
void dma_channel_stop(dma_channel_t *ch)
{
    if (!ch) return;

    /* 禁用通道 */
    REG_WRITE32(DMA_CH_CTRL(ch->id), 0);
    DMA_MB();

    /* 等待停止 */
    int timeout = 1000;
    while ((REG_READ32(DMA_CH_STATUS(ch->id)) & CH_STAT_ACTIVE) && timeout-- > 0) {
        __asm__ __volatile__("nop");
    }

    ch->state = DMA_CH_STATE_IDLE;
    atomic_fetch_sub_explicit(&g_dma_ctrl.active_channels, 1,
                              memory_order_relaxed);
}

/* 读取通道状态 */
uint32_t dma_channel_get_status(dma_channel_t *ch)
{
    if (!ch) return 0;
    return REG_READ32(DMA_CH_STATUS(ch->id));
}

/* 清除中断状态 */
void dma_channel_clear_irq(dma_channel_t *ch, uint32_t irq_mask)
{
    if (!ch) return;
    REG_WRITE32(DMA_CH_STATUS(ch->id), irq_mask);
    DMA_MB();
}
