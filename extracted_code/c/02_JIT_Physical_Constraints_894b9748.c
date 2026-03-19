/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\02_JIT_Physical_Constraints.md
 * Line: 163
 * Language: c
 * Block ID: 894b9748
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * JIT代码生成产生大量小页面映射：
 * - 每个代码块可能需要单独页面
 * - TLB（转译后备缓冲区）条目有限
 * - TLB未命中代价高（页表遍历）
 *
 * 解决方案：使用巨大页（hugepages）
 */

// Linux巨大页支持
#include <sys/mman.h>

void* jit_alloc_hugepage(size_t size) {
    // 尝试分配2MB巨大页
    void *mem = mmap(NULL, size,
                     PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                     -1, 0);

    if (mem == MAP_FAILED) {
        // 回退到透明巨大页
        mem = mmap(NULL, size,
                   PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS,
                   -1, 0);

        // 建议内核使用透明巨大页
        madvise(mem, size, MADV_HUGEPAGE);
    }

    return mem;
}

// TLB友好的代码块分配
#define HUGE_PAGE_SIZE (2 * 1024 * 1024)  // 2MB
#define TLB_ENTRIES 64  // 典型TLB大小

typedef struct {
    void *huge_pages[HUGE_PAGE_POOL_SIZE];
    int num_huge_pages;

    // 当前页使用情况
    void *current_page;
    size_t current_offset;
} TLBFriendlyAllocator;

void* tlb_friendly_alloc(TLBFriendlyAllocator *alloc, size_t size) {
    // 对齐到64字节（缓存行）
    size = (size + 63) & ~63;

    // 检查是否适合当前页
    if (alloc->current_offset + size > HUGE_PAGE_SIZE) {
        // 分配新巨大页
        alloc->current_page = jit_alloc_hugepage(HUGE_PAGE_SIZE);
        alloc->current_offset = 0;
    }

    void *result = (char*)alloc->current_page + alloc->current_offset;
    alloc->current_offset += size;

    return result;
}

// 计算TLB覆盖范围
double calculate_tlb_coverage(TLBFriendlyAllocator *alloc) {
    // 每个TLB条目映射2MB
    size_t tlb_coverage = TLB_ENTRIES * HUGE_PAGE_SIZE;

    // 当前代码大小
    size_t code_size = alloc->num_huge_pages * HUGE_PAGE_SIZE;

    // TLB命中率估计
    return (double)tlb_coverage / code_size;
}
