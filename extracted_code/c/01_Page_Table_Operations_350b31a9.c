/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 2071
 * Language: c
 * Block ID: 350b31a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：设置可写但不可读的页
 * 某些架构不支持此组合
 */
pte_t buggy_create_executable_page(struct page *page)
{
    pgprot_t prot = PAGE_SHARED;

    /* 错误：尝试创建可写但不可执行的页 */
    prot = pgprot_noncached(prot);  // 禁用缓存用于MMIO

    /* 错误：为数据页设置可执行权限 */
    return mk_pte(page, PAGE_KERNEL_EXEC);  // 应该用PAGE_KERNEL
}

/*
 * ✅ 正确：根据用途选择合适的页属性
 */
pte_t correct_create_page(struct page *page, enum page_type type)
{
    pgprot_t prot;

    switch (type) {
    case PAGE_TYPE_DATA:
        /* 普通数据页 */
        prot = PAGE_KERNEL;
        break;

    case PAGE_TYPE_CODE:
        /* 代码页 - 只读可执行 */
        prot = PAGE_KERNEL_RX;
        break;

    case PAGE_TYPE_RODATA:
        /* 只读数据 */
        prot = PAGE_KERNEL_RO;
        break;

    case PAGE_TYPE_MMIO:
        /* MMIO内存 - 禁用缓存 */
        prot = pgprot_noncached(PAGE_KERNEL_NOCACHE);
        break;

    case PAGE_TYPE_DEVICE:
        /* 设备内存 - 禁用缓存和预取 */
        prot = pgprot_writecombine(PAGE_KERNEL);
        break;

    default:
        prot = PAGE_KERNEL;
    }

    return mk_pte(page, prot);
}
