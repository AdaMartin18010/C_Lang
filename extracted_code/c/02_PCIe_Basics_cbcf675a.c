/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\02_PCIe_Basics.md
 * Line: 1170
 * Language: c
 * Block ID: cbcf675a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux PCIe MSI/MSI-X编程
 */

// 1. 启用MSI或MSI-X
static int setup_interrupts(struct pci_dev *pdev, struct my_dev *dev)
{
    int ret, nvec;

    // 尝试MSI-X (首选)
    ret = pci_alloc_irq_vectors(pdev, 1, 8, PCI_IRQ_MSIX);
    if (ret < 0) {
        dev_info(&pdev->dev, "MSI-X not available, trying MSI\n");

        // 尝试MSI
        ret = pci_alloc_irq_vectors(pdev, 1, 4, PCI_IRQ_MSI);
        if (ret < 0) {
            dev_info(&pdev->dev, "MSI not available, using legacy IRQ\n");

            // 使用传统中断
            ret = pci_alloc_irq_vectors(pdev, 1, 1, PCI_IRQ_LEGACY);
            if (ret < 0)
                return ret;
        }
    }

    nvec = ret;
    dev_info(&pdev->dev, "Allocated %d interrupt vectors\n", nvec);

    // 注册中断处理函数
    for (int i = 0; i < nvec; i++) {
        ret = request_irq(pci_irq_vector(pdev, i), my_irq_handler,
                          0, DRIVER_NAME, dev);
        if (ret) {
            dev_err(&pdev->dev, "Failed to request IRQ %d\n", i);
            goto err_free_irq;
        }
    }

    return 0;

err_free_irq:
    while (--i >= 0)
        free_irq(pci_irq_vector(pdev, i), dev);
    pci_free_irq_vectors(pdev);
    return ret;
}

// 2. 中断处理函数
static irqreturn_t my_irq_handler(int irq, void *data)
{
    struct my_dev *dev = data;
    u32 status;

    // 读取中断状态
    status = readl(dev->bar0 + REG_INT_STATUS);

    if (!status)
        return IRQ_NONE;  // 不是我们的中断

    // 处理RX完成
    if (status & INT_RX_DONE) {
        napi_schedule(&dev->napi);
    }

    // 处理TX完成
    if (status & INT_TX_DONE) {
        // 清理已发送的描述符
        // ...
    }

    // 清除中断状态
    writel(status, dev->bar0 + REG_INT_STATUS);

    return IRQ_HANDLED;
}

// 3. 清理中断
static void cleanup_interrupts(struct pci_dev *pdev, struct my_dev *dev)
{
    int i, nvec = pci_msix_vec_count(pdev);

    if (nvec <= 0)
        nvec = pci_msi_vec_count(pdev);
    if (nvec <= 0)
        nvec = 1;

    for (i = 0; i < nvec; i++)
        free_irq(pci_irq_vector(pdev, i), dev);

    pci_free_irq_vectors(pdev);
}

// 4. 中断亲和性设置
static int set_irq_affinity(struct pci_dev *pdev, int vector, int cpu)
{
    int irq = pci_irq_vector(pdev, vector);

    return irq_set_affinity_hint(irq, cpumask_of(cpu));
}
