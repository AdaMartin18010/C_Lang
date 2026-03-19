/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\01_DMA_Controller.md
 * Line: 1195
 * Language: c
 * Block ID: f4772d7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux DMA引擎API完整示例
 */

#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>

struct my_dma_ctx {
    struct dma_chan *chan;
    dma_addr_t src_dma;
    dma_addr_t dst_dma;
    void *src_cpu;
    void *dst_cpu;
    struct completion cmp;
};

// 1. 申请DMA通道
static int request_dma_channel(struct my_dma_ctx *ctx)
{
    dma_cap_mask_t mask;
    struct dma_chan *chan;

    // 设置能力掩码
    dma_cap_zero(mask);
    dma_cap_set(DMA_MEMCPY, mask);  // 内存拷贝能力
    // dma_cap_set(DMA_SG, mask);    // SG能力
    // dma_cap_set(DMA_CYCLIC, mask); // 循环传输

    // 申请通道
    chan = dma_request_channel(mask, NULL, NULL);
    if (!chan) {
        pr_err("Failed to request DMA channel\n");
        return -ENODEV;
    }

    ctx->chan = chan;
    pr_info("Got DMA channel: %s-%s\n",
            dma_chan_name(chan), dma_chan_id(chan));

    return 0;
}

// 2. 准备缓冲区
static int prepare_dma_buffers(struct my_dma_ctx *ctx, size_t size)
{
    struct device *dev = ctx->chan->device->dev;

    // 分配一致性DMA缓冲区
    ctx->src_cpu = dma_alloc_coherent(dev, size, &ctx->src_dma, GFP_KERNEL);
    if (!ctx->src_cpu)
        return -ENOMEM;

    ctx->dst_cpu = dma_alloc_coherent(dev, size, &ctx->dst_dma, GFP_KERNEL);
    if (!ctx->dst_cpu) {
        dma_free_coherent(dev, size, ctx->src_cpu, ctx->src_dma);
        return -ENOMEM;
    }

    // 填充源数据
    memset(ctx->src_cpu, 0xAA, size);

    return 0;
}

// 3. 完成回调
static void dma_transfer_complete(void *param)
{
    struct my_dma_ctx *ctx = param;

    pr_info("DMA transfer completed\n");
    complete(&ctx->cmp);
}

// 4. 执行DMA传输
static int do_dma_transfer(struct my_dma_ctx *ctx, size_t len)
{
    struct dma_async_tx_descriptor *desc;
    dma_cookie_t cookie;

    init_completion(&ctx->cmp);

    // 准备DMA传输描述符
    desc = dmaengine_prep_dma_memcpy(ctx->chan, ctx->dst_dma, ctx->src_dma,
                                      len, DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
    if (!desc) {
        pr_err("Failed to prepare DMA memcpy\n");
        return -EINVAL;
    }

    // 设置完成回调
    desc->callback = dma_transfer_complete;
    desc->callback_param = ctx;

    // 提交传输
    cookie = desc->tx_submit(desc);
    if (dma_submit_error(cookie)) {
        pr_err("Failed to submit DMA\n");
        return -EINVAL;
    }

    // 启动传输
    dma_async_issue_pending(ctx->chan);

    // 等待完成 (或使用异步通知)
    if (!wait_for_completion_timeout(&ctx->cmp, HZ)) {
        pr_err("DMA transfer timeout\n");
        return -ETIMEDOUT;
    }

    // 验证结果
    if (memcmp(ctx->src_cpu, ctx->dst_cpu, len) != 0) {
        pr_err("DMA data mismatch!\n");
        return -EIO;
    }

    pr_info("DMA transfer successful: %zu bytes\n", len);
    return 0;
}

// 5. SG传输示例
static int do_dma_sg_transfer(struct my_dma_ctx *ctx,
                               struct scatterlist *sgl, int sg_len)
{
    struct dma_async_tx_descriptor *desc;
    struct device *dev = ctx->chan->device->dev;
    dma_cookie_t cookie;

    // 映射SG列表
    sg_len = dma_map_sg(dev, sgl, sg_len, DMA_TO_DEVICE);
    if (!sg_len)
        return -ENOMEM;

    // 准备SG传输
    desc = dmaengine_prep_slave_sg(ctx->chan, sgl, sg_len,
                                    DMA_MEM_TO_DEV,
                                    DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
    if (!desc) {
        dma_unmap_sg(dev, sgl, sg_len, DMA_TO_DEVICE);
        return -EINVAL;
    }

    // 提交并启动
    cookie = desc->tx_submit(desc);
    dma_async_issue_pending(ctx->chan);

    return 0;
}

// 6. 循环传输示例 (音频等)
static int setup_dma_cyclic(struct my_dma_ctx *ctx, dma_addr_t buf,
                             size_t buf_len, size_t period_len)
{
    struct dma_async_tx_descriptor *desc;

    // 准备循环传输
    desc = dmaengine_prep_dma_cyclic(ctx->chan, buf, buf_len, period_len,
                                      DMA_DEV_TO_MEM,
                                      DMA_PREP_INTERRUPT);
    if (!desc)
        return -EINVAL;

    // 每周期回调
    desc->callback = dma_period_complete;
    desc->callback_param = ctx;

    desc->tx_submit(desc);
    dma_async_issue_pending(ctx->chan);

    return 0;
}

// 7. 释放资源
static void release_dma_resources(struct my_dma_ctx *ctx, size_t size)
{
    struct device *dev = ctx->chan->device->dev;

    // 终止正在进行的传输
    dmaengine_terminate_sync(ctx->chan);

    // 释放缓冲区
    if (ctx->src_cpu)
        dma_free_coherent(dev, size, ctx->src_cpu, ctx->src_dma);
    if (ctx->dst_cpu)
        dma_free_coherent(dev, size, ctx->dst_cpu, ctx->dst_dma);

    // 释放通道
    dma_release_channel(ctx->chan);
}
