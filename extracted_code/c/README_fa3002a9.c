/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\README.md
 * Line: 617
 * Language: c
 * Block ID: fa3002a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 平台设备驱动示例
 * 适用于不通过PCI/USB等总线枚举的嵌入式设备
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>

/* 设备寄存器定义 */
#define REG_CONTROL     0x00
#define REG_STATUS      0x04
#define REG_DATA_TX     0x08
#define REG_DATA_RX     0x0C
#define REG_IRQ_MASK    0x10
#define REG_IRQ_STATUS  0x14

#define CTRL_ENABLE     BIT(0)
#define CTRL_RESET      BIT(1)
#define CTRL_DMA_EN     BIT(2)

#define STATUS_BUSY     BIT(0)
#define STATUS_TX_EMPTY BIT(1)
#define STATUS_RX_FULL  BIT(2)

#define IRQ_TX_DONE     BIT(0)
#define IRQ_RX_READY    BIT(1)
#define IRQ_ERROR       BIT(2)

/* 设备私有数据结构 */
struct myplatform_dev {
    struct device *dev;
    void __iomem *base;
    int irq;
    struct clk *clk;
    struct reset_control *rst;

    /* DMA */
    struct dma_chan *dma_tx;
    struct dma_chan *dma_rx;
    dma_addr_t dma_tx_phys;
    dma_addr_t dma_rx_phys;
    void *dma_tx_virt;
    void *dma_rx_virt;

    /* 同步 */
    struct completion tx_complete;
    struct completion rx_complete;
    spinlock_t lock;

    /* 统计 */
    u64 bytes_transferred;
    u32 irq_count;
};

/* 中断处理函数 */
static irqreturn_t myplatform_irq_handler(int irq, void *dev_id)
{
    struct myplatform_dev *priv = dev_id;
    u32 status;
    unsigned long flags;

    spin_lock_irqsave(&priv->lock, flags);

    status = readl(priv->base + REG_IRQ_STATUS);

    if (status & IRQ_TX_DONE) {
        complete(&priv->tx_complete);
        writel(IRQ_TX_DONE, priv->base + REG_IRQ_STATUS);  /* W1C */
    }

    if (status & IRQ_RX_READY) {
        complete(&priv->rx_complete);
        writel(IRQ_RX_READY, priv->base + REG_IRQ_STATUS);
    }

    if (status & IRQ_ERROR) {
        dev_err(priv->dev, "Hardware error!\n");
        writel(IRQ_ERROR, priv->base + REG_IRQ_STATUS);
    }

    priv->irq_count++;
    spin_unlock_irqrestore(&priv->lock, flags);

    return IRQ_HANDLED;
}

/* 设备初始化 */
static int myplatform_hw_init(struct myplatform_dev *priv)
{
    u32 ctrl;

    /* 复位设备 */
    reset_control_assert(priv->rst);
    udelay(10);
    reset_control_deassert(priv->rst);
    msleep(10);  /* 等待稳定 */

    /* 禁用中断 */
    writel(0, priv->base + REG_IRQ_MASK);

    /* 清除状态 */
    writel(0xFFFFFFFF, priv->base + REG_IRQ_STATUS);

    /* 配置控制寄存器 */
    ctrl = CTRL_ENABLE;
    if (priv->dma_tx || priv->dma_rx)
        ctrl |= CTRL_DMA_EN;
    writel(ctrl, priv->base + REG_CONTROL);

    /* 启用中断 */
    writel(IRQ_TX_DONE | IRQ_RX_READY | IRQ_ERROR,
           priv->base + REG_IRQ_MASK);

    return 0;
}

/* DMA完成回调 */
static void myplatform_dma_callback(void *param)
{
    struct completion *comp = param;
    complete(comp);
}

/* 数据传输（使用DMA或PIO） */
static int myplatform_transfer(struct myplatform_dev *priv,
                               const void *tx_buf, void *rx_buf,
                               size_t len)
{
    struct dma_async_tx_descriptor *tx_desc = NULL;
    struct dma_async_tx_descriptor *rx_desc = NULL;
    dma_cookie_t cookie;
    int ret = 0;

    reinit_completion(&priv->tx_complete);
    reinit_completion(&priv->rx_complete);

    /* DMA传输 */
    if (priv->dma_tx && tx_buf && len > DMA_THRESHOLD) {
        /* 准备TX DMA */
        memcpy(priv->dma_tx_virt, tx_buf, len);
        dma_sync_single_for_device(priv->dev, priv->dma_tx_phys,
                                   len, DMA_TO_DEVICE);

        tx_desc = dmaengine_prep_slave_single(
            priv->dma_tx, priv->dma_tx_phys, len,
            DMA_MEM_TO_DEV, DMA_PREP_INTERRUPT);

        if (tx_desc) {
            tx_desc->callback = myplatform_dma_callback;
            tx_desc->callback_param = &priv->tx_complete;
            cookie = tx_desc->tx_submit(tx_desc);
            dma_async_issue_pending(priv->dma_tx);
        }
    }

    if (priv->dma_rx && rx_buf && len > DMA_THRESHOLD) {
        /* 准备RX DMA */
        rx_desc = dmaengine_prep_slave_single(
            priv->dma_rx, priv->dma_rx_phys, len,
            DMA_DEV_TO_MEM, DMA_PREP_INTERRUPT);

        if (rx_desc) {
            rx_desc->callback = myplatform_dma_callback;
            rx_desc->callback_param = &priv->rx_complete;
            cookie = rx_desc->tx_submit(rx_desc);
            dma_async_issue_pending(priv->dma_rx);
        }
    }

    /* 等待传输完成 */
    if (tx_desc) {
        if (!wait_for_completion_timeout(&priv->tx_complete,
                                         msecs_to_jiffies(1000))) {
            dev_err(priv->dev, "TX DMA timeout\n");
            dmaengine_terminate_sync(priv->dma_tx);
            ret = -ETIMEDOUT;
            goto out;
        }
    } else if (tx_buf) {
        /* PIO发送 */
        const u8 *p = tx_buf;
        size_t i;
        for (i = 0; i < len; i++) {
            /* 等待TX FIFO空闲 */
            while (readl(priv->base + REG_STATUS) & STATUS_BUSY)
                cpu_relax();
            writel(p[i], priv->base + REG_DATA_TX);
        }
        /* 等待完成 */
        if (!wait_for_completion_timeout(&priv->tx_complete,
                                         msecs_to_jiffies(1000))) {
            ret = -ETIMEDOUT;
            goto out;
        }
    }

    if (rx_desc) {
        if (!wait_for_completion_timeout(&priv->rx_complete,
                                         msecs_to_jiffies(1000))) {
            dev_err(priv->dev, "RX DMA timeout\n");
            dmaengine_terminate_sync(priv->dma_rx);
            ret = -ETIMEDOUT;
            goto out;
        }
        dma_sync_single_for_cpu(priv->dev, priv->dma_rx_phys,
                                len, DMA_FROM_DEVICE);
        memcpy(rx_buf, priv->dma_rx_virt, len);
    } else if (rx_buf) {
        /* PIO接收 */
        u8 *p = rx_buf;
        size_t i;
        for (i = 0; i < len; i++) {
            /* 等待RX数据 */
            while (!(readl(priv->base + REG_STATUS) & STATUS_RX_FULL))
                cpu_relax();
            p[i] = readl(priv->base + REG_DATA_RX);
        }
    }

    priv->bytes_transferred += len;

out:
    return ret;
}

/* 平台驱动probe函数 */
static int myplatform_probe(struct platform_device *pdev)
{
    struct myplatform_dev *priv;
    struct resource *res;
    int ret;

    priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    priv->dev = &pdev->dev;
    platform_set_drvdata(pdev, priv);
    spin_lock_init(&priv->lock);
    init_completion(&priv->tx_complete);
    init_completion(&priv->rx_complete);

    /* 获取寄存器资源 */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    priv->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(priv->base))
        return PTR_ERR(priv->base);

    /* 获取中断 */
    priv->irq = platform_get_irq(pdev, 0);
    if (priv->irq < 0)
        return priv->irq;

    ret = devm_request_irq(&pdev->dev, priv->irq,
                           myplatform_irq_handler,
                           IRQF_SHARED, dev_name(&pdev->dev), priv);
    if (ret)
        return ret;

    /* 获取时钟 */
    priv->clk = devm_clk_get(&pdev->dev, NULL);
    if (IS_ERR(priv->clk))
        return PTR_ERR(priv->clk);

    ret = clk_prepare_enable(priv->clk);
    if (ret)
        return ret;

    /* 获取复位控制 */
    priv->rst = devm_reset_control_get_exclusive(&pdev->dev, NULL);
    if (IS_ERR(priv->rst)) {
        ret = PTR_ERR(priv->rst);
        goto err_clk;
    }

    /* 申请DMA通道 */
    priv->dma_tx = dma_request_slave_channel(&pdev->dev, "tx");
    priv->dma_rx = dma_request_slave_channel(&pdev->dev, "rx");

    /* 分配DMA缓冲区 */
    if (priv->dma_tx) {
        priv->dma_tx_virt = dmam_alloc_coherent(&pdev->dev, PAGE_SIZE,
                                                &priv->dma_tx_phys,
                                                GFP_KERNEL);
        if (!priv->dma_tx_virt) {
            ret = -ENOMEM;
            goto err_dma;
        }
    }

    if (priv->dma_rx) {
        priv->dma_rx_virt = dmam_alloc_coherent(&pdev->dev, PAGE_SIZE,
                                                &priv->dma_rx_phys,
                                                GFP_KERNEL);
        if (!priv->dma_rx_virt) {
            ret = -ENOMEM;
            goto err_dma;
        }
    }

    /* 初始化硬件 */
    ret = myplatform_hw_init(priv);
    if (ret)
        goto err_dma;

    dev_info(&pdev->dev, "Probed successfully\n");
    return 0;

err_dma:
    if (priv->dma_tx)
        dma_release_channel(priv->dma_tx);
    if (priv->dma_rx)
        dma_release_channel(priv->dma_rx);
err_clk:
    clk_disable_unprepare(priv->clk);
    return ret;
}

/* 平台驱动remove函数 */
static int myplatform_remove(struct platform_device *pdev)
{
    struct myplatform_dev *priv = platform_get_drvdata(pdev);

    /* 禁用设备 */
    writel(0, priv->base + REG_CONTROL);

    /* 释放DMA */
    if (priv->dma_tx)
        dma_release_channel(priv->dma_tx);
    if (priv->dma_rx)
        dma_release_channel(priv->dma_rx);

    clk_disable_unprepare(priv->clk);

    return 0;
}

/* Device Tree匹配表 */
static const struct of_device_id myplatform_of_match[] = {
    { .compatible = "myvendor,mydevice" },
    { .compatible = "myvendor,mydevice-v2" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, myplatform_of_match);

/* 平台驱动结构 */
static struct platform_driver myplatform_driver = {
    .driver = {
        .name = "myplatform",
        .of_match_table = myplatform_of_match,
    },
    .probe = myplatform_probe,
    .remove = myplatform_remove,
};

module_platform_driver(myplatform_driver);
