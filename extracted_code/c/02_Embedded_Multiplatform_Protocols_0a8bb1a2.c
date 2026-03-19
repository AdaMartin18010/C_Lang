/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 2342
 * Language: c
 * Block ID: 0a8bb1a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// RP2040 dormont/sleep模式
// 注意: RP2040没有真正的RTC，需要外部RTC或定时唤醒
// ============================================================

#include "hardware/clocks.h"
#include "hardware/rosc.h"
#include "hardware/xosc.h"
#include "hardware/scb.h"

// 进入睡眠，等待中断
void sleep_until_interrupt(void) {
    // 等待中断 (WFI)
    __wfi();
}

// 降低系统时钟以节省功耗
void reduce_clock_speed(void) {
    // 切换到ROS C (内部RC振荡器)
    clocks_init();

    // 设置更低的系统频率
    set_sys_clock_khz(48000, false);  // 48MHz
    // 或者更低
    // set_sys_clock_khz(12000, false);  // 12MHz
}

// 恢复全速
void restore_full_speed(void) {
    set_sys_clock_khz(133000, true);  // 133MHz
}
