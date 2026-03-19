/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1898
 * Language: c
 * Block ID: 9db43606
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：修改页表后缺少内存屏障
 * 在弱序架构（ARM）上可能导致其他CPU看到乱序的页表更新
 */
void buggy_set_pte(pte_t *ptep, pte_t pte)
{
    *ptep = pte;  // 危险：缺少内存屏障
    /* 其他CPU可能先看到新的PTE值，后看到其他相关数据 */
}

/*
 * ✅ 正确：使用适当的内存屏障
 */
void correct_set_pte(struct mm_struct *mm, unsigned long addr,
                     pte_t *ptep, pte_t pte)
{
    /* x86: 使用set_pte_at，内部包含隐式屏障 */
    set_pte_at(mm, addr, ptep, pte);

    /* 或显式使用内存屏障 */
    smp_wmb();  /* 写内存屏障 */
    *ptep = pte;

    /* 确保PTE更新对其他CPU可见 */
    smp_mb();   /* 全内存屏障 */
}

/*
 * ARM64特定屏障需求
 */
void arm64_set_pte(pte_t *ptep, pte_t pte)
{
    /* ARM64: PTE更新需要DSB（数据同步屏障） */
    smp_mb();
    WRITE_ONCE(*ptep, pte);
    dsb(ishst);  /* 内部共享域，存储操作 */
    isb();       /* 指令同步屏障 */
}
