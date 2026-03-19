/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 428
 * Language: c
 * Block ID: 2346e123
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 平台设备驱动模板
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>

#define DRIVER_NAME "my_riscv_driver"

struct my_drvdata {
    void __iomem *base;
    int irq;
    struct clk *clk;
    struct dma_chan *dma_chan;
};

// 寄存器定义
#define REG_CTRL    0x00
#define REG_STATUS  0x04
#define REG_DATA    0x08
#define REG_IRQ_EN  0x0C

// 中断处理
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    struct my_drvdata *drv = dev_id;
    u32 status = readl(drv->base + REG_STATUS);

    if (status & 0x1) {
        // 处理完成中断
        writel(0x1, drv->base + REG_STATUS);  // 清除中断
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

// 探测函数
static int my_probe(struct platform_device *pdev)
{
    struct my_drvdata *drv;
    struct resource *res;
    int ret;

    drv = devm_kzalloc(&pdev->dev, sizeof(*drv), GFP_KERNEL);
    if (!drv)
        return -ENOMEM;

    // 1. 映射内存
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    drv->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(drv->base))
        return PTR_ERR(drv->base);

    // 2. 获取中断
    drv->irq = platform_get_irq(pdev, 0);
    if (drv->irq < 0)
        return drv->irq;

    ret = devm_request_irq(&pdev->dev, drv->irq, my_irq_handler,
                           IRQF_SHARED, DRIVER_NAME, drv);
    if (ret)
        return ret;

    // 3. 使能时钟
    drv->clk = devm_clk_get(&pdev->dev, NULL);
    if (IS_ERR(drv->clk))
        return PTR_ERR(drv->clk);

    ret = clk_prepare_enable(drv->clk);
    if (ret)
        return ret;

    // 4. 配置DMA
    drv->dma_chan = dma_request_chan(&pdev->dev, "rx");
    if (IS_ERR(drv->dma_chan)) {
        ret = PTR_ERR(drv->dma_chan);
        goto err_clk;
    }

    // 5. 初始化硬件
    writel(0x0, drv->base + REG_CTRL);      // 复位
    writel(0x1, drv->base + REG_IRQ_EN);    // 使能中断

    platform_set_drvdata(pdev, drv);

    dev_info(&pdev->dev, "Driver probed successfully\n");
    return 0;

err_clk:
    clk_disable_unprepare(drv->clk);
    return ret;
}

// 移除函数
static int my_remove(struct platform_device *pdev)
{
    struct my_drvdata *drv = platform_get_drvdata(pdev);

    dma_release_channel(drv->dma_chan);
    clk_disable_unprepare(drv->clk);

    return 0;
}

// 设备树匹配
static const struct of_device_id my_of_match[] = {
    { .compatible = "acme,my-accelerator" },
    { }
};
MODULE_DEVICE_TABLE(of, my_of_match);

static struct platform_driver my_driver = {
    .probe = my_probe,
    .remove = my_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = my_of_match,
    },
};

module_platform_driver(my_driver);

MODULE_AUTHOR("Developer");
MODULE_DESCRIPTION("RISC-V Platform Driver Example");
MODULE_LICENSE("GPL");
