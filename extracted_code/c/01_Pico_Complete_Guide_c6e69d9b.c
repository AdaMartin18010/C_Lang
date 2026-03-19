/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 147
 * Language: c
 * Block ID: c6e69d9b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file boot_sequence.c
 * @brief RP2040 启动流程示例
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <stdio.h>

// Core 1 的入口函数
void core1_entry(void) {
    printf("Core 1 启动!\n");

    while (1) {
        // Core 1 任务
        sleep_ms(1000);
        printf("Core 1 运行中...\n");
    }
}

int main(void) {
    // 初始化标准库
    stdio_init_all();

    printf("Core 0 启动!\n");

    // 启动 Core 1
    multicore_launch_core1(core1_entry);

    // Core 0 主循环
    while (1) {
        printf("Core 0 运行中...\n");
        sleep_ms(500);
    }

    return 0;
}
