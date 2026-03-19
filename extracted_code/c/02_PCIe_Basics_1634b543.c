/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\02_PCIe_Basics.md
 * Line: 1456
 * Language: c
 * Block ID: 1634b543
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 完整PCIe设备驱动示例
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/ethtool.h>

#define DRIVER_NAME "my_pcie_nic"
#define BAR0_SIZE (4 * 1024)
#define TX_RING_SIZE 256
#define RX_RING_SIZE 256

// 设备寄存器定义
#define REG_DEVICE_CTRL     0x00
#define REG_DEVICE_STATUS   0x04
#define REG_MAC_ADDR_LOW    0x08
#define REG_MAC_ADDR_HIGH   0x0C
#define REG_TX_DESC_BASE    0x10
#define REG_TX_DESC_TAIL    0x18
#define REG_RX_DESC_BASE    0x20
#define REG_RX_DESC_TAIL    0x28
#define REG_INT_MASK        0x30
#define REG_INT_STATUS      0x34

#define CTRL_TX_EN          BIT(0)
#define CTRL_RX_EN          BIT(1)
#define CTRL_RESET          BIT(31)

#define INT_TX_COMPLETE     BIT(0)
#define INT_RX_COMPLETE     BIT(1)
#define INT_LINK_CHANGE     BIT(2)

// 描述符结构
struct my_desc {
    u64 buffer_addr;
    u16 length;
    u16 csum;
    u8 status;
    u8 errors;
    u16 vlan;
} __packed;

// 私有数据结构
struct my_priv {
    struct pci_dev *pdev;
    struct net_device *netdev;

    // MMIO区域
    void __iomem *mmio_base;

    // 描述符环
    struct my_desc *tx_ring;
    struct my_desc *rx_ring;
    dma_addr_t tx_ring_dma;
    dma_addr_t rx_ring_dma;

    // 缓冲区
    struct sk_buff *tx_skb[TX_RING_SIZE];
    struct sk_buff *rx_skb[RX_RING_SIZE];

    // 索引
    u16 tx_head;
    u16 tx_tail;
    u16 rx_tail;

    // NAPI
    struct napi_struct napi;

    // 统计
    u64 packets;
    u64 bytes;
    u64 dropped;
};

// 寄存器访问辅助函数
static inline u32 my_readl(struct my_priv *priv, u32 reg)
{
    return readl(priv->mmio_base + reg);
}

static inline void my_writel(struct my_priv *priv, u32 reg, u32 val)
{
    writel(val, priv->mmio_base + reg);
}

// 分配接收缓冲区
static int my_alloc_rx_buffers(struct my_priv *priv)
{
    int i;

    for (i = 0; i < RX_RING_SIZE; i++) {
        struct sk_buff *skb;
        dma_addr_t dma;

        skb = netdev_alloc_skb(priv->netdev, priv->netdev->mtu + ETH_HLEN);
        if (!skb)
            return -ENOMEM;

        dma = dma_map_single(&priv->pdev->dev, skb->data,
                              priv->netdev->mtu + ETH_HLEN,
                              DMA_FROM_DEVICE);
        if (dma_mapping_error(&priv->pdev->dev, dma)) {
            dev_kfree_skb(skb);
            return -ENOMEM;
        }

        priv->rx_skb[i] = skb;
        priv->rx_ring[i].buffer_addr = cpu_to_le64(dma);
        priv->rx_ring[i].status = 0;
    }

    return 0;
}

// 设备初始化
static int my_hw_init(struct my_priv *priv)
{
    u32 val;
    int err;

    // 重置设备
    my_writel(priv, REG_DEVICE_CTRL, CTRL_RESET);
    msleep(10);

    // 等待重置完成
    do {
        val = my_readl(priv, REG_DEVICE_CTRL);
    } while (val & CTRL_RESET);

    // 分配描述符环
    priv->tx_ring = dma_alloc_coherent(&priv->pdev->dev,
                                        sizeof(struct my_desc) * TX_RING_SIZE,
                                        &priv->tx_ring_dma, GFP_KERNEL);
    priv->rx_ring = dma_alloc_coherent(&priv->pdev->dev,
                                        sizeof(struct my_desc) * RX_RING_SIZE,
                                        &priv->rx_ring_dma, GFP_KERNEL);
    if (!priv->tx_ring || !priv->rx_ring)
        return -ENOMEM;

    // 分配RX缓冲区
    err = my_alloc_rx_buffers(priv);
    if (err)
        return err;

    // 配置描述符基址
    my_writel(priv, REG_TX_DESC_BASE, lower_32_bits(priv->tx_ring_dma));
    my_writel(priv, REG_TX_DESC_BASE + 4, upper_32_bits(priv->tx_ring_dma));
    my_writel(priv, REG_RX_DESC_BASE, lower_32_bits(priv->rx_ring_dma));
    my_writel(priv, REG_RX_DESC_BASE + 4, upper_32_bits(priv->rx_ring_dma));

    // 设置RX尾指针
    my_writel(priv, REG_RX_DESC_TAIL, RX_RING_SIZE - 1);
    priv->rx_tail = 0;
    priv->tx_head = priv->tx_tail = 0;

    // 配置MAC地址
    my_writel(priv, REG_MAC_ADDR_LOW,
              priv->netdev->dev_addr[0] |
              priv->netdev->dev_addr[1] << 8 |
              priv->netdev->dev_addr[2] << 16 |
              priv->netdev->dev_addr[3] << 24);
    my_writel(priv, REG_MAC_ADDR_HIGH,
              priv->netdev->dev_addr[4] |
              priv->netdev->dev_addr[5] << 8);

    // 使能中断
    my_writel(priv, REG_INT_MASK, INT_TX_COMPLETE | INT_RX_COMPLETE);

    return 0;
}

// 启动设备
static int my_open(struct net_device *netdev)
{
    struct my_priv *priv = netdev_priv(netdev);
    int err;

    err = my_hw_init(priv);
    if (err)
        return err;

    napi_enable(&priv->napi);
    netif_start_queue(netdev);

    // 使能TX/RX
    my_writel(priv, REG_DEVICE_CTRL, CTRL_TX_EN | CTRL_RX_EN);

    return 0;
}

// 停止设备
static int my_stop(struct net_device *netdev)
{
    struct my_priv *priv = netdev_priv(netdev);

    netif_stop_queue(netdev);
    napi_disable(&priv->napi);

    // 禁用TX/RX
    my_writel(priv, REG_DEVICE_CTRL, 0);

    // 释放缓冲区
    // ...

    return 0;
}

// 发送数据包
static netdev_tx_t my_start_xmit(struct sk_buff *skb, struct net_device *netdev)
{
    struct my_priv *priv = netdev_priv(netdev);
    u16 tx_head = priv->tx_head;
    struct my_desc *desc;
    dma_addr_t dma;

    // 检查描述符可用性
    if ((tx_head + 1) % TX_RING_SIZE == priv->tx_tail) {
        netif_stop_queue(netdev);
        return NETDEV_TX_BUSY;
    }

    // 映射SKB
    dma = dma_map_single(&priv->pdev->dev, skb->data, skb->len, DMA_TO_DEVICE);
    if (dma_mapping_error(&priv->pdev->dev, dma)) {
        dev_kfree_skb(skb);
        priv->stats.tx_dropped++;
        return NETDEV_TX_OK;
    }

    // 填充描述符
    desc = &priv->tx_ring[tx_head];
    desc->buffer_addr = cpu_to_le64(dma);
    desc->length = cpu_to_le16(skb->len);
    desc->status = 0;

    priv->tx_skb[tx_head] = skb;
    priv->tx_head = (tx_head + 1) % TX_RING_SIZE;

    // 更新尾指针
    my_writel(priv, REG_TX_DESC_TAIL, priv->tx_head);

    return NETDEV_TX_OK;
}

// NAPI轮询
static int my_poll(struct napi_struct *napi, int budget)
{
    struct my_priv *priv = container_of(napi, struct my_priv, napi);
    int work_done = 0;

    // 处理RX完成
    while (work_done < budget) {
        struct my_desc *desc = &priv->rx_ring[priv->rx_tail];

        if (!(desc->status & 0x01))  // 检查DD位
            break;

        // 处理接收的包
        struct sk_buff *skb = priv->rx_skb[priv->rx_tail];
        dma_addr_t dma = le64_to_cpu(desc->buffer_addr);
        u16 len = le16_to_cpu(desc->length);

        dma_unmap_single(&priv->pdev->dev, dma,
                          priv->netdev->mtu + ETH_HLEN, DMA_FROM_DEVICE);

        skb_put(skb, len);
        skb->protocol = eth_type_trans(skb, priv->netdev);

        napi_gro_receive(napi, skb);

        // 分配新的缓冲区
        skb = netdev_alloc_skb(priv->netdev, priv->netdev->mtu + ETH_HLEN);
        dma = dma_map_single(&priv->pdev->dev, skb->data,
                              priv->netdev->mtu + ETH_HLEN, DMA_FROM_DEVICE);

        priv->rx_skb[priv->rx_tail] = skb;
        desc->buffer_addr = cpu_to_le64(dma);
        desc->status = 0;

        priv->rx_tail = (priv->rx_tail + 1) % RX_RING_SIZE;
        work_done++;
    }

    // 更新RX尾指针
    my_writel(priv, REG_RX_DESC_TAIL,
              (priv->rx_tail - 1 + RX_RING_SIZE) % RX_RING_SIZE);

    if (work_done < budget)
        napi_complete_done(napi, work_done);

    return work_done;
}

// 中断处理
static irqreturn_t my_interrupt(int irq, void *data)
{
    struct my_priv *priv = data;
    u32 status;

    status = my_readl(priv, REG_INT_STATUS);
    if (!status)
        return IRQ_NONE;

    // 清除中断
    my_writel(priv, REG_INT_STATUS, status);

    if (status & INT_RX_COMPLETE) {
        my_writel(priv, REG_INT_MASK,
                  my_readl(priv, REG_INT_MASK) & ~INT_RX_COMPLETE);
        napi_schedule(&priv->napi);
    }

    if (status & INT_TX_COMPLETE) {
        // 清理发送描述符
        // ...
    }

    return IRQ_HANDLED;
}

// 网络设备操作
static const struct net_device_ops my_netdev_ops = {
    .ndo_open = my_open,
    .ndo_stop = my_stop,
    .ndo_start_xmit = my_start_xmit,
    .ndo_set_mac_address = eth_mac_addr,
    .ndo_validate_addr = eth_validate_addr,
};

// PCI探测
static int my_pcie_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    struct net_device *netdev;
    struct my_priv *priv;
    int err;

    // 启用PCI设备
    err = pci_enable_device(pdev);
    if (err) {
        dev_err(&pdev->dev, "Cannot enable PCI device\n");
        return err;
    }

    // 请求BAR区域
    err = pci_request_regions(pdev, DRIVER_NAME);
    if (err) {
        dev_err(&pdev->dev, "Cannot obtain PCI resources\n");
        goto err_disable;
    }

    // 检查BAR0
    if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
        dev_err(&pdev->dev, "BAR0 is not MMIO\n");
        err = -ENODEV;
        goto err_release;
    }

    // 分配网络设备
    netdev = alloc_etherdev(sizeof(*priv));
    if (!netdev) {
        err = -ENOMEM;
        goto err_release;
    }

    SET_NETDEV_DEV(netdev, &pdev->dev);
    netdev->netdev_ops = &my_netdev_ops;

    priv = netdev_priv(netdev);
    priv->pdev = pdev;
    priv->netdev = netdev;

    // 映射BAR0
    priv->mmio_base = pci_iomap(pdev, 0, pci_resource_len(pdev, 0));
    if (!priv->mmio_base) {
        err = -ENOMEM;
        goto err_free_netdev;
    }

    // 设置DMA
    err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
    if (err) {
        err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
        if (err)
            goto err_unmap;
    }

    pci_set_master(pdev);

    // 申请中断
    err = pci_alloc_irq_vectors(pdev, 1, 1, PCI_IRQ_ALL_TYPES);
    if (err < 0)
        goto err_unmap;

    err = request_irq(pci_irq_vector(pdev, 0), my_interrupt, 0,
                      DRIVER_NAME, priv);
    if (err)
        goto err_free_irq;

    // 初始化NAPI
    netif_napi_add(netdev, &priv->napi, my_poll, 64);

    // 生成MAC地址
    eth_hw_addr_random(netdev);

    // 注册网络设备
    err = register_netdev(netdev);
    if (err)
        goto err_napi;

    pci_set_drvdata(pdev, priv);

    dev_info(&pdev->dev, "My PCIe NIC driver loaded\n");

    return 0;

err_napi:
    netif_napi_del(&priv->napi);
    free_irq(pci_irq_vector(pdev, 0), priv);
err_free_irq:
    pci_free_irq_vectors(pdev);
err_unmap:
    pci_iounmap(pdev, priv->mmio_base);
err_free_netdev:
    free_netdev(netdev);
err_release:
    pci_release_regions(pdev);
err_disable:
    pci_disable_device(pdev);
    return err;
}

// PCI移除
static void my_pcie_remove(struct pci_dev *pdev)
{
    struct my_priv *priv = pci_get_drvdata(pdev);

    unregister_netdev(priv->netdev);
    netif_napi_del(&priv->napi);

    free_irq(pci_irq_vector(pdev, 0), priv);
    pci_free_irq_vectors(pdev);

    pci_iounmap(pdev, priv->mmio_base);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

    free_netdev(priv->netdev);
}

static const struct pci_device_id my_pcie_ids[] = {
    { PCI_DEVICE(0x1234, 0x5678) },
    { }
};
MODULE_DEVICE_TABLE(pci, my_pcie_ids);

static struct pci_driver my_pcie_driver = {
    .name = DRIVER_NAME,
    .id_table = my_pcie_ids,
    .probe = my_pcie_probe,
    .remove = my_pcie_remove,
};

module_pci_driver(my_pcie_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("My PCIe NIC Driver");
MODULE_AUTHOR("Author");
