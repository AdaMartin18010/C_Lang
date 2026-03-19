/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\README.md
 * Line: 373
 * Language: c
 * Block ID: b519b503
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "pico/multicore.h"
#include "pico/stdlib.h"

// Core 1 入口函数
void core1_entry() {
    while (1) {
        // Core 1 处理任务
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();

    // 启动 Core 1
    multicore_launch_core1(core1_entry);

    // Core 0 主循环
    while (1) {
        // Core 0 处理任务
        sleep_ms(500);
    }
}
