/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\02_PCIe_Basics.md
 * Line: 897
 * Language: c
 * Block ID: 796b14ae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux PCIe设备DMA编程
 */

struct my_pcie_dev {
    struct pci_dev *pdev;
    void __iomem *bar0_base;

    // DMA相关
    struct dma_pool *desc_pool;
    struct device *dma_dev;

    // 描述符环
    struct my_dma_desc *desc_ring;
    dma_addr_t desc_dma;
    int desc_count;

    // 缓冲区
    void *tx_buf;
    dma_addr_t tx_dma;
    size_t buf_size;
};

// 1. 设备初始化
static int my_pcie_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    struct my_pcie_dev *dev;
    int err;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->pdev = pdev;
    dev->dma_dev = &pdev->dev;

    // 启用PCI设备
    err = pci_enable_device(pdev);
    if (err) {
        dev_err(&pdev->dev, "Cannot enable PCI device\n");
        goto err_free;
    }

    // 请求BAR区域
    err = pci_request_regions(pdev, DRIVER_NAME);
    if (err) {
        dev_err(&pdev->dev, "Cannot obtain PCI resources\n");
        goto err_disable;
    }

    // 映射BAR0
    dev->bar0_base = pci_iomap(pdev, 0, pci_resource_len(pdev, 0));
    if (!dev->bar0_base) {
        err = -ENOMEM;
        goto err_release;
    }

    // 设置DMA掩码
    err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
    if (err) {
        err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
        if (err) {
            dev_err(&pdev->dev, "Cannot set DMA mask\n");
            goto err_unmap;
        }
    }

    // 使能PCIe relaxed ordering (如果需要)
    pcie_capability_set_word(pdev, PCI_EXP_DEVCTL,
                              PCI_EXP_DEVCTL_RELAX_EN);

    // 使能设备作为总线主控
    pci_set_master(pdev);

    // 分配DMA缓冲区
    dev->buf_size = 4096;
    dev->tx_buf = dma_alloc_coherent(&pdev->dev, dev->buf_size,
                                      &dev->tx_dma, GFP_KERNEL);
    if (!dev->tx_buf) {
        err = -ENOMEM;
        goto err_unmap;
    }

    // 创建DMA描述符池
    dev->desc_pool = dma_pool_create("my_desc", &pdev->dev,
                                      sizeof(struct my_dma_desc),
                                      64, 0);
    if (!dev->desc_pool) {
        err = -ENOMEM;
        goto err_free_dma;
    }

    // 分配描述符环
    dev->desc_count = 256;
    dev->desc_ring = dma_alloc_coherent(&pdev->dev,
                                         sizeof(struct my_dma_desc) * dev->desc_count,
                                         &dev->desc_dma, GFP_KERNEL);
    if (!dev->desc_ring) {
        err = -ENOMEM;
        goto err_pool;
    }

    // 配置设备描述符环寄存器
    // 写入描述符环基址到设备
    writel(lower_32_bits(dev->desc_dma),
           dev->bar0_base + REG_DESC_BASE_LO);
    writel(upper_32_bits(dev->desc_dma),
           dev->bar0_base + REG_DESC_BASE_HI);
    writel(dev->desc_count, dev->bar0_base + REG_DESC_COUNT);

    dev_info(&pdev->dev, "PCIe device initialized\n");
    dev_info(&pdev->dev, "  BAR0: %pR\n", &pdev->resource[0]);
    dev_info(&pdev->dev, "  DMA buffer: %pad\n", &dev->tx_dma);
    dev_info(&pdev->dev, "  Desc ring: %pad\n", &dev->desc_dma);

    return 0;

err_pool:
    dma_pool_destroy(dev->desc_pool);
err_free_dma:
    dma_free_coherent(&pdev->dev, dev->buf_size, dev->tx_buf, dev->tx_dma);
err_unmap:
    pci_iounmap(pdev, dev->bar0_base);
err_release:
    pci_release_regions(pdev);
err_disable:
    pci_disable_device(pdev);
err_free:
    kfree(dev);
    return err;
}

// 2. 提交DMA传输
static int submit_pcie_dma(struct my_pcie_dev *dev, dma_addr_t src,
                            dma_addr_t dst, size_t len)
{
    struct my_dma_desc *desc;
    u32 tail;

    // 获取下一个描述符位置
    tail = readl(dev->bar0_base + REG_DESC_TAIL);
    desc = &dev->desc_ring[tail];

    // 填充描述符
    desc->src_addr = cpu_to_le64(src);
    desc->dst_addr = cpu_to_le64(dst);
    desc->length = cpu_to_le32(len);
    desc->control = cpu_to_le32(DESC_CTRL_VALID | DESC_CTRL_IRQ);

    // 同步描述符到设备
    dma_wmb();

    // 更新尾指针通知设备
    tail = (tail + 1) % dev->desc_count;
    writel(tail, dev->bar0_base + REG_DESC_TAIL);

    return 0;
}

// 3. 清理和移除
static void my_pcie_remove(struct pci_dev *pdev)
{
    struct my_pcie_dev *dev = pci_get_drvdata(pdev);

    // 停止DMA
    writel(0, dev->bar0_base + REG_DMA_CTRL);

    // 释放资源
    dma_free_coherent(&pdev->dev,
                      sizeof(struct my_dma_desc) * dev->desc_count,
                      dev->desc_ring, dev->desc_dma);
    dma_pool_destroy(dev->desc_pool);
    dma_free_coherent(&pdev->dev, dev->buf_size, dev->tx_buf, dev->tx_dma);

    pci_iounmap(pdev, dev->bar0_base);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

    kfree(dev);
}

// PCI设备ID表
static const struct pci_device_id my_pcie_ids[] = {
    { PCI_DEVICE(0x1234, 0x5678) },  // 厂商ID, 设备ID
    { 0, }
};
MODULE_DEVICE_TABLE(pci, my_pcie_ids);

static struct pci_driver my_pcie_driver = {
    .name = DRIVER_NAME,
    .id_table = my_pcie_ids,
    .probe = my_pcie_probe,
    .remove = my_pcie_remove,
};

module_pci_driver(my_pcie_driver);
