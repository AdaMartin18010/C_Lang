/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 5102
 * Language: c
 * Block ID: 7bd0a1f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 迁移前: FreeRTOS直接寄存器访问 */
#define UART_BASE   0x40011000
#define UART_DR     (*(volatile uint32_t *)(UART_BASE + 0x04))

void uart_send(uint8_t data) {
    UART_DR = data;
}

/* 迁移后: Zephyr设备API */
const struct device *uart = DEVICE_DT_GET(DT_ALIAS(uart_0));

void uart_send_zephyr(uint8_t data) {
    uart_poll_out(uart, data);
}
