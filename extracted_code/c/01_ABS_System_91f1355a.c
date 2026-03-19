/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 3182
 * Language: c
 * Block ID: 91f1355a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * TRAP 08: 违反ISO 26262功能安全要求
 *============================================================================*/

/* ❌ 严重违规：单点故障可能导致失控 */
/* 单核处理器，无安全监控 */
void bad_functional_safety(void) {
    /* 单核执行所有控制 */
    abs_control();

    /* 单路看门狗 */
    feed_watchdog();  /* 软件喂狗 */
}

/* ✅ 符合ASIL-D的设计 */
void good_functional_safety(void) {
    /* 1. 双核锁步CPU */
    /* 主核执行控制，从核执行相同计算并比较 */

    /* 2. 多路独立看门狗 */
    /* 内部看门狗 + 外部看门狗 + 窗口看门狗 */

    /* 3. 内存保护 */
    /* ECC校验RAM */
    /* MPU内存保护单元 */

    /* 4. 安全监控器 */
    safety_monitor_task();  /* 独立任务监控主控制流 */

    /* 5. 安全通信 */
    /* E2E保护关键信号 */

    /* 6. 故障处理 */
    if (safety_violation_detected()) {
        enter_safe_state();  /* 安全状态 */
    }
}
