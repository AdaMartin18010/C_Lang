/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\02_Page_Table_Walker.md
 * Line: 62
 * Language: c
 * Block ID: f83c5a78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 软件页表遍历 vs 硬件页表遍历对比
 */

/*
 * 方案1: 软件页表遍历 (早期系统如MIPS R3000)
 * 问题: TLB未命中时触发异常，由软件遍历页表
 */
void software_page_table_walk(uintptr_t vaddr, int access_type) {
    // 1. 保存寄存器状态
    save_context();

    // 2. 从专用寄存器读取页表基址
    uintptr_t pgd_base = read_c0_context();

    // 3. 软件遍历多级页表 (可能触发更多异常!)
    pgd_t *pgd = pgd_offset(pgd_base, vaddr);
    if (!pgd_present(*pgd)) {
        handle_page_fault(vaddr, access_type);
        return;
    }

    pud_t *pud = pud_offset(pgd, vaddr);
    if (!pud_present(*pud)) {
        handle_page_fault(vaddr, access_type);
        return;
    }

    pmd_t *pmd = pmd_offset(pud, vaddr);
    if (!pmd_present(*pmd)) {
        handle_page_fault(vaddr, access_type);
        return;
    }

    pte_t *pte = pte_offset(pmd, vaddr);
    if (!pte_present(*pte)) {
        handle_page_fault(vaddr, access_type);
        return;
    }

    // 4. 更新TLB (写TLB寄存器)
    write_c0_entryhi(vaddr & PAGE_MASK);
    write_c0_entrylo(pte_val(*pte));
    tlb_write_index();

    // 5. 恢复上下文并返回
    restore_context();
    eret();  // 异常返回
}

/*
 * 软件遍历的问题:
 * 1. 异常处理开销: 数十到数百个周期
 * 2. 页表访问可能触发更多TLB未命中 (嵌套异常)
 * 3. 需要保存/恢复完整上下文
 * 4. 代码复杂，容易出错
 */

/*
 * 方案2: 硬件页表遍历 (现代系统如x86-64, ARM64)
 * 优势: 硬件自动完成遍历，对软件透明
 */
// 无需软件介入！硬件自动完成:
// 1. 检测到TLB未命中
// 2. 硬件状态机自动遍历页表
// 3. 更新TLB
// 4. 重试原始内存访问
// 总延迟: 约10-100个周期 (取决于内存延迟)
