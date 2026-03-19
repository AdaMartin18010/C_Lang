/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\01_DMA_Controller.md
 * Line: 913
 * Language: c
 * Block ID: 98dc7fa1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Intel I/OAT DMA引擎
 * 用于高吞吐量数据传输，如网络、存储
 */

// I/OAT寄存器定义
#define IOAT_CHANCTRL_OFFSET        0x80
#define IOAT_CHANSTS_OFFSET         0x84
#define IOAT_CHAINADDR_OFFSET       0x88
#define IOAT_CHANCMP_OFFSET         0xA0

// 描述符格式
struct ioat_dma_descriptor {
    uint32_t size;              // 传输大小
    uint32_t ctl;               // 控制字
    uint64_t src_addr;          // 源物理地址
    uint64_t dst_addr;          // 目的物理地址
    uint64_t next;              // 下一个描述符地址
    uint64_t rsvd;
} __attribute__((packed));

// I/OAT驱动初始化
static int ioat_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    struct ioatdma_device *ioat_dma;
    void __iomem *regs;

    // 启用PCI设备
    pci_enable_device_mem(pdev);
    pci_set_master(pdev);

    // 映射BAR
    regs = pci_iomap(pdev, 0, 0);
    if (!regs)
        return -ENOMEM;

    // 分配DMA设备结构
    ioat_dma = devm_kzalloc(&pdev->dev, sizeof(*ioat_dma), GFP_KERNEL);
    ioat_dma->reg_base = regs;

    // 初始化DMA通道
    ioat_dma->dma_dev = ioat_init_channel(ioat_dma, 0);

    // 注册到DMA引擎框架
    dma_async_device_register(&ioat_dma->dma_dev);

    return 0;
}

// 提交DMA操作
static dma_cookie_t ioat_tx_submit(struct dma_async_tx_descriptor *tx)
{
    struct ioat_ring_ent *desc;
    struct ioat_chan_common *chan;

    desc = tx_to_ioat_desc(tx);
    chan = to_ioat_chan(tx->chan);

    // 填充描述符
    desc->hw->size = desc->len;
    desc->hw->ctl = IOAT_CTL_OP_MEMCPY | IOAT_CTL_COMPL_DCA;
    desc->hw->src_addr = desc->src;
    desc->hw->dst_addr = desc->dst;
    desc->hw->next = 0;  // 链结束

    // 写入描述符地址到硬件
    writel(lower_32_bits(desc->phys),
           chan->reg_base + IOAT_CHAINADDR_OFFSET);
    writel(upper_32_bits(desc->phys),
           chan->reg_base + IOAT_CHAINADDR_OFFSET + 4);

    // 启动通道
    ioat_start(chan);

    return tx->cookie;
}
