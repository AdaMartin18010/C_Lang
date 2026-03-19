/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\01_DMA_Controller.md
 * Line: 574
 * Language: c
 * Block ID: 00040875
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核分散/聚集列表管理
 * include/linux/scatterlist.h
 */

// 分散/聚集列表项
struct scatterlist {
#ifdef CONFIG_DEBUG_SG
    unsigned long   sg_magic;       // 调试魔数
#endif
    unsigned long   page_link;      // 页指针或链接到下一段
    unsigned int    offset;         // 页内偏移
    unsigned int    length;         // 本段长度
    dma_addr_t      dma_address;    // DMA地址 (物理或IOVA)
    unsigned int    dma_length;     // DMA长度 (可能与length不同)
};

// 创建SG列表示例
struct scatterlist *sg;
int nents, i;

// 分配SG列表
sg = kmalloc(sizeof(*sg) * num_pages, GFP_KERNEL);
sg_init_table(sg, num_pages);

// 填充SG列表 (从用户缓冲区)
nents = 0;
for (i = 0; i < num_pages; i++) {
    struct page *page;
    unsigned int offset, len;

    // 获取物理页
    page = alloc_page(GFP_KERNEL);

    // 计算偏移和长度
    offset = (i == 0) ? (user_addr & ~PAGE_MASK) : 0;
    len = min_t(unsigned int, PAGE_SIZE - offset, remaining);

    // 设置SG项
    sg_set_page(&sg[nents], page, len, offset);

    remaining -= len;
    nents++;
}

// 标记最后一个SG项
sg_mark_end(&sg[nents - 1]);

// 映射SG列表用于DMA (获取DMA地址)
dma_addr_t dma_addr;
int mapped_nents;

mapped_nents = dma_map_sg(dev, sg, nents, DMA_TO_DEVICE);
if (!mapped_nents) {
    // 映射失败
    return -ENOMEM;
}

// 遍历映射后的SG列表
for_each_sg(sg, s, mapped_nents, i) {
    dma_addr = sg_dma_address(s);
    dma_len = sg_dma_len(s);

    // 将DMA地址填入设备描述符
    // ...
}

// DMA完成后取消映射
dma_unmap_sg(dev, sg, nents, DMA_TO_DEVICE);

// 释放SG列表
kfree(sg);

// 辅助函数
static inline void sg_set_buf(struct scatterlist *sg, const void *buf,
                               unsigned int buflen)
{
    sg_set_page(sg, virt_to_page(buf), buflen, offset_in_page(buf));
}

static inline void sg_set_page(struct scatterlist *sg, struct page *page,
                                unsigned int len, unsigned int offset)
{
    sg->page_link = (unsigned long)page;
    sg->offset = offset;
    sg->length = len;
}

static inline void sg_mark_end(struct scatterlist *sg)
{
    sg->page_link |= 0x02;  // 标记结束
}
