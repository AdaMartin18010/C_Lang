/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 409
 * Language: c
 * Block ID: 2ed642e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * DMA通道管理器 - 支持5G基带多通道并发
 * ============================================================================ */

#define DMA_MAX_CHANNELS        32
#define DMA_MAX_BUFS_PER_CH     256

/* DMA通道状态 */
typedef enum {
    DMA_CH_STATE_IDLE = 0,              /* 空闲 */
    DMA_CH_STATE_CONFIG,                /* 配置中 */
    DMA_CH_STATE_ACTIVE,                /* 传输中 */
    DMA_CH_STATE_PAUSED,                /* 已暂停 */
    DMA_CH_STATE_ERROR                  /* 错误状态 */
} dma_ch_state_t;

/* DMA通道配置 */
typedef struct dma_ch_config {
    uint32_t channel_id;                /* 通道ID */
    uint32_t priority;                  /* 优先级 0-7 */
    bool chain_mode;                    /* 链模式 */
    bool circular_mode;                 /* 环形模式 */
    uint32_t burst_size;                /* 突发传输大小 */
    dma_data_width_t data_width;        /* 数据宽度 */
    uint32_t irq_mask;                  /* 中断掩码 */
} dma_ch_config_t;

/* DMA通道上下文 */
typedef struct dma_channel {
    uint32_t id;                        /* 通道ID */
    dma_ch_state_t state;               /* 当前状态 */
    atomic_flag in_use;                 /* 使用标记 */

    /* 描述符管理 */
    dma_ring_buffer_t ring;             /* 描述符环形缓冲 */
    dma_descriptor_t *desc_pool;        /* 描述符池 */
    uint32_t desc_pool_size;            /* 池大小 */

    /* 回调管理 */
    void (*complete_cb)(struct dma_channel *, dma_descriptor_t *, void *);
    void (*error_cb)(struct dma_channel *, uint32_t, void *);
    void *cb_ctx;                       /* 回调上下文 */

    /* 统计 */
    atomic_ulong xfer_bytes;            /* 传输字节数 */
    atomic_ulong xfer_count;            /* 传输次数 */
    atomic_uint error_count;            /* 错误计数 */
} dma_channel_t;

/* DMA控制器上下文 */
typedef struct dma_controller {
    uint32_t base_addr;                 /* 寄存器基地址 */
    uint32_t num_channels;              /* 通道数量 */
    dma_channel_t channels[DMA_MAX_CHANNELS];
    atomic_uint active_channels;        /* 活跃通道数 */

    /* 全局锁 */
    atomic_flag global_lock;
} dma_controller_t;

/* 全局DMA控制器实例 */
static dma_controller_t g_dma_ctrl;

/* 初始化DMA控制器 */
int dma_controller_init(uint32_t base_addr, uint32_t num_channels)
{
    if (!base_addr || num_channels > DMA_MAX_CHANNELS) {
        return -1;
    }

    g_dma_ctrl.base_addr = base_addr;
    g_dma_ctrl.num_channels = num_channels;
    atomic_init(&g_dma_ctrl.active_channels, 0);
    atomic_flag_clear(&g_dma_ctrl.global_lock);

    /* 复位DMA控制器 */
    REG_WRITE32(DMA_GLB_CTRL, 0);
    DMA_MB();

    /* 初始化所有通道 */
    for (uint32_t i = 0; i < num_channels; i++) {
        dma_channel_t *ch = &g_dma_ctrl.channels[i];
        ch->id = i;
        ch->state = DMA_CH_STATE_IDLE;
        atomic_flag_clear(&ch->in_use);
        atomic_init(&ch->xfer_bytes, 0);
        atomic_init(&ch->xfer_count, 0);
        atomic_init(&ch->error_count, 0);

        /* 复位通道 */
        REG_WRITE32(DMA_CH_CTRL(i), CH_CTRL_RESET);
        DMA_MB();
        REG_WRITE32(DMA_CH_CTRL(i), 0);

        /* 清除中断状态 */
        REG_WRITE32(DMA_CH_STATUS(i), 0xFFFFFFFF);
    }

    /* 使能全局DMA */
    REG_WRITE32(DMA_GLB_CTRL, 1);
    DMA_MB();

    return 0;
}

/* 申请DMA通道 */
dma_channel_t *dma_channel_alloc(uint32_t priority)
{
    for (uint32_t i = 0; i < g_dma_ctrl.num_channels; i++) {
        dma_channel_t *ch = &g_dma_ctrl.channels[i];

        if (!atomic_flag_test_and_set(&ch->in_use)) {
            /* 成功获取通道 */
            ch->state = DMA_CH_STATE_CONFIG;

            /* 配置优先级 */
            uint32_t ctrl = (priority << CH_CTRL_PRIO_SHIFT) & CH_CTRL_PRIO_MASK;
            REG_WRITE32(DMA_CH_CTRL(ch->id), ctrl);
            DMA_MB();

            return ch;
        }
    }
    return NULL;  /* 无可用通道 */
}

/* 释放DMA通道 */
void dma_channel_free(dma_channel_t *ch)
{
    if (!ch) return;

    /* 停止通道 */
    REG_WRITE32(DMA_CH_CTRL(ch->id), CH_CTRL_RESET);
    DMA_MB();

    ch->state = DMA_CH_STATE_IDLE;
    ch->complete_cb = NULL;
    ch->error_cb = NULL;
    ch->cb_ctx = NULL;

    atomic_flag_clear(&ch->in_use);
}
