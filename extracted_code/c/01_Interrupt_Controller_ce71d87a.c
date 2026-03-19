/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 1402
 * Language: c
 * Block ID: ce71d87a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux设备驱动中断注册完整示例
 * 以网卡驱动为例
 */

#include <linux/interrupt.h>
#include <linux/netdevice.h>

struct my_netdev {
    struct net_device *netdev;
    void __iomem *mmio_base;
    int irq;
    struct napi_struct napi;

    // 统计信息
    u64 rx_packets;
    u64 rx_bytes;
    u64 tx_packets;
    u64 tx_bytes;
};

/*
 * 硬中断处理函数 (Top Half)
 * 特点: 快速执行，屏蔽当前IRQ
 */
static irqreturn_t my_netdev_irq_handler(int irq, void *dev_id)
{
    struct my_netdev *priv = dev_id;
    u32 status;

    // 读取设备状态寄存器
    status = readl(priv->mmio_base + ISR_REG);

    // 检查是否是我们设备的中断
    if (!(status & (RX_INT | TX_INT | ERR_INT)))
        return IRQ_NONE;    // 不是我们的中断

    // 禁用中断 (NAPI轮询模式)
    writel(0, priv->mmio_base + IER_REG);

    // 调度NAPI进行软中断处理
    napi_schedule(&priv->napi);

    return IRQ_HANDLED;
}

/*
 * NAPI轮询函数 (Bottom Half)
 * 特点: 可以睡眠，处理大量数据包
 */
static int my_netdev_poll(struct napi_struct *napi, int budget)
{
    struct my_netdev *priv = container_of(napi, struct my_netdev, napi);
    int work_done = 0;

    // 处理接收数据包，最多budget个
    while (work_done < budget) {
        struct sk_buff *skb;

        // 检查是否有数据
        if (!(readl(priv->mmio_base + RX_STATUS) & RX_AVAIL))
            break;

        // 分配skb并接收数据
        skb = netdev_alloc_skb(priv->netdev, RX_BUF_SIZE);
        if (!skb)
            break;

        // 从设备读取数据到skb
        // ... DMA同步，复制数据等

        // 提交给网络栈
        napi_gro_receive(napi, skb);

        priv->rx_packets++;
        work_done++;
    }

    // 如果处理完所有数据，退出NAPI模式
    if (work_done < budget) {
        napi_complete_done(napi, work_done);

        // 重新使能中断
        writel(RX_INT | TX_INT, priv->mmio_base + IER_REG);
    }

    return work_done;
}

/*
 * 设备初始化 - 注册中断
 */
static int my_netdev_open(struct net_device *netdev)
{
    struct my_netdev *priv = netdev_priv(netdev);
    int ret;

    // 分配NAPI
    netif_napi_add(netdev, &priv->napi, my_netdev_poll, 64);

    // 1. 请求中断 (传统方式)
    ret = request_irq(priv->irq,                     // IRQ号
                      my_netdev_irq_handler,         // 处理函数
                      IRQF_SHARED,                   // 共享中断标志
                      netdev->name,                  // 设备名称
                      priv);                         // dev_id
    if (ret) {
        pr_err("Failed to request IRQ %d\n", priv->irq);
        netif_napi_del(&priv->napi);
        return ret;
    }

    // 2. 或者使用线程化中断 (更简单)
    // ret = request_threaded_irq(priv->irq,
    //                            my_netdev_irq_handler,  // 硬中断
    //                            my_netdev_thread_fn,    // 线程函数
    //                            IRQF_ONESHOT,
    //                            netdev->name, priv);

    // 3. 或者使用 managed 版本 (自动释放)
    // ret = devm_request_irq(&pdev->dev, priv->irq, ...);

    // 初始化硬件
    // ...

    // 使能NAPI
    napi_enable(&priv->napi);

    // 使能设备中断
    writel(RX_INT | TX_INT, priv->mmio_base + IER_REG);

    netif_start_queue(netdev);

    return 0;
}

/*
 * 设备关闭 - 释放中断
 */
static int my_netdev_stop(struct net_device *netdev)
{
    struct my_netdev *priv = netdev_priv(netdev);

    // 停止网络队列
    netif_stop_queue(netdev);

    // 禁用NAPI
    napi_disable(&priv->napi);

    // 禁用设备中断
    writel(0, priv->mmio_base + IER_REG);

    // 同步中断
    synchronize_irq(priv->irq);

    // 释放中断
    free_irq(priv->irq, priv);

    // 删除NAPI
    netif_napi_del(&priv->napi);

    return 0;
}
