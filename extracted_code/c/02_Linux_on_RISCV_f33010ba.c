/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 561
 * Language: c
 * Block ID: f33010ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RISC-V DMA驱动示例
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/direction.h>

// DMA传输函数
int my_dma_transfer(struct my_drvdata *drv, void *src, void *dst, size_t len)
{
    struct dma_async_tx_descriptor *desc;
    dma_cookie_t cookie;
    dma_addr_t src_dma, dst_dma;

    // 映射DMA地址
    src_dma = dma_map_single(drv->dma_chan->device->dev, src, len, DMA_TO_DEVICE);
    if (dma_mapping_error(drv->dma_chan->device->dev, src_dma))
        return -ENOMEM;

    dst_dma = dma_map_single(drv->dma_chan->device->dev, dst, len, DMA_FROM_DEVICE);
    if (dma_mapping_error(drv->dma_chan->device->dev, dst_dma)) {
        dma_unmap_single(drv->dma_chan->device->dev, src_dma, len, DMA_TO_DEVICE);
        return -ENOMEM;
    }

    // 准备DMA描述符
    desc = dmaengine_prep_dma_memcpy(drv->dma_chan, dst_dma, src_dma, len,
                                     DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
    if (!desc) {
        ret = -ENOMEM;
        goto err_unmap;
    }

    // 设置回调
    desc->callback = my_dma_complete;
    desc->callback_param = drv;

    // 提交传输
    cookie = desc->tx_submit(desc);
    if (dma_submit_error(cookie)) {
        ret = -EIO;
        goto err_unmap;
    }

    // 启动传输
    dma_async_issue_pending(drv->dma_chan);

    return 0;

err_unmap:
    dma_unmap_single(drv->dma_chan->device->dev, src_dma, len, DMA_TO_DEVICE);
    dma_unmap_single(drv->dma_chan->device->dev, dst_dma, len, DMA_FROM_DEVICE);
    return ret;
}
