/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\README.md
 * Line: 458
 * Language: c
 * Block ID: a1512224
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// memory_monitor.c
#include <stdint.h>

extern uint32_t _sdata, _edata;    // 数据段
extern uint32_t _sbss, _ebss;      // BSS 段
extern uint32_t _estack;           // 栈底

#define STACK_CANARY 0xDEADBEEF

void stack_monitor_init(void) {
    uint32_t *p = (uint32_t *)&_ebss;
    while (p < &_estack) {
        *p++ = STACK_CANARY;
    }
}

uint32_t stack_get_free(void) {
    uint32_t *p = (uint32_t *)&_ebss;
    while (p < &_estack && *p == STACK_CANARY) {
        p++;
    }
    return (uint32_t)p - (uint32_t)&_ebss;
}

void memory_print_stats(void) {
    uint32_t data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint32_t bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    uint32_t stack_used = ((uint32_t)&_estack - (uint32_t)&_ebss) - stack_get_free();

    // 通过 UART 或其他方式输出
    printf("RAM Usage:\r\n");
    printf("  .data: %lu bytes\r\n", data_size);
    printf("  .bss:  %lu bytes\r\n", bss_size);
    printf("  stack: %lu bytes used\r\n", stack_used);
}
