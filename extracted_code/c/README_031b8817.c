/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 299
 * Language: c
 * Block ID: 031b8817
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 加固库单元配置
typedef struct {
    int use_radhard_cells;      // 使用抗辐射单元库
    int use_guard_rings;        // 使用保护环
    int use_triple_well;        // 三阱工艺
    int use_epitaxial_layer;    // 外延层
} RadHardConfig;

// SEL 防护
void configure_sel_protection(void) {
    // 配置衬底抽头
    enable_substrate_taps();

    // 设置闩锁检测电路
    enable_latchup_detector();

    // 配置自动重启
    set_auto_restart_threshold(3);  // 3次检测后重启
}

// 电源监控
void power_monitor_task(void) {
    while (1) {
        float core_voltage = read_core_voltage();
        float current = read_supply_current();

        // SEL 检测：电流突增
        if (current > SEL_CURRENT_THRESHOLD) {
            trigger_sel_recovery();
        }

        // 欠压检测
        if (core_voltage < UVLO_THRESHOLD) {
            trigger_safe_mode();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
