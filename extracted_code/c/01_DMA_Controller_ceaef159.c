/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\01_DMA_Controller.md
 * Line: 994
 * Language: c
 * Block ID: ceaef159
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 现代网卡DMA设计 (以Intel E1000e为例)
 */

// 接收描述符格式
union e1000_rx_desc_extended {
    struct {
        __le64 buffer_addr;     // 缓冲区物理地址
        __le64 reserved;
    } read;
    struct {
        struct {
            __le32 mrq;         // RSS哈希值
            union {
                __le32 rss;     // RSS类型
                struct {
                    __le16 ip_id;   // IP标识
                    __le16 csum;    // 校验和
                } csum_ip;
            } hi_dword;
        } lower;
        struct {
            __le32 status_error;    // 状态和错误
            __le16 length;          // 包长度
            __le16 vlan;            // VLAN标签
        } upper;
    } wb;  // 写回格式
};

// 发送描述符格式
union e1000_tx_desc {
    struct {
        __le64 buffer_addr;     // 缓冲区地址
        __le32 cmd_type_len;    // 命令/类型/长度
        __le32 olinfo_status;   // 卸载信息/状态
    };
};

// 初始化RX环
static int e1000_setup_rx_resources(struct e1000_adapter *adapter)
{
    struct e1000_ring *rx_ring = adapter->rx_ring;
    int size, desc_len;

    // 分配描述符环 (物理连续)
    desc_len = sizeof(union e1000_rx_desc_extended);
    size = desc_len * rx_ring->count;

    rx_ring->desc = dma_alloc_coherent(&adapter->pdev->dev, size,
                                        &rx_ring->dma, GFP_KERNEL);
    if (!rx_ring->desc)
        return -ENOMEM;

    // 分配接收缓冲区
    rx_ring->buffer_info = vzalloc(sizeof(*rx_ring->buffer_info) * rx_ring->count);

    // 分配并映射缓冲区
    for (i = 0; i < rx_ring->count; i++) {
        struct e1000_buffer *bi = &rx_ring->buffer_info[i];
        struct sk_buff *skb;

        skb = netdev_alloc_skb(adapter->netdev, adapter->rx_buffer_len);
        if (!skb)
            goto err;

        bi->skb = skb;
        bi->dma = dma_map_single(&adapter->pdev->dev, skb->data,
                                  adapter->rx_buffer_len, DMA_FROM_DEVICE);

        // 填入描述符
        rx_desc = E1000_RX_DESC_EXT(*rx_ring, i);
        rx_desc->read.buffer_addr = cpu_to_le64(bi->dma);
    }

    // 设置尾指针
    writel(rx_ring->count - 1, adapter->hw.hw_addr + rx_ring->tail);

    return 0;
}

// 接收处理 (NAPI轮询)
static int e1000_clean_rx_irq(struct e1000_adapter *adapter,
                               struct e1000_ring *rx_ring,
                               int budget)
{
    struct pci_dev *pdev = adapter->pdev;
    union e1000_rx_desc_extended *rx_desc;
    struct e1000_buffer *bi;
    int i = rx_ring->next_to_clean;
    int cleaned_count = 0;

    while (cleaned_count < budget) {
        rx_desc = E1000_RX_DESC_EXT(*rx_ring, i);

        // 检查DD位 (Descriptor Done)
        if (!(rx_desc->wb.upper.status_error & E1000_RXD_STAT_DD))
            break;  // 没有新包

        bi = &rx_ring->buffer_info[i];

        // 同步CPU视图
dma_sync_single_for_cpu(&pdev->dev, bi->dma,
                              adapter->rx_buffer_len,
                              DMA_FROM_DEVICE);

        // 获取包长度
        u16 length = le16_to_cpu(rx_desc->wb.upper.length);

        // 构建skb并提交
        struct sk_buff *skb = bi->skb;
        skb_put(skb, length);
        skb->protocol = eth_type_trans(skb, adapter->netdev);

        napi_gro_receive(&adapter->napi, skb);

        // 分配新缓冲区
        bi->skb = netdev_alloc_skb(adapter->netdev, adapter->rx_buffer_len);
        bi->dma = dma_map_single(&pdev->dev, bi->skb->data,
                                  adapter->rx_buffer_len, DMA_FROM_DEVICE);

        // 更新描述符
        rx_desc->read.buffer_addr = cpu_to_le64(bi->dma);

        i = (i + 1) % rx_ring->count;
        cleaned_count++;
    }

    rx_ring->next_to_clean = i;

    // 更新尾指针，释放已处理的描述符
    writel(i, adapter->hw.hw_addr + rx_ring->tail);

    return cleaned_count;
}
