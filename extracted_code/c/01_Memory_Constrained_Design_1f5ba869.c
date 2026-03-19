/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 373
 * Language: c
 * Block ID: 1f5ba869
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 内存使用监控工具
 * ============================================================================ */

/* memory_monitor.h */
#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include <stdint.h>
#include <stddef.h>

/* 内存统计信息 */
typedef struct {
    uint32_t flash_used;      /* Flash已使用 */
    uint32_t flash_total;     /* Flash总大小 */
    uint32_t ram_data;        /* .data段大小 */
    uint32_t ram_bss;         /* .bss段大小 */
    uint32_t stack_used;      /* 栈已使用 */
    uint32_t stack_total;     /* 栈总大小 */
    uint32_t heap_used;       /* 堆已使用 */
    uint32_t heap_total;      /* 堆总大小 */
    uint32_t ram_free;        /* 估计剩余RAM */
} memory_stats_t;

/* 获取内存统计 */
void memory_get_stats(memory_stats_t *stats);

/* 打印内存报告 */
void memory_print_report(void);

/* 检查栈溢出 */
uint32_t stack_get_free(void);
uint32_t stack_get_used(void);

/* 检查堆使用 (如果使用堆) */
uint32_t heap_get_free(void);

#endif /* MEMORY_MONITOR_H */

/* memory_monitor.c */
#include "memory_monitor.h"

/* 外部符号 - 由链接器脚本定义 */
extern uint32_t _stext, _etext;       /* Flash代码 */
extern uint32_t _sdata, _edata;       /* 初始化数据 */
extern uint32_t _sbss, _ebss;         /* 未初始化数据 */
extern uint32_t _sstack, _estack;     /* 栈范围 */
extern uint32_t end;                  /* 堆起始 (由链接器提供) */

#define STACK_CANARY 0xDEADBEEF

/* 初始化栈监控 - 填充魔数 */
void stack_monitor_init(void)
{
    /* 填充栈区域为已知模式 */
    uint32_t *p = &_sstack;
    while (p < &_estack) {
        *p++ = STACK_CANARY;
    }
}

/* 获取栈剩余空间 */
uint32_t stack_get_free(void)
{
    uint32_t *p = &_sstack;

    while (p < &_estack && *p == STACK_CANARY) {
        p++;
    }

    return (uint32_t)p - (uint32_t)&_sstack;
}

/* 获取栈已使用空间 */
uint32_t stack_get_used(void)
{
    uint32_t stack_total = (uint32_t)&_estack - (uint32_t)&_sstack;
    return stack_total - stack_get_free();
}

/* 获取堆信息 (简化版) */
uint32_t heap_get_free(void)
{
    /* 堆顶 - 需要跟踪brk位置 */
    extern void *_sbrk(intptr_t increment);

    char *heap_end = (char *)_sbrk(0);
    if (heap_end == (char *)-1) {
        return 0;
    }

    uint32_t heap_top = (uint32_t)heap_end;
    uint32_t stack_bottom = (uint32_t)&_estack - stack_get_used();

    if (stack_bottom > heap_top) {
        return stack_bottom - heap_top;
    }
    return 0;  /* 堆栈碰撞！ */
}

/* 获取完整内存统计 */
void memory_get_stats(memory_stats_t *stats)
{
    if (!stats) return;

    /* Flash使用 */
    stats->flash_used = (uint32_t)&_etext - (uint32_t)&_stext;
    stats->flash_total = 64 * 1024;  /* 根据实际芯片调整 */

    /* RAM静态使用 */
    stats->ram_data = (uint32_t)&_edata - (uint32_t)&_sdata;
    stats->ram_bss = (uint32_t)&_ebss - (uint32_t)&_sbss;

    /* 栈信息 */
    stats->stack_total = (uint32_t)&_estack - (uint32_t)&_sstack;
    stats->stack_used = stack_get_used();

    /* 堆信息 */
    stats->heap_total = stats->stack_total / 4;  /* 假设堆占栈空间的1/4 */
    stats->heap_used = stats->heap_total - heap_get_free();

    /* 估计剩余 */
    uint32_t static_ram = stats->ram_data + stats->ram_bss;
    stats->ram_free = stats->stack_total - static_ram - stats->stack_used;
}

/* 格式化数字到字符串 (避免printf) */
static void uint_to_str(uint32_t value, char *buf, uint8_t base)
{
    char temp[12];
    int i = 0;

    do {
        uint32_t digit = value % base;
        temp[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        value /= base;
    } while (value > 0);

    /* 反转 */
    int j;
    for (j = 0; j < i; j++) {
        buf[j] = temp[i - 1 - j];
    }
    buf[i] = '\0';
}

/* 打印内存报告 (使用UART或其他输出) */
extern void uart_puts(const char *str);
extern void uart_puthex(uint32_t val);

void memory_print_report(void)
{
    memory_stats_t stats;
    memory_get_stats(&stats);
    char buf[16];

    uart_puts("\r\n===== Memory Report =====\r\n");

    uart_puts("Flash: ");
    uint_to_str(stats.flash_used, buf, 10);
    uart_puts(buf);
    uart_puts(" / ");
    uint_to_str(stats.flash_total, buf, 10);
    uart_puts(buf);
    uart_puts(" bytes\r\n");

    uart_puts("Stack Used: ");
    uint_to_str(stats.stack_used, buf, 10);
    uart_puts(buf);
    uart_puts(" / ");
    uint_to_str(stats.stack_total, buf, 10);
    uart_puts(buf);
    uart_puts(" bytes\r\n");

    uart_puts("Static RAM: ");
    uint_to_str(stats.ram_data + stats->ram_bss, buf, 10);
    uart_puts(buf);
    uart_puts(" bytes\r\n");

    uart_puts("========================\r\n");
}
