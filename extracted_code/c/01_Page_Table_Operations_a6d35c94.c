/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 2280
 * Language: c
 * Block ID: a6d35c94
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 使用全局页减少TLB刷新
 */
void optimize_kernel_tlb(void)
{
    pte_t *pte;

    /* 内核代码页使用全局页 */
    pte = pte_offset_kernel(pmd, address);
    set_pte(pte, pte_mkglobal(pte_val(*pte)));

    /* 刷新全局TLB条目 */
    flush_tlb_kernel_page(address);
}

/*
 * PCID使用示例
 */
void switch_mm_with_pcid(struct mm_struct *prev, struct mm_struct *next)
{
    u16 pcid = next->context.pcid;

    /* 如果PCID可用，使用INVPCID代替完全刷新 */
    if (cpu_feature_enabled(X86_FEATURE_PCID)) {
        /* 仅刷新当前PCID的非全局TLB */
        invpcid_flush_single_context(pcid);
    } else {
        /* 回退到CR3切换 */
        load_cr3(__pa(next->pgd));
    }
}
