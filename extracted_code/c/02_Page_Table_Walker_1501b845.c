/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\02_Page_Table_Walker.md
 * Line: 812
 * Language: c
 * Block ID: 1501b845
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 硬件遍历 vs 软件遍历性能对比
 * 假设：4级页表，每个页表项在单独缓存行
 */

/*
 * 硬件页表遍历延迟分析
 */
void hardware_walk_timing(void) {
    /*
     * 场景1: 所有页表项在L1缓存中
     * - 每个内存访问: 4-5周期
     * - 4级页表需要4次读取
     * - 额外开销: 状态机、地址计算
     * 总延迟: 约20-30周期
     */
    int cycles_best_case = 25;

    /*
     * 场景2: 页表项在L2缓存中
     * - 每个内存访问: 12-15周期
     * - 4次读取
     * 总延迟: 约50-70周期
     */
    int cycles_l2_case = 60;

    /*
     * 场景3: 页表项在L3缓存中
     * - 每个内存访问: 30-40周期
     * - 4次读取
     * 总延迟: 约120-160周期
     */
    int cycles_l3_case = 140;

    /*
     * 场景4: 页表项需要从内存读取
     * - 每个内存访问: 100-300周期
     * - 4次读取 (可能部分并行)
     * 总延迟: 约200-500周期
     */
    int cycles_mem_case = 350;

    printf("硬件遍历延迟:\n");
    printf("  L1缓存命中: %d 周期\n", cycles_best_case);
    printf("  L2缓存命中: %d 周期\n", cycles_l2_case);
    printf("  L3缓存命中: %d 周期\n", cycles_l3_case);
    printf("  内存访问:   %d 周期\n", cycles_mem_case);
}

/*
 * 软件页表遍历延迟分析
 */
void software_walk_timing(void) {
    /*
     * 场景: TLB未命中触发TLB缺失异常
     *
     * 1. 异常入口开销:
     *    - 保存上下文 (16-32个寄存器) : 16-32周期
     *    - 切换堆栈、特权级            : 5-10周期
     *    - 跳转到异常处理程序          : 5-10周期
     *    小计: 约30-50周期
     *
     * 2. 软件遍历开销:
     *    - 读取页表基址                : 1-5周期
     *    - 遍历4级页表 (每次读取)      : 4 × (5-100) = 20-400周期
     *    - 可能触发额外的TLB未命中!
     *    小计: 约30-500周期
     *
     * 3. 异常退出开销:
     *    - 恢复上下文                  : 16-32周期
     *    - 返回原程序                  : 5-10周期
     *    小计: 约25-45周期
     *
     * 总延迟: 约85-600周期 (不包含可能的嵌套TLB未命中)
     */
    int cycles_min = 85;
    int cycles_typical = 300;
    int cycles_max = 1000;  // 包含嵌套TLB未命中

    printf("\n软件遍历延迟:\n");
    printf("  最佳情况:   %d 周期\n", cycles_min);
    printf("  典型情况:   %d 周期\n", cycles_typical);
    printf("  最坏情况:   %d+ 周期\n", cycles_max);
}

/*
 * 加速比计算
 */
void calculate_speedup(void) {
    /*
     * 硬件遍历平均: 60周期
     * 软件遍历平均: 300周期
     * 加速比: 5倍
     */
    double speedup = 300.0 / 60.0;
    printf("\n加速比: %.1fx\n", speedup);

    /*
     * 在现代多核系统中，TLB未命中更频繁
     * 硬件遍历的优势更加显著
     */
}
