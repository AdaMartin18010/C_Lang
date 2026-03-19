/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 1014
 * Language: c
 * Block ID: f563bf90
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * DMA中断处理 - 支持MSI和传统IRQ
 * ============================================================================ */

#include <signal.h>

/* 中断上下文 */
typedef struct dma_irq_ctx {
    uint32_t ch_id;                     /* 通道ID */
    uint32_t status;                    /* 中断状态 */
    uint64_t timestamp;                 /* 时间戳 */
} dma_irq_ctx_t;

/* 中断处理统计 */
typedef struct irq_stats {
    atomic_ulong irq_count;             /* 中断计数 */
    atomic_ulong desc_done_count;       /* 描述符完成计数 */
    atomic_ulong chain_done_count;      /* 链表完成计数 */
    atomic_ulong error_count;           /* 错误计数 */
    atomic_ulong spurious_count;        /* 伪中断计数 */
} irq_stats_t;

static irq_stats_t g_irq_stats;

/* 获取当前时间戳 (纳秒) */
static inline uint64_t get_timestamp_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

/* 单个通道中断处理 */
static void dma_handle_channel_irq(uint32_t ch_id)
{
    dma_channel_t *ch = &g_dma_ctrl.channels[ch_id];
    uint32_t status = REG_READ32(DMA_CH_STATUS(ch_id));

    if (status == 0) {
        atomic_fetch_add_explicit(&g_irq_stats.spurious_count, 1,
                                  memory_order_relaxed);
        return;
    }

    atomic_fetch_add_explicit(&g_irq_stats.irq_count, 1,
                              memory_order_relaxed);

    /* 描述符完成处理 */
    if (status & CH_STAT_DESC_DONE) {
        atomic_fetch_add_explicit(&g_irq_stats.desc_done_count, 1,
                                  memory_order_relaxed);

        /* 读取当前描述符 */
        uint64_t curr_desc_phys = REG_READ64(DMA_CH_CURR_DESC(ch_id));

        if (ch->complete_cb && curr_desc_phys != 0) {
            /* 将物理地址映射到虚拟地址 (简化处理) */
            dma_descriptor_t *desc = phys_to_virt(curr_desc_phys);
            if (desc) {
                ch->complete_cb(ch, desc, ch->cb_ctx);
            }
        }
    }

    /* 链表完成处理 */
    if (status & CH_STAT_CHAIN_DONE) {
        atomic_fetch_add_explicit(&g_irq_stats.chain_done_count, 1,
                                  memory_order_relaxed);

        ch->state = DMA_CH_STATE_IDLE;
        atomic_fetch_sub_explicit(&g_dma_ctrl.active_channels, 1,
                                  memory_order_relaxed);
    }

    /* 错误处理 */
    if (status & CH_STAT_ERROR) {
        atomic_fetch_add_explicit(&g_irq_stats.error_count, 1,
                                  memory_order_relaxed);
        atomic_fetch_add_explicit(&ch->error_count, 1,
                                  memory_order_relaxed);

        uint32_t error_type = status & (DMA_DESC_STAT_CRC_ERR |
                                         DMA_DESC_STAT_AXI_ERR);

        if (ch->error_cb) {
            ch->error_cb(ch, error_type, ch->cb_ctx);
        }

        ch->state = DMA_CH_STATE_ERROR;
    }

    /* 清除中断状态 */
    REG_WRITE32(DMA_CH_STATUS(ch_id), status);
    DMA_MB();
}

/* 全局中断处理入口 */
void dma_irq_handler(int sig, siginfo_t *info, void *context)
{
    (void)sig; (void)context;

    /* 读取全局中断状态 */
    uint32_t global_status = REG_READ32(DMA_GLB_IRQ_STATUS);

    if (global_status == 0) {
        return;  /* 无有效中断 */
    }

    /* 轮询所有通道 */
    for (uint32_t i = 0; i < g_dma_ctrl.num_channels; i++) {
        if (global_status & (1U << i)) {
            dma_handle_channel_irq(i);
        }
    }

    /* 清除全局中断 */
    REG_WRITE32(DMA_GLB_IRQ_STATUS, global_status);
}

/* SG传输完成处理 */
void dma_sg_complete_handler(dma_channel_t *ch, dma_descriptor_t *desc,
                              void *ctx)
{
    sg_xfer_request_t *req = (sg_xfer_request_t *)ctx;

    if (!req) return;

    /* 更新统计 */
    atomic_fetch_add_explicit(&ch->xfer_bytes, desc->xfer_size,
                              memory_order_relaxed);
    atomic_fetch_add_explicit(&ch->xfer_count, 1,
                              memory_order_relaxed);

    /* 检查是否是最后一个描述符 */
    if (desc->ctrl & DMA_DESC_CTRL_EOL) {
        /* 传输完成回调 */
        if (req->complete_cb) {
            req->complete_cb(req, 0);  /* 成功 */
        }
    }
}
