/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 2138
 * Language: c
 * Block ID: 77ee5a8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * Linux DMA Engine API使用示例
 * 适用于嵌入式Linux 5G基带系统
 * ============================================================================ */

#ifdef __KERNEL__

#include <linux/module.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/scatterlist.h>

/* DMA设备上下文 */
struct dma_5g_ctx {
    struct dma_chan *chan;              /* DMA通道 */
    struct device *dev;                 /* 设备指针 */
    dma_addr_t dma_addr;                /* DMA地址 */
    void *virt_addr;                    /* 虚拟地址 */
    size_t buffer_size;
    struct completion cmp;              /* 完成信号 */
    dma_cookie_t cookie;
    int status;
};

/* DMA完成回调 */
static void dma_5g_callback(void *param)
{
    struct dma_5g_ctx *ctx = param;
    ctx->status = dmaengine_tx_status(ctx->chan, ctx->cookie, NULL);
    complete(&ctx->cmp);
}

/* 申请DMA通道 */
static int dma_5g_request_channel(struct dma_5g_ctx *ctx)
{
    dma_cap_mask_t mask;

    dma_cap_zero(mask);
    dma_cap_set(DMA_SLAVE, mask);
    dma_cap_set(DMA_SG, mask);

    /* 请求通道 */
    ctx->chan = dma_request_chan_by_mask(&mask);
    if (IS_ERR(ctx->chan)) {
        pr_err("Failed to request DMA channel\n");
        return PTR_ERR(ctx->chan);
    }

    ctx->dev = ctx->chan->device->dev;
    init_completion(&ctx->cmp);

    return 0;
}

/* 准备分散收集传输 */
static int dma_5g_prepare_sg_xfer(struct dma_5g_ctx *ctx,
                                   struct scatterlist *src_sg,
                                   struct scatterlist *dst_sg,
                                   int src_nents, int dst_nents)
{
    struct dma_async_tx_descriptor *desc;
    enum dma_ctrl_flags flags;

    flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;

    /* 准备SG传输 */
    desc = dmaengine_prep_dma_sg(ctx->chan, dst_sg, dst_nents,
                                  src_sg, src_nents,
                                  DMA_MEM_TO_MEM | flags);
    if (!desc) {
        pr_err("Failed to prepare SG transfer\n");
        return -1;
    }

    /* 设置回调 */
    desc->callback = dma_5g_callback;
    desc->callback_param = ctx;

    /* 提交传输 */
    ctx->cookie = desc->tx_submit(desc);
    if (dma_submit_error(ctx->cookie)) {
        pr_err("Failed to submit DMA transfer\n");
        return -1;
    }

    /* 启动传输 */
    dma_async_issue_pending(ctx->chan);

    return 0;
}

/* 等待传输完成 */
static int dma_5g_wait_completion(struct dma_5g_ctx *ctx,
                                   unsigned long timeout_ms)
{
    unsigned long tmo = msecs_to_jiffies(timeout_ms);

    tmo = wait_for_completion_timeout(&ctx->cmp, tmo);

    if (tmo == 0) {
        pr_err("DMA transfer timeout\n");
        dmaengine_terminate_sync(ctx->chan);
        return -ETIMEDOUT;
    }

    return (ctx->status == DMA_COMPLETE) ? 0 : -EIO;
}

/* 示例: 5G基带数据块传输 */
static int dma_5g_xfer_codeblock(struct dma_5g_ctx *ctx,
                                  void *src_data, void *dst_data,
                                  size_t len)
{
    struct scatterlist src_sg, dst_sg;
    int ret;

    /* 映射源内存 */
    sg_init_one(&src_sg, src_data, len);
    ret = dma_map_sg(ctx->dev, &src_sg, 1, DMA_TO_DEVICE);
    if (ret != 1) {
        pr_err("Failed to map source SG\n");
        return -1;
    }

    /* 映射目的内存 */
    sg_init_one(&dst_sg, dst_data, len);
    ret = dma_map_sg(ctx->dev, &dst_sg, 1, DMA_FROM_DEVICE);
    if (ret != 1) {
        pr_err("Failed to map dest SG\n");
        dma_unmap_sg(ctx->dev, &src_sg, 1, DMA_TO_DEVICE);
        return -1;
    }

    /* 准备并启动传输 */
    ret = dma_5g_prepare_sg_xfer(ctx, &src_sg, &dst_sg, 1, 1);
    if (ret != 0) {
        goto unmap;
    }

    /* 等待完成 */
    ret = dma_5g_wait_completion(ctx, 1000);

unmap:
    dma_unmap_sg(ctx->dev, &dst_sg, 1, DMA_FROM_DEVICE);
    dma_unmap_sg(ctx->dev, &src_sg, 1, DMA_TO_DEVICE);

    return ret;
}

/* 模块初始化 */
static int __init dma_5g_init(void)
{
    struct dma_5g_ctx ctx;
    int ret;

    pr_info("5G DMA Engine Driver Init\n");

    ret = dma_5g_request_channel(&ctx);
    if (ret != 0) {
        return ret;
    }

    /* 测试传输 */
    char src_buf[4096], dst_buf[4096];
    memset(src_buf, 0xAB, sizeof(src_buf));

    ret = dma_5g_xfer_codeblock(&ctx, src_buf, dst_buf, sizeof(src_buf));
    if (ret == 0) {
        pr_info("DMA transfer successful\n");
    }

    dma_release_channel(ctx.chan);
    return 0;
}

static void __exit dma_5g_exit(void)
{
    pr_info("5G DMA Engine Driver Exit\n");
}

module_init(dma_5g_init);
module_exit(dma_5g_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("5G Baseband DMA Engine Driver");
MODULE_AUTHOR("5G Baseband Team");

#endif /* __KERNEL__ */
