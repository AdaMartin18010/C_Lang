/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 4036
 * Language: c
 * Block ID: 25725cfa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file runtime_stats.c
 * @brief 运行时统计信息
 */

#include "pico/stdlib.h"
#include <stdio.h>

void print_runtime_stats(void) {
    printf("\n=== 运行时统计 ===\n");

    // 系统时钟
    uint32_t sys_clk = clock_get_hz(clk_sys);
    printf("系统时钟: %u MHz\n", sys_clk / 1000000);

    // USB 时钟
    uint32_t usb_clk = clock_get_hz(clk_usb);
    printf("USB 时钟: %u MHz\n", usb_clk / 1000000);

    // ADC 时钟
    uint32_t adc_clk = clock_get_hz(clk_adc);
    printf("ADC 时钟: %u MHz\n", adc_clk / 1000000);

    // RTC 时钟
    uint32_t rtc_clk = clock_get_hz(clk_rtc);
    printf("RTC 时钟: %u Hz\n", rtc_clk);

    // 温度
    adc_select_input(4);
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096.0f;
    float temp = 27.0f - (voltage - 0.706f) / 0.001721f;
    printf("芯片温度: %.1f C\n", temp);

    // 电压
    printf("\n电压调节器:\n");
    printf("  DVDD: 由内部调节\n");
    printf("  USB_VDD: 3.3V\n");
    printf("  ADC_AVDD: 3.3V\n");

    printf("=================\n\n");
}
