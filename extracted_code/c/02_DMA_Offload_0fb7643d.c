/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 1283
 * Language: c
 * Block ID: 0fb7643d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 缓存一致性管理 - ARMv8架构下的DMA缓存操作
 * ============================================================================ */

/* Cache行大小 */
#define CACHE_LINE_SIZE         64
#define CACHE_LINE_MASK         (~(CACHE_LINE_SIZE - 1))

/* 数据缓存清理 (Clean) - 写回脏数据 */
static inline void cache_clean_range(void *addr, size_t len)
{
    uintptr_t start = (uintptr_t)addr & CACHE_LINE_MASK;
    uintptr_t end = ((uintptr_t)addr + len + CACHE_LINE_SIZE - 1)
                    & CACHE_LINE_MASK;

    for (uintptr_t p = start; p < end; p += CACHE_LINE_SIZE) {
        __asm__ __volatile__(
            "dc cvac, %0"
            :
            : "r"(p)
            : "memory"
        );
    }
    __asm__ __volatile__("dsb ish" ::: "memory");
}

/* 数据缓存失效 (Invalidate) - 丢弃缓存数据 */
static inline void cache_invalidate_range(void *addr, size_t len)
{
    uintptr_t start = (uintptr_t)addr & CACHE_LINE_MASK;
    uintptr_t end = ((uintptr_t)addr + len + CACHE_LINE_SIZE - 1)
                    & CACHE_LINE_MASK;

    for (uintptr_t p = start; p < end; p += CACHE_LINE_SIZE) {
        __asm__ __volatile__(
            "dc ivac, %0"
            :
            : "r"(p)
            : "memory"
        );
    }
    __asm__ __volatile__("dsb ish" ::: "memory");
}

/* 数据缓存清理并失效 (Clean & Invalidate) */
static inline void cache_flush_range(void *addr, size_t len)
{
    uintptr_t start = (uintptr_t)addr & CACHE_LINE_MASK;
    uintptr_t end = ((uintptr_t)addr + len + CACHE_LINE_SIZE - 1)
                    & CACHE_LINE_MASK;

    for (uintptr_t p = start; p < end; p += CACHE_LINE_SIZE) {
        __asm__ __volatile__(
            "dc civac, %0"
            :
            : "r"(p)
            : "memory"
        );
    }
    __asm__ __volatile__("dsb ish" ::: "memory");
}

/* DMA映射同步操作 */
typedef enum {
    DMA_SYNC_FOR_DEVICE = 0,            /* 同步给设备 */
    DMA_SYNC_FOR_CPU                    /* 同步给CPU */
} dma_sync_dir_t;

/* 同步DMA映射 */
void dma_sync_sg_for_device(sg_node_t *sg_list, uint32_t nents,
                             dma_sync_dir_t dir)
{
    sg_node_t *node = sg_list;

    while (node && nents--) {
        if (dir == DMA_SYNC_FOR_DEVICE) {
            /* CPU -> Device: 清理缓存 */
            cache_clean_range(node->virt_addr, node->len);
        } else {
            /* Device -> CPU: 失效缓存 */
            cache_invalidate_range(node->virt_addr, node->len);
        }
        node = node->next;
    }
}

/* 非缓存内存分配 */
void *dma_alloc_coherent(size_t size, uint64_t *phys_addr)
{
    /* 对齐到页面边界 */
    size_t aligned_size = (size + 4095) & ~4095;

    void *virt = aligned_alloc(4096, aligned_size);
    if (!virt) return NULL;

    /* 配置内存属性为非缓存 (简化实现) */
    /* 实际实现需要MMU配置 */

    /* 返回物理地址 (假设1:1映射) */
    if (phys_addr) {
        *phys_addr = (uint64_t)virt;
    }

    return virt;
}

void dma_free_coherent(void *virt, size_t size)
{
    if (virt) {
        free(virt);
    }
}
