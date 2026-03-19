/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 140
 * Language: c
 * Block ID: b33a4bf0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OpenSBI关键代码分析
// lib/sbi/sbi_init.c

static void __noreturn init_coldboot(struct sbi_scratch *scratch, u32 hartid)
{
    // 1. 初始化平台
    rc = sbi_platform_early_init(plat, FALSE);

    // 2. 初始化控制台
    rc = sbi_console_init(scratch);

    // 3. 初始化中断控制器
    rc = sbi_irqchip_init(scratch, TRUE);

    // 4. 初始化IP核
    rc = sbi_ipi_init(scratch, TRUE);

    // 5. 初始化计时器
    rc = sbi_timer_init(scratch, TRUE);

    // 6. 初始化平台finalize
    rc = sbi_platform_final_init(plat, FALSE);

    // 7. 启动下一个阶段 (Linux)
    sbi_hart_switch_mode(hartid, scratch->next_arg1,
                         scratch->next_addr, scratch->next_mode, FALSE);
}
