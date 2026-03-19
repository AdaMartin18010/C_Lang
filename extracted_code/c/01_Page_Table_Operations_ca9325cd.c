/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 2186
 * Language: c
 * Block ID: ca9325cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：频繁的小页分配导致内存碎片
 * 影响大页分配成功率
 */
void buggy_fragment_memory(struct mm_struct *mm)
{
    int i;
    unsigned long addr;

    /* 随机分配4KB页，导致内存碎片化 */
    for (i = 0; i < 10000; i++) {
        addr = get_unmapped_area(NULL, 0, PAGE_SIZE, 0, 0);
        do_mmap(NULL, addr, PAGE_SIZE, PROT_READ|PROT_WRITE,
                MAP_ANONYMOUS|MAP_PRIVATE, 0);
    }
    /* 内存现在高度碎片化，后续大页分配失败 */
}

/*
 * ✅ 正确：使用THP或大页减少碎片
 */
void correct_manage_memory(struct mm_struct *mm)
{
    int i;
    unsigned long addr;

    /* 1. 启用透明大页 */
    set_bit(TRANSPARENT_HUGEPAGE_FLAG, &transparent_hugepage_flags);

    /* 2. 预分配大内存块 */
    addr = get_unmapped_area(NULL, 0, HPAGE_PMD_SIZE * 10,
                             HPAGE_PMD_SIZE, 0);
    do_mmap(NULL, addr, HPAGE_PMD_SIZE * 10,
            PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, 0);

    /* 3. 使用madvise提示内核使用大页 */
    madvise((void *)addr, HPAGE_PMD_SIZE * 10, MADV_HUGEPAGE);

    /* 4. 定期整理内存 */
    for (i = 0; i < 10; i++) {
        /* 使用连续的2MB块 */
        memset((void *)(addr + i * HPAGE_PMD_SIZE), 0, HPAGE_PMD_SIZE);
    }
}

/*
 * ❌ 错误：长时间持有大块内存但不使用
 */
void buggy_hold_memory(struct mm_struct *mm)
{
    /* 分配1GB内存但不访问 */
    unsigned long addr = vm_mmap(NULL, 0, 1UL << 30,
                                  PROT_READ|PROT_WRITE,
                                  MAP_ANONYMOUS|MAP_PRIVATE, 0);

    /* 长时间不访问，页被swap出去，内存浪费 */
    sleep(3600);
}

/*
 * ✅ 正确：使用懒分配和按需分配
 */
void correct_lazy_allocate(struct mm_struct *mm)
{
    /* 仅预留虚拟地址空间，物理页按需分配 */
    unsigned long addr = vm_mmap(NULL, 0, 1UL << 30,
                                  PROT_READ|PROT_WRITE,
                                  MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE, 0);

    /* 仅在需要时访问对应页，触发缺页分配 */
    /* 使用madvise控制内存行为 */
    madvise((void *)addr, 1UL << 30, MADV_DONTDUMP);
    madvise((void *)addr, 1UL << 30, MADV_HUGEPAGE);
}
