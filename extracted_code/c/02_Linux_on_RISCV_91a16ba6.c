/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 208
 * Language: c
 * Block ID: 91a16ba6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// arch/riscv/kernel/setup.c

void __init setup_arch(char **cmdline_p)
{
    // 1. 解析设备树
    parse_dtb();

    // 2. 初始化内存
    setup_bootmem();
    paging_init();

    // 3. 初始化SBI接口
    sbi_init();

    // 4. 初始化CPU特性
    riscv_fill_hwcap();

    // 5. 初始化中断
    init_IRQ();

    // 6. 初始化计时器
    time_init();

    // 7. 初始化控制台
    console_init();

    // 8. 初始化缓存信息
    riscv_init_caches();
}

// 设备树解析
static void __init parse_dtb(void)
{
    if (early_init_dt_scan(dtb_early_va)) {
        // 成功解析设备树
        pr_info("Machine model: %s\n", of_flat_dt_get_machine_name());
    } else {
        // 回退到默认配置
        pr_warn("No DTB passed to kernel!\n");
    }
}
