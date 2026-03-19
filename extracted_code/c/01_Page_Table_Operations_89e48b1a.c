/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1067
 * Language: c
 * Block ID: 89e48b1a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * TLB刷新操作 - 内核级实现
 */

# include <linux/mm.h>
# include <asm/tlbflush.h>
# include <asm/cpufeature.h>

/*
 * 刷新指定虚拟地址的TLB条目
 * 使用INVLPG指令（x86）
 */
static inline void __flush_tlb_one(unsigned long addr)
{
    asm volatile("invlpg (%0)" :: "r" (addr) : "memory");
}

/*
 * 刷新整个用户态地址空间的TLB
 * 通过重新加载CR3实现（会刷新非全局TLB条目）
 */
void flush_tlb_mm(struct mm_struct *mm)
{
    unsigned long cr3;

    /* 获取当前CR3值 */
    cr3 = __read_cr3();

    /* 如果PCID可用，使用PCID避免完全刷新 */
    if (cpu_feature_enabled(X86_FEATURE_PCID)) {
        /* 使用INVPCID指令 */
        invpcid_flush_single_context(mm->context.pcid);
    } else {
        /* 重新加载CR3，刷新非全局TLB */
        __write_cr3(cr3);
    }
}

/*
 * 刷新指定地址范围的TLB
 */
void flush_tlb_range(struct vm_area_struct *vma,
                     unsigned long start, unsigned long end)
{
    struct mm_struct *mm = vma->vm_mm;
    unsigned long addr;

    /* 如果范围较小，逐页刷新 */
    if ((end - start) >> PAGE_SHIFT < 32) {
        for (addr = start; addr < end; addr += PAGE_SIZE) {
            __flush_tlb_one(addr);
        }
    } else {
        /* 范围较大，刷新整个mm的TLB */
        flush_tlb_mm(mm);
    }
}

/*
 * 刷新指定虚拟地址的TLB（页级别）
 */
void flush_tlb_page(struct vm_area_struct *vma, unsigned long addr)
{
    struct mm_struct *mm = vma->vm_mm;

    /* 使用INVLPG指令 */
    __flush_tlb_one(addr);
}

/*
 * 刷新内核页表的TLB
 */
void flush_tlb_kernel_page(unsigned long addr)
{
    /* 内核页使用全局位，需要特殊处理 */
    if (cpu_feature_enabled(X86_FEATURE_PCID)) {
        /* 使用INVPCID刷新全局页 */
        invpcid_flush_all();
    } else {
        /* 禁用并重新启用CR4.PGE来刷新全局TLB */
        unsigned long cr4 = native_read_cr4();
        native_write_cr4(cr4 & ~X86_CR4_PGE);
        native_write_cr4(cr4 | X86_CR4_PGE);
    }
}

/*
 * 完全刷新所有TLB（包括全局条目）
 */
void flush_tlb_all(void)
{
    /* 通过修改CR4.PGE刷新所有TLB */
    unsigned long cr4 = native_read_cr4();
    native_write_cr4(cr4 & ~X86_CR4_PGE);
    native_write_cr4(cr4 | X86_CR4_PGE);
}

/*
 * 批量TLB刷新优化
 * 使用TLB gather减少刷新次数
 */
void tlb_gather_mmu(struct mmu_gather *tlb, struct mm_struct *mm)
{
    tlb->mm = mm;
    tlb->fullmm = 0;
    tlb->need_flush = 0;
    tlb->start = ~0UL;
    tlb->end = 0;
    tlb->freed_tables = 0;
}

void tlb_flush_mmu(struct mmu_gather *tlb)
{
    if (!tlb->need_flush)
        return;

    if (tlb->fullmm) {
        flush_tlb_mm(tlb->mm);
    } else if (tlb->end > tlb->start) {
        flush_tlb_range(NULL, tlb->start, tlb->end);
    }

    tlb->need_flush = 0;
    tlb->start = ~0UL;
    tlb->end = 0;
}

void tlb_finish_mmu(struct mmu_gather *tlb)
{
    tlb_flush_mmu(tlb);
}
