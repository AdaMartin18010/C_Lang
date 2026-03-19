/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 1862
 * Language: c
 * Block ID: 2d013982
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 5G NR LDPC解码器DMA驱动
 * 支持码块分段和并行解码
 * ============================================================================ */

#include <stdlib.h>
#include <string.h>

/* 5G NR LDPC参数 (3GPP 38.212) */
#define LDPC_MAX_CODEBLOCK_SIZE 8448    /* 最大码块大小 */
#define LDPC_MIN_CODEBLOCK_SIZE 40      /* 最小码块大小 */
#define LDPC_BASE_GRAPH_1       1       /* BG1: K=22, Z=384 */
#define LDPC_BASE_GRAPH_2       2       /* BG2: K=10, Z=384 */

/* LDPC码块信息 */
typedef struct ldpc_codeblock {
    uint32_t cb_index;                  /* 码块索引 */
    uint32_t tb_index;                  /* 传输块索引 */
    uint16_t K;                         /* 信息比特数 */
    uint16_t N;                         /* 码块大小 */
    uint8_t bg_type;                    /* Base Graph类型 */
    uint8_t rv_idx;                     /* Redundancy Version */
    uint8_t qm;                         /* 调制阶数 */
    uint16_t num_layers;                /* 层数 */
} ldpc_codeblock_t;

/* LDPC DMA请求 */
typedef struct ldpc_dma_request {
    ldpc_codeblock_t cb_info;           /* 码块信息 */
    void *llr_buffer;                   /* LLR缓冲区 */
    void *output_buffer;                /* 输出缓冲区 */
    uint64_t llr_phys;                  /* LLR物理地址 */
    uint64_t output_phys;               /* 输出物理地址 */
    uint32_t llr_len;                   /* LLR长度 */
    uint32_t max_iterations;            /* 最大迭代次数 */

    /* 完成回调 */
    void (*complete_cb)(struct ldpc_dma_request *, int status,
                        uint32_t iterations);
    void *cb_ctx;
} ldpc_dma_request_t;

/* LDPC DMA上下文 */
typedef struct ldpc_dma_context {
    dma_channel_t *input_ch;            /* 输入通道 */
    dma_channel_t *output_ch;           /* 输出通道 */
    dma_channel_t *ctrl_ch;             /* 控制通道 */
    desc_pool_t desc_pool;              /* 描述符池 */

    /* 码块队列 */
    ldpc_dma_request_t *pending_queue[32];
    atomic_uint queue_head;
    atomic_uint queue_tail;

    /* 统计 */
    atomic_ulong cb_processed;          /* 处理码块数 */
    atomic_ulong cb_errors;             /* 错误码块数 */
    atomic_uint32_t total_iterations;   /* 总迭代次数 */
} ldpc_dma_context_t;

static ldpc_dma_context_t g_ldpc_ctx;

/* 初始化LDPC DMA上下文 */
int ldpc_dma_init(void)
{
    memset(&g_ldpc_ctx, 0, sizeof(g_ldpc_ctx));

    /* 申请专用通道 */
    g_ldpc_ctx.input_ch = dma_channel_alloc(7);  /* 最高优先级 */
    g_ldpc_ctx.output_ch = dma_channel_alloc(6);
    g_ldpc_ctx.ctrl_ch = dma_channel_alloc(5);

    if (!g_ldpc_ctx.input_ch || !g_ldpc_ctx.output_ch ||
        !g_ldpc_ctx.ctrl_ch) {
        return -1;
    }

    /* 初始化描述符池 */
    if (desc_pool_init(&g_ldpc_ctx.desc_pool, 256) != 0) {
        return -1;
    }

    /* 配置通道 */
    dma_ch_config_t cfg = {
        .burst_size = 16,
        .data_width = DMA_WIDTH_128BIT,
        .irq_mask = CH_IRQ_DESC_DONE | CH_IRQ_CHAIN_DONE | CH_IRQ_ERROR
    };

    /* 应用配置 */
    REG_WRITE32(DMA_CH_IRQ_MASK(g_ldpc_ctx.input_ch->id), cfg.irq_mask);
    REG_WRITE32(DMA_CH_IRQ_MASK(g_ldpc_ctx.output_ch->id), cfg.irq_mask);

    return 0;
}

/* 准备LDPC解码DMA传输 */
int ldpc_dma_submit_decode(ldpc_dma_request_t *req)
{
    if (!req || !req->llr_buffer || !req->output_buffer) {
        return -1;
    }

    ldpc_dma_context_t *ctx = &g_ldpc_ctx;

    /* 申请描述符 */
    dma_descriptor_t *input_desc = desc_alloc(&ctx->desc_pool);
    dma_descriptor_t *output_desc = desc_alloc(&ctx->desc_pool);
    dma_descriptor_t *ctrl_desc = desc_alloc(&ctx->desc_pool);

    if (!input_desc || !output_desc || !ctrl_desc) {
        goto cleanup;
    }

    /* 配置输入描述符 (LLR -> LDPC解码器) */
    input_desc->ctrl = DMA_DESC_CTRL_SRC_INC | DMA_DESC_CTRL_DST_FIXED |
                       DMA_DESC_CTRL_IRQ_EN;
    input_desc->xfer_size = req->llr_len;
    input_desc->xfer_type = DMA_XFER_LDPC_DEC_INPUT;
    input_desc->src_addr = req->llr_phys;
    input_desc->dst_addr = LDPC_DECODER_BASE_ADDR + LDPC_REG_LLR_IN;
    input_desc->cb_index = req->cb_info.cb_index;
    input_desc->rv_index = req->cb_info.rv_idx;
    input_desc->harq_pid = req->cb_info.tb_index;
    input_desc->next_desc_phys = (uint64_t)ctrl_desc;
    input_desc->user_data = (uint64_t)req;

    /* 配置控制描述符 (设置解码参数) */
    ctrl_desc->ctrl = DMA_DESC_CTRL_SRC_INC | DMA_DESC_CTRL_DST_FIXED;
    ctrl_desc->xfer_size = sizeof(ldpc_codeblock_t);
    ctrl_desc->xfer_type = DMA_XFER_MEM_TO_PERIPH;
    ctrl_desc->src_addr = (uint64_t)&req->cb_info;
    ctrl_desc->dst_addr = LDPC_DECODER_BASE_ADDR + LDPC_REG_CTRL;
    ctrl_desc->next_desc_phys = (uint64_t)output_desc;

    /* 配置输出描述符 (LDPC解码器 -> 输出) */
    uint32_t output_size = req->cb_info.K / 8;  /* 字节对齐 */
    output_desc->ctrl = DMA_DESC_CTRL_SRC_FIXED | DMA_DESC_CTRL_DST_INC |
                        DMA_DESC_CTRL_IRQ_EN | DMA_DESC_CTRL_EOL;
    output_desc->xfer_size = output_size;
    output_desc->xfer_type = DMA_XFER_LDPC_DEC_OUTPUT;
    output_desc->src_addr = LDPC_DECODER_BASE_ADDR + LDPC_REG_DEC_OUT;
    output_desc->dst_addr = req->output_phys;
    output_desc->next_desc_phys = 0;
    output_desc->user_data = (uint64_t)req;

    /* 清理缓存 */
    cache_clean_range(req->llr_buffer, req->llr_len);
    cache_clean_range(&req->cb_info, sizeof(ldpc_codeblock_t));
    cache_invalidate_range(req->output_buffer, output_size);

    /* 启动输入传输 */
    int ret = dma_start_xfer_chain(ctx->input_ch, input_desc);
    if (ret != 0) {
        goto cleanup;
    }

    /* 设置完成回调 */
    ctx->input_ch->complete_cb = ldpc_input_complete;
    ctx->output_ch->complete_cb = ldpc_output_complete;
    ctx->input_ch->cb_ctx = req;
    ctx->output_ch->cb_ctx = req;

    return 0;

cleanup:
    if (input_desc) desc_free(&ctx->desc_pool, input_desc);
    if (output_desc) desc_free(&ctx->desc_pool, output_desc);
    if (ctrl_desc) desc_free(&ctx->desc_pool, ctrl_desc);
    return -1;
}

/* 输入传输完成处理 */
static void ldpc_input_complete(dma_channel_t *ch, dma_descriptor_t *desc,
                                 void *ctx)
{
    (void)ch;
    ldpc_dma_request_t *req = (ldpc_dma_request_t *)ctx;

    DMA_DBG(DMA_DBG_INFO, "LDPC input complete for CB %u",
            req->cb_info.cb_index);

    /* LDPC解码器自动开始处理，无需额外操作 */
}

/* 输出传输完成处理 */
static void ldpc_output_complete(dma_channel_t *ch, dma_descriptor_t *desc,
                                  void *ctx)
{
    (void)ch;
    ldpc_dma_request_t *req = (ldpc_dma_request_t *)ctx;
    ldpc_dma_context_t *ldpc_ctx = &g_ldpc_ctx;

    /* 读取实际迭代次数 */
    uint32_t iterations = REG_READ32(LDPC_DECODER_BASE_ADDR +
                                      LDPC_REG_ITER_CNT);

    /* 检查解码状态 */
    uint32_t status = REG_READ32(LDPC_DECODER_BASE_ADDR +
                                  LDPC_REG_STATUS);
    int decode_status = (status & LDPC_STATUS_SUCCESS) ? 0 : -1;

    /* 更新统计 */
    atomic_fetch_add_explicit(&ldpc_ctx->cb_processed, 1,
                              memory_order_relaxed);
    atomic_fetch_add_explicit(&ldpc_ctx->total_iterations, iterations,
                              memory_order_relaxed);

    if (decode_status != 0) {
        atomic_fetch_add_explicit(&ldpc_ctx->cb_errors, 1,
                                  memory_order_relaxed);
    }

    /* 调用用户回调 */
    if (req->complete_cb) {
        req->complete_cb(req, decode_status, iterations);
    }

    /* 释放描述符 */
    /* 简化处理：实际应该跟踪所有描述符 */
}

/* 批量LDPC解码 */
int ldpc_dma_submit_batch(ldpc_dma_request_t **reqs, uint32_t count)
{
    if (!reqs || count == 0 || count > 32) {
        return -1;
    }

    /* 构建批量描述符链 */
    for (uint32_t i = 0; i < count; i++) {
        int ret = ldpc_dma_submit_decode(reqs[i]);
        if (ret != 0) {
            /* 回滚已提交的 */
            return -1;
        }
    }

    return 0;
}

/* 获取LDPC解码统计 */
void ldpc_dma_get_stats(uint64_t *cb_processed, uint64_t *cb_errors,
                        uint32_t *avg_iterations)
{
    uint64_t processed = atomic_load_explicit(&g_ldpc_ctx.cb_processed,
                                               memory_order_relaxed);
    uint64_t errors = atomic_load_explicit(&g_ldpc_ctx.cb_errors,
                                            memory_order_relaxed);
    uint32_t iterations = atomic_load_explicit(&g_ldpc_ctx.total_iterations,
                                                memory_order_relaxed);

    if (cb_processed) *cb_processed = processed;
    if (cb_errors) *cb_errors = errors;
    if (avg_iterations && processed > 0) {
        *avg_iterations = iterations / processed;
    }
}
