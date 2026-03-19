/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\01_Malloc_Physics.md
 * Line: 503
 * Language: c
 * Block ID: 57f80af8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// madvise优化
#include <sys/mman.h>

// 释放内存给系统（但不unmap）
void malloc_madvise_free(void *ptr, size_t size) {
    // 告诉内核可以回收这些页面
    madvise(ptr, size, MADV_FREE);

    // 页面内容保留，但可被回收
    // 再次访问时，页面会重新分配（内容为0或原值，未定义）
}

// 顺序访问提示
void malloc_madvise_sequential(void *ptr, size_t size) {
    // 提示内核这些页面将顺序访问
    madvise(ptr, size, MADV_SEQUENTIAL);

    // 内核可能预取并减少缓存保持
}

// 随机访问提示
void malloc_madvise_random(void *ptr, size_t size) {
    madvise(ptr, size, MADV_RANDOM);

    // 内核禁用预取
}

// 巨大页支持
void* malloc_hugepage(size_t size) {
    size_t hugepage_size = 2 * 1024 * 1024;  // 2MB

    // 对齐到巨大页边界
    size = ALIGN_UP(size, hugepage_size);

    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                     -1, 0);

    if (mem == MAP_FAILED) {
        // 回退到透明巨大页
        mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        madvise(mem, size, MADV_HUGEPAGE);
    }

    return mem;
}
