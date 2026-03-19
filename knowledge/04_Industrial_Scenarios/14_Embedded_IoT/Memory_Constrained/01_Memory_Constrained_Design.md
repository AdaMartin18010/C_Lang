# 内存受限系统开发指南

> 针对RAM < 64KB嵌入式系统的极致优化技术 - 8位/16位/32位MCU完整开发手册

---

## 目录

- [内存受限系统开发指南](#内存受限系统开发指南)
  - [目录](#目录)
  - [1. 内存受限系统概述](#1-内存受限系统概述)
    - [1.1 什么是内存受限系统](#11-什么是内存受限系统)
    - [1.2 典型硬件平台对比](#12-典型硬件平台对比)
    - [1.3 内存使用分布分析](#13-内存使用分布分析)
    - [1.4 内存受限编程的核心原则](#14-内存受限编程的核心原则)
  - [2. 内存布局优化](#2-内存布局优化)
    - [2.1 链接器脚本优化](#21-链接器脚本优化)
    - [2.2 启动代码优化](#22-启动代码优化)
    - [2.3 内存区域监控](#23-内存区域监控)
  - [3. 静态内存分配策略](#3-静态内存分配策略)
    - [3.1 固定块内存池](#31-固定块内存池)
    - [3.2 对象池](#32-对象池)
    - [3.3 环形缓冲区](#33-环形缓冲区)
  - [4. 栈vs堆使用指南](#4-栈vs堆使用指南)
    - [4.1 栈的使用最佳实践](#41-栈的使用最佳实践)
    - [4.2 堆的使用风险与避免](#42-堆的使用风险与避免)
    - [4.3 对齐和填充优化](#43-对齐和填充优化)
  - [5. 代码体积优化技巧](#5-代码体积优化技巧)
    - [5.1 编译器优化选项](#51-编译器优化选项)
    - [5.2 精简库函数实现](#52-精简库函数实现)
    - [5.3 条件编译与功能裁剪](#53-条件编译与功能裁剪)
  - [6. 数据结构压缩](#6-数据结构压缩)
    - [6.1 数据打包技术](#61-数据打包技术)
    - [6.2 压缩算法实现](#62-压缩算法实现)
  - [7. 位域使用最佳实践](#7-位域使用最佳实践)
    - [7.1 位域基础与陷阱](#71-位域基础与陷阱)
    - [7.2 实用位操作库](#72-实用位操作库)
  - [8. 精简printf实现](#8-精简printf实现)
    - [8.1 迷你printf实现](#81-迷你printf实现)
  - [9. 中断安全编程](#9-中断安全编程)
    - [9.1 中断安全内存管理](#91-中断安全内存管理)
  - [10. 实际项目：传感器节点固件](#10-实际项目传感器节点固件)
    - [10.1 完整传感器节点实现](#101-完整传感器节点实现)
  - [11. 功耗优化](#11-功耗优化)
    - [11.1 睡眠模式管理](#111-睡眠模式管理)
    - [11.2 电池管理](#112-电池管理)
  - [12. 调试与监控工具](#12-调试与监控工具)
    - [12.1 轻量级日志系统](#121-轻量级日志系统)
  - [参考资源](#参考资源)
  - [总结](#总结)

---

## 1. 内存受限系统概述

### 1.1 什么是内存受限系统

内存受限系统通常指RAM容量小于64KB的嵌入式设备，广泛应用于：

- 物联网传感器节点
- 可穿戴设备
- 工业控制器
- 智能家居设备
- 汽车电子模块

### 1.2 典型硬件平台对比

| 平台 | RAM | Flash | 特点 | 典型应用 |
|:-----|:----|:------|:-----|:---------|
| ATmega328P | 2KB | 32KB | 8位AVR，低功耗 | Arduino Uno |
| ATtiny85 | 512B | 8KB | 超小体积 | 简单控制器 |
| STM32F030 | 4KB | 16KB | Cortex-M0 | 低端控制 |
| STM32F103C8 | 20KB | 64KB | Cortex-M3 | Blue Pill |
| STM32F072 | 16KB | 128KB | USB支持 | USB设备 |
| ESP8266 | 80KB | 4MB | WiFi集成 | WiFi模组 |
| ESP8285 | 80KB | 1MB | 内置Flash | 紧凑型WiFi |
| nRF51822 | 16KB | 256KB | BLE 4.0 | 蓝牙设备 |
| nRF52832 | 64KB | 512KB | BLE 5.0 | 现代BLE |
| MSP430G2553 | 512B | 16KB | 超低功耗 | 传感器节点 |
| MSP430FR5969 | 2KB | 64KB | FRAM | 数据记录 |
| PIC16F877A | 368B | 14KB | 经典8位 | 工业控制 |
| PIC18F4550 | 2KB | 32KB | USB支持 | 通用控制 |

### 1.3 内存使用分布分析

```
典型RAM布局 (以STM32F103 20KB为例):

低地址
0x20000000 ┌─────────────────────┐ ◄── _sdata (数据段起始)
           │    已初始化数据     │      .data段 (全局变量初值)
           │   (全局/静态变量)   │      从Flash复制
0x20000400 ├─────────────────────┤ ◄── _edata
           │   未初始化数据      │      .bss段 (零初始化)
           │      (BSS)          │      启动时清零
0x20000800 ├─────────────────────┤ ◄── _ebss / __bss_end__
           │                     │
           │      堆 (Heap)      │  动态分配区域
           │    (向上增长)       │  malloc/free使用
           │                     │
           ├─────────────────────┤ ◄── _sbrk管理的break指针
           │                     │
           │                     │
           ├─────────────────────┤ ◄── 栈顶指针(初始)
           │      栈 (Stack)     │  函数调用、局部变量
           │    (向下增长)       │  中断上下文保存
           │                     │
0x20005000 └─────────────────────┘ ◄── _estack (栈底/最高地址)

使用比例建议:
- 数据段(.data): 5-10%
- BSS段(.bss): 10-20%
- 堆区: 10-20% (尽量避免使用)
- 栈区: 30-50% (预留足够空间)
- 空闲: 10-20% (安全缓冲)
```

### 1.4 内存受限编程的核心原则

```c
/* ============================================================================
 * 内存受限系统编程黄金法则
 * ============================================================================ */

/* 原则1: 避免动态内存分配 */
/* 错误做法 */
void process_sensor_bad(void) {
    uint8_t *buffer = malloc(64);  /* 运行时分配 - 危险！ */
    if (buffer) {
        read_sensor(buffer, 64);
        free(buffer);  /* 容易遗漏，导致内存泄漏 */
    }
}

/* 正确做法 */
static uint8_t sensor_buffer[64];  /* 静态分配 */
void process_sensor_good(void) {
    read_sensor(sensor_buffer, 64);
    /* 自动管理，无需释放 */
}

/* 原则2: 使用最小足够的数据类型 */
#include <stdint.h>

/* 错误做法 - 浪费内存 */
int global_counter;           /* 可能是16或32位 */
long timestamp;               /* 32位可能足够 */

/* 正确做法 - 精确指定 */
uint8_t  small_counter;       /* 0-255范围 */
uint16_t medium_counter;      /* 0-65535范围 */
uint32_t large_counter;       /* 32位范围 */
int32_t  timestamp_sec;       /* Unix时间戳 */

/* 原则3: 常量数据放入Flash */
/* 错误做法 - 占用RAM */
const char error_msg[] = "Sensor Error\n";  /* 仍在RAM中！ */

/* 正确做法 - 留在Flash (GCC) */
const char error_msg_flash[] __attribute__((section(".rodata"))) = "Sensor Error\n";

/* AVR特定做法 */
#include <avr/pgmspace.h>
const char error_msg_avr[] PROGMEM = "Sensor Error\n";

/* 原则4: 栈上分配临时数据 */
void process_data(void) {
    /* 临时缓冲区放在栈上，函数返回自动释放 */
    uint8_t temp_buffer[32];

    read_data(temp_buffer, 32);
    process(temp_buffer, 32);
    /* temp_buffer自动释放，零开销 */
}

/* 原则5: 复用内存缓冲区 */
static union {
    uint8_t uart_buffer[32];
    uint8_t sensor_buffer[32];
    uint8_t packet_buffer[32];
} shared_buffers;
/* 同一时间只使用一个缓冲区，节省内存 */
```

---

## 2. 内存布局优化

### 2.1 链接器脚本优化

```c
/* ============================================================================
 * 自定义链接器脚本 - 精确控制内存布局
 * 针对: STM32F103C8 (20KB RAM, 64KB Flash)
 * ============================================================================ */

/* memory.ld */
MEMORY
{
    /* Flash - 只读 */
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 64K

    /* RAM - 读写 */
    RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 20K

    /* CCM/Core Coupled Memory (如果有) */
    CCM (rw)        : ORIGIN = 0x10000000, LENGTH = 0K  /* F103无CCM */
}

SECTIONS
{
    /* 代码段 - 放入Flash */
    .text :
    {
        . = ALIGN(4);
        _stext = .;           /* 代码起始 */
        KEEP(*(.isr_vector))  /* 中断向量表 */
        *(.text*)             /* 代码 */
        *(.rodata*)           /* 只读数据 */
        *(.glue_7)            /* ARM/Thumb glue */
        *(.glue_7t)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;           /* 代码结束 */
    } > FLASH

    /* 初始化数据 - 运行时放入RAM，加载时从Flash复制 */
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;           /* 数据起始 */
        *(.data*)
        . = ALIGN(4);
        _edata = .;           /* 数据结束 */
    } > RAM AT > FLASH

    /* 未初始化数据 - 运行时清零 */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;            /* BSS起始 */
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;            /* BSS结束 */
    } > RAM

    /* 栈底初始化值 */
    __stack_bottom = ORIGIN(RAM) + LENGTH(RAM);
}
```

### 2.2 启动代码优化

```c
/* ============================================================================
 * 精简启动代码 - 去除不必要的初始化
 * ============================================================================ */

/* startup.c */
#include <stdint.h>

/* 外部符号 - 由链接器定义 */
extern uint32_t _stext, _etext;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;
extern uint32_t __stack_bottom;

/* 主函数入口 */
extern int main(void);

/* 默认中断处理 - 死循环便于调试 */
void Default_Handler(void)
{
    while (1) {
        /* 在此设置断点可捕获未处理中断 */
        __asm volatile ("NOP");
    }
}

/* 弱定义中断处理程序 - 可被覆盖 */
void Reset_Handler(void)          __attribute__((weak, alias("Default_Handler")));
void NMI_Handler(void)            __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)      __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)            __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)         __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)        __attribute__((weak, alias("Default_Handler")));

/* 中断向量表 - 放入Flash */
__attribute__((section(".isr_vector")))
const void (*vector_table[])(void) = {
    (void (*)(void))&__stack_bottom,  /* 栈顶指针 */
    Reset_Handler,                    /* 复位 */
    NMI_Handler,                      /* NMI */
    HardFault_Handler,                /* Hard Fault */
    0, 0, 0, 0,                       /* 保留 */
    0, 0, 0,                          /* 保留 */
    SVC_Handler,                      /* SVCall */
    0, 0,                             /* 保留 */
    PendSV_Handler,                   /* PendSV */
    SysTick_Handler,                  /* SysTick */
    /* 外设中断... */
};

/* 快速内存复制 - 用于数据初始化 */
static inline void memcpy_fast(void *dst, const void *src, uint32_t n)
{
    uint32_t *d = dst;
    const uint32_t *s = src;

    /* 4字节对齐复制 */
    while (n >= 4) {
        *d++ = *s++;
        n -= 4;
    }

    /* 剩余字节 */
    uint8_t *d8 = (uint8_t *)d;
    const uint8_t *s8 = (const uint8_t *)s;
    while (n--) {
        *d8++ = *s8++;
    }
}

/* 快速内存清零 - 用于BSS初始化 */
static inline void memset_fast(void *dst, uint32_t n)
{
    uint32_t *d = dst;

    /* 4字节对齐清零 */
    while (n >= 4) {
        *d++ = 0;
        n -= 4;
    }

    /* 剩余字节 */
    uint8_t *d8 = (uint8_t *)d;
    while (n--) {
        *d8++ = 0;
    }
}

/* 复位处理程序 - 系统启动入口 */
void Reset_Handler(void)
{
    /* 1. 复制初始化数据从Flash到RAM */
    uint32_t data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    if (data_size > 0) {
        memcpy_fast(&_sdata, &_sidata, data_size);
    }

    /* 2. 清零BSS段 */
    uint32_t bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    if (bss_size > 0) {
        memset_fast(&_sbss, bss_size);
    }

    /* 3. 调用系统初始化 (可选) */
    /* SystemInit(); */

    /* 4. 跳转到main */
    main();

    /* 5. main不应返回，但如果返回则进入死循环 */
    while (1);
}
```

### 2.3 内存区域监控

```c
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
```

---

## 3. 静态内存分配策略

### 3.1 固定块内存池

```c
/* ============================================================================
 * 固定块内存池 - 无碎片、确定性的内存分配
 * ============================================================================ */

/* fixed_pool.h */
#ifndef FIXED_POOL_H
#define FIXED_POOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 内存池句柄 */
typedef struct {
    uint8_t *pool;            /* 内存池基地址 */
    uint8_t *used_map;        /* 使用位图 */
    uint16_t block_size;      /* 块大小 */
    uint16_t num_blocks;      /* 块数量 */
    uint16_t used_count;      /* 已使用块数 */
} fixed_pool_t;

/* 初始化内存池 */
bool fixed_pool_init(fixed_pool_t *pool, void *buffer, uint16_t buffer_size,
                     uint16_t block_size);

/* 分配内存 */
void *fixed_pool_alloc(fixed_pool_t *pool);

/* 释放内存 */
void fixed_pool_free(fixed_pool_t *pool, void *ptr);

/* 获取空闲块数 */
uint16_t fixed_pool_free_count(const fixed_pool_t *pool);

/* 检查指针是否属于该池 */
bool fixed_pool_contains(const fixed_pool_t *pool, const void *ptr);

#endif /* FIXED_POOL_H */

/* fixed_pool.c */
#include "fixed_pool.h"
#include <string.h>

/* 计算所需的位图大小 */
#define BITMAP_SIZE(n) (((n) + 7) / 8)

/* 设置位图中的位 */
static inline void bitmap_set(uint8_t *bitmap, uint16_t index)
{
    bitmap[index / 8] |= (1 << (index % 8));
}

/* 清除位图中的位 */
static inline void bitmap_clear(uint8_t *bitmap, uint16_t index)
{
    bitmap[index / 8] &= ~(1 << (index % 8));
}

/* 检查位图中的位 */
static inline bool bitmap_test(const uint8_t *bitmap, uint16_t index)
{
    return (bitmap[index / 8] & (1 << (index % 8))) != 0;
}

/* 初始化内存池 */
bool fixed_pool_init(fixed_pool_t *pool, void *buffer, uint16_t buffer_size,
                     uint16_t block_size)
{
    if (!pool || !buffer || block_size == 0) {
        return false;
    }

    /* 确保块大小至少为指针大小 (用于对齐) */
    if (block_size < sizeof(void *)) {
        block_size = sizeof(void *);
    }

    /* 计算可能的块数量 */
    /* 一部分空间用于位图，剩余用于数据 */
    uint16_t max_blocks = buffer_size / block_size;
    uint16_t bitmap_bytes = BITMAP_SIZE(max_blocks);

    /* 调整块数以容纳位图 */
    while ((max_blocks * block_size + bitmap_bytes) > buffer_size
           && max_blocks > 0) {
        max_blocks--;
        bitmap_bytes = BITMAP_SIZE(max_blocks);
    }

    if (max_blocks == 0) {
        return false;
    }

    /* 设置位图位置在池起始处 */
    pool->used_map = buffer;
    pool->pool = (uint8_t *)buffer + bitmap_bytes;
    pool->block_size = block_size;
    pool->num_blocks = max_blocks;
    pool->used_count = 0;

    /* 清零位图 */
    memset(pool->used_map, 0, bitmap_bytes);

    return true;
}

/* 分配内存 - O(n) 但无碎片 */
void *fixed_pool_alloc(fixed_pool_t *pool)
{
    if (!pool) return NULL;

    /* 查找空闲块 */
    for (uint16_t i = 0; i < pool->num_blocks; i++) {
        if (!bitmap_test(pool->used_map, i)) {
            bitmap_set(pool->used_map, i);
            pool->used_count++;
            return pool->pool + (i * pool->block_size);
        }
    }

    return NULL;  /* 无可用块 */
}

/* 释放内存 */
void fixed_pool_free(fixed_pool_t *pool, void *ptr)
{
    if (!pool || !ptr) return;

    /* 检查指针是否在池范围内 */
    uint8_t *p = ptr;
    if (p < pool->pool || p >= pool->pool + (pool->num_blocks * pool->block_size)) {
        return;  /* 非法指针 */
    }

    /* 检查对齐 */
    uint16_t offset = p - pool->pool;
    if (offset % pool->block_size != 0) {
        return;  /* 未对齐 */
    }

    uint16_t index = offset / pool->block_size;
    if (bitmap_test(pool->used_map, index)) {
        bitmap_clear(pool->used_map, index);
        pool->used_count--;
    }
}

/* 获取空闲块数 */
uint16_t fixed_pool_free_count(const fixed_pool_t *pool)
{
    if (!pool) return 0;
    return pool->num_blocks - pool->used_count;
}

/* 检查指针是否属于该池 */
bool fixed_pool_contains(const fixed_pool_t *pool, const void *ptr)
{
    if (!pool || !ptr) return false;

    const uint8_t *p = ptr;
    return (p >= pool->pool &&
            p < pool->pool + (pool->num_blocks * pool->block_size));
}

/* ============================================================================
 * 使用示例
 * ============================================================================ */

#ifdef EXAMPLE_USAGE

#include <stdio.h>

/* 定义池大小 */
#define POOL_BUFFER_SIZE 1024
#define BLOCK_SIZE 32
#define NUM_BLOCKS 28  /* ~1024 / 32，略少以容纳开销 */

/* 池缓冲区 */
static uint8_t pool_buffer[POOL_BUFFER_SIZE];
static fixed_pool_t my_pool;

/* 测试结构 */
typedef struct {
    uint16_t id;
    uint8_t data[24];
    uint8_t flags;
} packet_t;

void example_usage(void)
{
    /* 初始化池 */
    if (!fixed_pool_init(&my_pool, pool_buffer, POOL_BUFFER_SIZE, BLOCK_SIZE)) {
        printf("Pool init failed!\n");
        return;
    }

    printf("Pool: %d blocks of %d bytes\n",
           my_pool.num_blocks, my_pool.block_size);

    /* 分配 */
    packet_t *pkt1 = fixed_pool_alloc(&my_pool);
    packet_t *pkt2 = fixed_pool_alloc(&my_pool);

    if (pkt1 && pkt2) {
        pkt1->id = 1;
        pkt2->id = 2;

        printf("Allocated: pkt1=%p, pkt2=%p\n", (void*)pkt1, (void*)pkt2);
        printf("Free blocks: %d\n", fixed_pool_free_count(&my_pool));
    }

    /* 释放 */
    fixed_pool_free(&my_pool, pkt1);
    printf("After free: %d free blocks\n", fixed_pool_free_count(&my_pool));
}

#endif /* EXAMPLE_USAGE */
```

### 3.2 对象池

```c
/* ============================================================================
 * 类型安全对象池 - 针对特定数据结构优化
 * ============================================================================ */

/* object_pool.h */
#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <stdint.h>
#include <stdbool.h>

/* 对象池宏 - 生成类型安全的对象池 */
#define DEFINE_OBJECT_POOL(type, name, count) \
    static type name##_storage[count]; \
    static uint8_t name##_used[count] = {0}; \
    static const uint16_t name##_count = count; \
    \
    static inline type *name##_alloc(void) { \
        for (uint16_t i = 0; i < count; i++) { \
            if (!name##_used[i]) { \
                name##_used[i] = 1; \
                return &name##_storage[i]; \
            } \
        } \
        return NULL; \
    } \
    \
    static inline void name##_free(type *ptr) { \
        if (ptr >= &name##_storage[0] && ptr < &name##_storage[count]) { \
            name##_used[ptr - name##_storage] = 0; \
        } \
    } \
    \
    static inline uint16_t name##_free_count(void) { \
        uint16_t free = 0; \
        for (uint16_t i = 0; i < count; i++) { \
            if (!name##_used[i]) free++; \
        } \
        return free; \
    }

/* 显式对象池结构 */
#define OBJECT_POOL(type, count) \
    struct { \
        type storage[count]; \
        uint8_t used[count]; \
        const uint16_t capacity; \
        uint16_t used_count; \
    }

/* 初始化对象池 */
#define OBJECT_POOL_INIT(pool) \
    do { \
        memset((pool)->used, 0, sizeof((pool)->used)); \
        *(uint16_t*)&(pool)->capacity = sizeof((pool)->storage) / sizeof((pool)->storage[0]); \
        (pool)->used_count = 0; \
    } while(0)

#endif /* OBJECT_POOL_H */

/* object_pool_example.c */
#include "object_pool.h"
#include <string.h>
#include <stdio.h>

/* 定义传感器数据结构 */
typedef struct {
    uint16_t sensor_id;
    int16_t temperature;    /* 0.1度为单位 */
    uint16_t humidity;      /* 0.1%为单位 */
    uint32_t timestamp;
    uint8_t status;
    uint8_t battery_level;
} sensor_data_t;

/* 定义传感器对象池 - 最多8个传感器 */
DEFINE_OBJECT_POOL(sensor_data_t, sensor_pool, 8)

/* 定义消息缓冲区对象池 */
typedef struct {
    uint8_t data[64];
    uint8_t length;
    uint8_t type;
} message_buffer_t;

DEFINE_OBJECT_POOL(message_buffer_t, msg_pool, 4)

/* 使用示例 */
void sensor_pool_example(void)
{
    printf("Object Pool Example\n");
    printf("===================\n");

    /* 分配传感器对象 */
    sensor_data_t *sensor1 = sensor_pool_alloc();
    sensor_data_t *sensor2 = sensor_pool_alloc();

    if (sensor1 && sensor2) {
        /* 初始化数据 */
        sensor1->sensor_id = 0x1234;
        sensor1->temperature = 256;  /* 25.6度 */
        sensor1->humidity = 456;     /* 45.6% */
        sensor1->timestamp = 0;
        sensor1->status = 1;
        sensor1->battery_level = 85;

        sensor2->sensor_id = 0x5678;
        sensor2->temperature = 189;  /* 18.9度 */
        sensor2->humidity = 600;     /* 60.0% */

        printf("Sensor 1: ID=0x%04X, Temp=%.1fC, Hum=%.1f%%\n",
               sensor1->sensor_id, sensor1->temperature / 10.0,
               sensor1->humidity / 10.0);
        printf("Sensor 2: ID=0x%04X, Temp=%.1fC, Hum=%.1f%%\n",
               sensor2->sensor_id, sensor2->temperature / 10.0,
               sensor2->humidity / 10.0);

        printf("Free sensors: %d\n", sensor_pool_free_count());
    }

    /* 释放 */
    sensor_pool_free(sensor1);
    printf("After free: %d free sensors\n", sensor_pool_free_count());
}

/* 显式对象池使用 */
typedef struct sensor_node {
    uint16_t id;
    struct sensor_node *next;
    uint8_t data[20];
} sensor_node_t;

/* 定义节点池 */
static struct {
    sensor_node_t nodes[10];
    uint8_t used[10];
    uint16_t capacity;
    uint16_t used_count;
} node_pool;

void explicit_pool_init(void)
{
    memset(node_pool.used, 0, sizeof(node_pool.used));
    node_pool.capacity = 10;
    node_pool.used_count = 0;
}

sensor_node_t *node_alloc(void)
{
    for (uint16_t i = 0; i < node_pool.capacity; i++) {
        if (!node_pool.used[i]) {
            node_pool.used[i] = 1;
            node_pool.used_count++;
            memset(&node_pool.nodes[i], 0, sizeof(sensor_node_t));
            return &node_pool.nodes[i];
        }
    }
    return NULL;
}

void node_free(sensor_node_t *node)
{
    if (node >= &node_pool.nodes[0] &&
        node < &node_pool.nodes[node_pool.capacity]) {
        uint16_t index = node - node_pool.nodes;
        if (node_pool.used[index]) {
            node_pool.used[index] = 0;
            node_pool.used_count--;
        }
    }
}
```

### 3.3 环形缓冲区

```c
/* ============================================================================
 * 环形缓冲区 (Ring Buffer) - 高效的FIFO数据结构
 * ============================================================================ */

/* ring_buffer.h */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 环形缓冲区结构 */
typedef struct {
    uint8_t *buffer;          /* 缓冲区基地址 */
    uint16_t size;            /* 缓冲区大小 */
    volatile uint16_t head;   /* 写入位置 */
    volatile uint16_t tail;   /* 读取位置 */
    uint16_t count;           /* 当前元素数 */
} ring_buffer_t;

/* 初始化环形缓冲区 */
void rb_init(ring_buffer_t *rb, uint8_t *buffer, uint16_t size);

/* 清空缓冲区 */
void rb_reset(ring_buffer_t *rb);

/* 写入单个字节 */
bool rb_put(ring_buffer_t *rb, uint8_t data);

/* 读取单个字节 */
bool rb_get(ring_buffer_t *rb, uint8_t *data);

/* 批量写入 */
uint16_t rb_write(ring_buffer_t *rb, const uint8_t *data, uint16_t len);

/* 批量读取 */
uint16_t rb_read(ring_buffer_t *rb, uint8_t *data, uint16_t len);

/* 查看但不读取 */
bool rb_peek(ring_buffer_t *rb, uint8_t *data);

/* 获取可用空间 */
uint16_t rb_free(const ring_buffer_t *rb);

/* 获取已用空间 */
uint16_t rb_used(const ring_buffer_t *rb);

/* 检查是否为空 */
static inline bool rb_is_empty(const ring_buffer_t *rb) {
    return rb->count == 0;
}

/* 检查是否已满 */
static inline bool rb_is_full(const ring_buffer_t *rb) {
    return rb->count >= rb->size;
}

#endif /* RING_BUFFER_H */

/* ring_buffer.c */
#include "ring_buffer.h"

/* 初始化 */
void rb_init(ring_buffer_t *rb, uint8_t *buffer, uint16_t size)
{
    rb->buffer = buffer;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* 清空 */
void rb_reset(ring_buffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* 写入单个字节 */
bool rb_put(ring_buffer_t *rb, uint8_t data)
{
    if (rb->count >= rb->size) {
        return false;  /* 已满 */
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->size;
    rb->count++;
    return true;
}

/* 读取单个字节 */
bool rb_get(ring_buffer_t *rb, uint8_t *data)
{
    if (rb->count == 0) {
        return false;  /* 已空 */
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count--;
    return true;
}

/* 批量写入 */
uint16_t rb_write(ring_buffer_t *rb, const uint8_t *data, uint16_t len)
{
    uint16_t written = 0;

    while (written < len && rb->count < rb->size) {
        rb->buffer[rb->head] = data[written++];
        rb->head = (rb->head + 1) % rb->size;
        rb->count++;
    }

    return written;
}

/* 批量读取 */
uint16_t rb_read(ring_buffer_t *rb, uint8_t *data, uint16_t len)
{
    uint16_t read = 0;

    while (read < len && rb->count > 0) {
        data[read++] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;
        rb->count--;
    }

    return read;
}

/* 查看但不读取 */
bool rb_peek(ring_buffer_t *rb, uint8_t *data)
{
    if (rb->count == 0) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    return true;
}

/* 获取可用空间 */
uint16_t rb_free(const ring_buffer_t *rb)
{
    return rb->size - rb->count;
}

/* 获取已用空间 */
uint16_t rb_used(const ring_buffer_t *rb)
{
    return rb->count;
}

/* ============================================================================
 * 线程安全变体 (用于中断环境)
 * ============================================================================ */

/* 用于ISR的写入 - 中断安全版本 */
bool rb_put_isr(ring_buffer_t *rb, uint8_t data)
{
    /* 中断安全：只修改head和count */
    /* 假设只有一个生产者（中断）和一个消费者（主循环） */

    uint16_t next_head = (rb->head + 1) % rb->size;

    /* 检查是否已满 */
    if (next_head == rb->tail) {
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = next_head;
    /* count更新可能有竞态，但在此模型下可接受 */

    return true;
}

/* DMA友好的环形缓冲区 */
typedef struct {
    uint8_t *buffer;
    uint16_t size;
    volatile uint16_t write_idx;  /* 由生产者更新 */
    volatile uint16_t read_idx;   /* 由消费者更新 */
} dma_ring_buffer_t;

void dma_rb_init(dma_ring_buffer_t *rb, uint8_t *buffer, uint16_t size)
{
    rb->buffer = buffer;
    rb->size = size;
    rb->write_idx = 0;
    rb->read_idx = 0;
}

/* 获取连续可写空间 (用于DMA) */
uint16_t dma_rb_write_prepare(dma_ring_buffer_t *rb, uint8_t **ptr)
{
    uint16_t free;

    if (rb->write_idx >= rb->read_idx) {
        /* 写到缓冲区末尾 */
        free = rb->size - rb->write_idx;
        /* 如果有空间，也可以绕回到开头写 */
        if (rb->read_idx > 0) {
            free = rb->size - rb->write_idx;
        }
    } else {
        free = rb->read_idx - rb->write_idx - 1;
    }

    *ptr = &rb->buffer[rb->write_idx];
    return free;
}

/* 提交写入 (DMA完成后调用) */
void dma_rb_write_commit(dma_ring_buffer_t *rb, uint16_t len)
{
    rb->write_idx = (rb->write_idx + len) % rb->size;
}
```

---

## 4. 栈vs堆使用指南

### 4.1 栈的使用最佳实践

```c
/* ============================================================================
 * 栈使用指南 - 嵌入式系统的首选分配方式
 * ============================================================================ */

/* 栈的优点：
 * 1. 自动管理 - 函数返回自动释放
 * 2. 无碎片 - 后进先出保证连续性
 * 3. 确定性 - 编译时可计算最大用量
 * 4. 高性能 - 仅需调整SP寄存器
 */

/* 推荐：小数组和结构体使用栈 */
void process_packet_good(uint8_t packet_type)
{
    /* 小缓冲区放栈上 */
    uint8_t header[8];
    uint16_t crc;

    /* 处理数据 */
    header[0] = packet_type;
    header[1] = 0x01;
    crc = calculate_crc(header, 8);

    /* 自动释放 - 无需管理 */
}

/* 不推荐：大数组使用栈 */
void process_packet_bad(void)
{
    /* 危险！可能栈溢出 */
    uint8_t large_buffer[2048];  /* 2KB在栈上 */
    /* ... */
}

/* 栈大小估算 */
/* 每个函数调用需要：
 * - 返回地址: 4字节 (ARM)
 * - 保存的寄存器: 4-32字节
 * - 局部变量: 视情况而定
 * - 对齐填充: 0-3字节
 */

/* 中断栈使用计算示例 */
void calculate_stack_usage(void)
{
    /* 主循环最大调用深度:
     * main() -> loop() -> process() -> handle() = 4层
     * 每层 ~32字节 = 128字节
     */

    /* 中断最坏情况:
     * - 中断发生时主循环在深层调用
     * - 高优先级中断打断低优先级中断
     *
     * 假设3级中断嵌套:
     * - 保存上下文: 16-64字节/级
     * - 中断处理: 32-128字节/级
     * 总计: ~500字节
     */

    /* 建议栈大小 = 正常用量 * 2 + 中断用量
     * = 128 * 2 + 500 = 756字节最小
     * 实际建议分配 2-4KB
     */
}

/* 递归的替代方案 */
/* 不推荐：递归消耗不确定栈空间 */
int factorial_recursive(int n)
{
    if (n <= 1) return 1;
    return n * factorial_recursive(n - 1);
}

/* 推荐：迭代使用固定栈空间 */
int factorial_iterative(int n)
{
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/* 变长数组的替代 */
/* 不推荐：VLA使用不确定栈空间 */
void process_vla_bad(int size)
{
    uint8_t buffer[size];  /* VLA - 避免使用 */
    /* ... */
}

/* 推荐：使用最大固定大小 */
#define MAX_BUFFER_SIZE 128
void process_fixed_good(uint8_t size)
{
    uint8_t buffer[MAX_BUFFER_SIZE];

    if (size > MAX_BUFFER_SIZE) {
        size = MAX_BUFFER_SIZE;  /* 限制大小 */
    }
    /* ... */
}

/* 或分段处理 */
void process_in_chunks(const uint8_t *data, uint16_t total_size)
{
    uint8_t chunk[32];
    uint16_t processed = 0;

    while (processed < total_size) {
        uint16_t chunk_size = total_size - processed;
        if (chunk_size > sizeof(chunk)) {
            chunk_size = sizeof(chunk);
        }

        memcpy(chunk, data + processed, chunk_size);
        process_chunk(chunk, chunk_size);
        processed += chunk_size;
    }
}
```

### 4.2 堆的使用风险与避免

```c
/* ============================================================================
 * 堆使用风险 - 为什么内存受限系统应避免堆
 * ============================================================================ */

/* 风险1: 内存碎片 */
void fragmentation_demo(void)
{
    /* 初始: 空闲内存 100字节 [                  ]
     */

    void *a = malloc(30);  /* [AAAAAA            ] 分配30字节 */
    void *b = malloc(30);  /* [AAAAAABBBBBB      ] 分配30字节 */
    void *c = malloc(30);  /* [AAAAAABBBBBBCCCCCC] 分配30字节 */

    free(b);               /* [AAAAAA      CCCCCC] 释放B */
    /* 现在30字节空闲，但如果在A和C之间 */

    void *d = malloc(40);  /* 失败！虽然有30+30字节，但不连续 */

    free(a);
    free(c);
    free(d);
}

/* 风险2: 分配失败处理复杂 */
void* risky_malloc(size_t size)
{
    void *ptr = malloc(size);
    /* 错误! 分配失败返回NULL，调用者必须检查 */
    return ptr;
}

/* 风险3: 内存泄漏 */
void memory_leak_example(void)
{
    for (int i = 0; i < 100; i++) {
        uint8_t *buffer = malloc(64);
        /* 处理... */
        if (some_error_condition()) {
            return;  /* 错误! 内存泄漏 */
        }
        free(buffer);  /* 可能永远不会执行 */
    }
}

/* 风险4: 堆栈碰撞 */
/*
 * 堆向上增长，栈向下增长
 * 如果没有边界检查，可能相互覆盖
 */

/* ============================================================================
 * 安全的内存分配模式
 * ============================================================================ */

/* 模式1: 完全避免堆 - 静态分配 */
/* 编译时确定所有内存需求 */
static uint8_t uart_rx_buffer[64];
static uint8_t uart_tx_buffer[64];
static sensor_data_t sensor_buffers[4];
static message_t message_pool[8];

/* 模式2: 一次性分配 */
/* 启动时分配，运行时不释放 */
typedef struct {
    uint8_t *config_data;
    uint8_t *working_buffer;
    sensor_data_t *sensors;
} system_buffers_t;

static system_buffers_t sys_bufs;

bool system_init_buffers(void)
{
    /* 启动时一次性分配 */
    static uint8_t memory_arena[2048];  /* 静态内存区 */
    uint16_t offset = 0;

    sys_bufs.config_data = memory_arena + offset;
    offset += 256;

    sys_bufs.working_buffer = memory_arena + offset;
    offset += 512;

    sys_bufs.sensors = (sensor_data_t *)(memory_arena + offset);
    offset += 4 * sizeof(sensor_data_t);

    if (offset > sizeof(memory_arena)) {
        return false;  /* 内存不足 */
    }

    return true;
}

/* 模式3: 内存池 (见第3节) */
/* 使用固定块大小的内存池避免碎片 */

/* 模式4: 栈分配 */
/* 临时数据使用栈 */
void process_temp_data(void)
{
    uint8_t temp[128];  /* 栈分配 */

    read_data(temp, sizeof(temp));
    process(temp, sizeof(temp));
    /* 自动释放 */
}

/* 如果需要"持久"数据，复制到静态区 */
static uint8_t persistent_data[128];

void save_persistent_data(void)
{
    uint8_t temp[128];

    read_data(temp, sizeof(temp));
    memcpy(persistent_data, temp, sizeof(temp));
}

/* 模式5: 双缓冲 */
/* 两个缓冲区交替使用 */
static uint8_t buffer_a[256];
static uint8_t buffer_b[256];
static volatile uint8_t active_buffer = 0;

uint8_t* get_working_buffer(void)
{
    return active_buffer ? buffer_b : buffer_a;
}

uint8_t* get_backup_buffer(void)
{
    return active_buffer ? buffer_a : buffer_b;
}

void swap_buffers(void)
{
    active_buffer = !active_buffer;
}
```

### 4.3 对齐和填充优化

```c
/* ============================================================================
 * 内存对齐与填充 - 理解编译器的内存布局
 * ============================================================================ */

/* 结构体填充示例 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* 未优化的结构体 - 浪费空间 */
typedef struct {
    uint8_t  flag;        /* 1字节 + 3字节填充 */
    uint32_t timestamp;   /* 4字节 */
    uint8_t  status;      /* 1字节 + 1字节填充 */
    uint16_t value;       /* 2字节 */
} bad_layout_t;
/* 总计: 12字节 (浪费4字节填充) */

/* 优化后的结构体 - 按大小排序 */
typedef struct {
    uint32_t timestamp;   /* 4字节 */
    uint16_t value;       /* 2字节 */
    uint8_t  flag;        /* 1字节 */
    uint8_t  status;      /* 1字节 */
} good_layout_t;
/* 总计: 8字节 (无浪费) */

/* 编译器特定打包 */
/* GCC */
typedef struct __attribute__((packed)) {
    uint8_t  flag;
    uint32_t timestamp;   /* 未对齐访问可能慢 */
    uint8_t  status;
    uint16_t value;
} packed_gcc_t;
/* 总计: 8字节，但可能有性能损失 */

/* MSVC */
#pragma pack(push, 1)
typedef struct {
    uint8_t  flag;
    uint32_t timestamp;
    uint8_t  status;
    uint16_t value;
} packed_msvc_t;
#pragma pack(pop)

/* 验证大小 */
void check_sizes(void)
{
    printf("bad_layout_t:  %zu bytes\n", sizeof(bad_layout_t));
    printf("good_layout_t: %zu bytes\n", sizeof(good_layout_t));
    printf("packed_gcc_t:  %zu bytes\n", sizeof(packed_gcc_t));
}

/* 数组对齐优化 */
/* 32位DMA通常需要4字节对齐 */
struct {
    uint8_t small_data[10];   /* 10字节 */
    /* 编译器插入2字节填充 */
    uint32_t dma_buffer[16];  /* 64字节，4字节对齐 */
} aligned_example;

/* 显式对齐 */
__attribute__((aligned(4))) uint8_t dma_buffer[64];
__attribute__((aligned(8))) uint64_t cache_line_buffer[8];

/* 混合数据类型的优化布局 */
typedef struct {
    /* 第一组: 4字节对齐 */
    uint32_t id;
    uint32_t timestamp;

    /* 第二组: 2字节对齐 */
    uint16_t x;
    uint16_t y;
    uint16_t z;

    /* 第三组: 1字节 */
    uint8_t flags;
    uint8_t status;
    uint8_t reserved[2];  /* 填充到4字节边界 */
} sensor_optimized_t;
/* 总计: 16字节，无填充浪费 */

/* 位域与成员的混合使用 */
typedef struct {
    /* 位域打包到一个字节 */
    uint8_t is_active : 1;
    uint8_t has_error : 1;
    uint8_t power_mode : 2;
    uint8_t reserved : 4;

    /* 完整字节 */
    uint8_t battery_level;

    /* 2字节对齐 */
    uint16_t sample_rate;

    /* 4字节对齐 */
    uint32_t total_samples;
} mixed_optimized_t;
/* 总计: 8字节 */
```

---

## 5. 代码体积优化技巧

### 5.1 编译器优化选项

```c
/* ============================================================================
 * GCC编译器优化选项 - 最小化代码体积
 * ============================================================================ */

/* Makefile 优化配置 */
/*
# 优化级别 - 代码大小
CFLAGS += -Os                    # 优化代码大小 (不是-O2或-O3)
CFLAGS += -Oz                    # 极致大小优化 (GCC 4.8+)

# 去除未使用代码
CFLAGS += -ffunction-sections    # 每个函数放入独立段
CFLAGS += -fdata-sections        # 每个数据放入独立段
LDFLAGS += -Wl,--gc-sections     # 链接时删除未使用段

# 链接时优化
CFLAGS += -flto                  # 链接时优化
LDFLAGS += -flto

# 禁用不必要功能
CFLAGS += -fno-builtin-printf    # 不使用内置printf
CFLAGS += -fno-unwind-tables     # 禁用异常展开表
CFLAGS += -fno-asynchronous-unwind-tables
CFLAGS += -fno-exceptions        # C++禁用异常
CFLAGS += -fno-rtti              # C++禁用RTTI
CFLAGS += -fno-stack-protector   # 禁用栈保护
CFLAGS += -fno-ident             # 去除编译器标识

# ARM特定优化
CFLAGS += -mthumb                # Thumb指令 (更紧凑)
CFLAGS += -mcpu=cortex-m0        # 针对特定CPU优化
CFLAGS += -mlong-calls           # 允许长跳转

# 调试信息控制
CFLAGS += -g0                    # 无调试信息 (发布版本)
# 或 CFLAGS += -g3                # 完整调试信息 (调试版本)

# 警告选项 (帮助发现优化机会)
CFLAGS += -Wall -Wextra -Wunused
CFLAGS += -Wunused-function
CFLAGS += -Wunused-variable
CFLAGS += -Wunused-parameter

# 链接器优化
LDFLAGS += -Wl,--relax           # 优化跳转和调用
LDFLAGS += -Wl,--strip-all       # 去除所有符号
LDFLAGS += -Wl,--strip-debug     # 去除调试信息
LDFLAGS += -nostartfiles         # 自定义启动文件
LDFLAGS += -nodefaultlibs        # 不使用默认库
*/

/* 链接脚本优化 */
/*
SECTIONS
{
    .text :
    {
        *(.text.startup*)      /* 启动代码 */
        *(.text.interrupt*)    /* 中断处理 */
        *(.text*)              /* 其他代码 */

        /* 合并常量到.text */
        *(.rodata*)
        *(.rodata1)
    } > FLASH

    /* 去除.comment和.note段 */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.gnu.*)
    }
}
*/

/* 检查代码大小的方法 */
/*
# 查看各段大小
arm-none-eabi-size firmware.elf
   text    data     bss     dec     hex filename
   8456     256     512    9224    2408 firmware.elf

# 查看详细符号大小
arm-none-eabi-nm --print-size --size-sort --radix=d firmware.elf

# 生成内存映射
arm-none-eabi-ld -Map=firmware.map ...

# 反汇编查看代码
arm-none-eabi-objdump -d firmware.elf > firmware.asm
*/
```

### 5.2 精简库函数实现

```c
/* ============================================================================
 * 迷你字符串操作
 * ============================================================================ */

#include <stddef.h>

/* 自定义strlen - 小型化 */
size_t mini_strlen(const char *s)
{
    const char *p = s;
    while (*p) p++;
    return p - s;
}

/* 自定义strcpy */
char *mini_strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

/* 自定义memcpy - 对齐优化 */
void *mini_memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;

    /* 尝试4字节对齐复制 */
    if (!((uintptr_t)d & 3) && !((uintptr_t)s & 3)) {
        while (n >= 4) {
            *(uint32_t *)d = *(const uint32_t *)s;
            d += 4;
            s += 4;
            n -= 4;
        }
    }

    /* 剩余字节 */
    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

/* 自定义memset */
void *mini_memset(void *s, int c, size_t n)
{
    uint8_t *p = s;
    uint8_t val = c;

    /* 4字节对齐清零 */
    if (!((uintptr_t)p & 3) && n >= 4 && val == 0) {
        while (n >= 4) {
            *(uint32_t *)p = 0;
            p += 4;
            n -= 4;
        }
    }

    while (n--) {
        *p++ = val;
    }

    return s;
}

/* 自定义memcmp */
int mini_memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = s1;
    const uint8_t *p2 = s2;

    while (n--) {
        int diff = *p1++ - *p2++;
        if (diff) return diff;
    }
    return 0;
}

/* 快速整数转字符串 */
char *mini_itoa(int value, char *buf, int base)
{
    char *p = buf;
    char *q = buf;

    /* 处理负数 */
    if (value < 0 && base == 10) {
        *p++ = '-';
        value = -value;
    }

    /* 转换 */
    do {
        int digit = value % base;
        *p++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= base;
    } while (value);

    *p = '\0';

    /* 反转 */
    for (p--; q < p; q++, p--) {
        char t = *q;
        *q = *p;
        *p = t;
    }

    return buf;
}

/* 快速字符串转整数 */
int mini_atoi(const char *s)
{
    int sign = 1;
    int result = 0;

    /* 跳过空白 */
    while (*s == ' ' || *s == '\t') s++;

    /* 符号 */
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    /* 转换 */
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }

    return sign * result;
}
```

### 5.3 条件编译与功能裁剪

```c
/* ============================================================================
 * 条件编译配置 - 按需编译功能
 * ============================================================================ */

/* config.h - 系统配置文件 */
#ifndef CONFIG_H
#define CONFIG_H

/* 版本信息 */
#define FIRMWARE_VERSION_MAJOR 1
#define FIRMWARE_VERSION_MINOR 2
#define FIRMWARE_VERSION_PATCH 3

/* 功能开关 - 未使用的功能不编译 */
#define ENABLE_UART0            1   /* 串口0 */
#define ENABLE_UART1            0   /* 串口1 - 节省空间 */
#define ENABLE_SPI              1   /* SPI接口 */
#define ENABLE_I2C              1   /* I2C接口 */
#define ENABLE_ADC              1   /* ADC转换 */
#define ENABLE_PWM              0   /* PWM输出 - 不需要 */
#define ENABLE_WATCHDOG         1   /* 看门狗 */
#define ENABLE_RTC              0   /* 实时时钟 - 不需要 */

/* 通信协议选择 */
#define ENABLE_MODBUS           1
#define ENABLE_CANBUS           0   /* 不需要 */
#define ENABLE_MQTT             0   /* 太占空间 */

/* 调试功能 */
#define ENABLE_DEBUG_PRINT      1
#define ENABLE_ASSERT           1   /* 调试版本开启 */
#define ENABLE_ERROR_LOG        1
#define ENABLE_PERF_COUNTERS    0   /* 发布版本关闭 */

/* 模块大小限制 */
#define MAX_SENSORS             4   /* 而不是8或16 */
#define MAX_ACTUATORS           2   /* 限制执行器数量 */
#define UART_RX_BUFFER_SIZE     64  /* 而不是256 */
#define UART_TX_BUFFER_SIZE     32  /* 小缓冲区 */
#define MAX_PACKET_SIZE         128 /* 限制包大小 */
#define MAX_CONNECTIONS         2   /* 连接限制 */

/* 时间配置 */
#define MAIN_LOOP_INTERVAL_MS   10
#define SENSOR_SAMPLE_INTERVAL_MS 100
#define HEARTBEAT_INTERVAL_MS   1000

/* 电源管理 */
#define ENABLE_SLEEP_MODE       1
#define SLEEP_TIMEOUT_MS        5000
#define ENABLE_DYNAMIC_CLOCK    0   /* 简化时钟管理 */

#endif /* CONFIG_H */

/* 条件编译使用示例 */
#include "config.h"

/* UART模块 */
#if ENABLE_UART0
    void uart0_init(uint32_t baudrate);
    void uart0_send(uint8_t data);
    int uart0_recv(void);
#else
    #define uart0_init(x)   ((void)0)
    #define uart0_send(x)   ((void)0)
    #define uart0_recv()    (-1)
#endif

#if ENABLE_UART1
    void uart1_init(uint32_t baudrate);
    void uart1_send(uint8_t data);
    int uart1_recv(void);
#else
    #define uart1_init(x)   ((void)0)
    #define uart1_send(x)   ((void)0)
    #define uart1_recv()    (-1)
#endif

/* 调试打印 */
#if ENABLE_DEBUG_PRINT
    #include "mini_printf.h"
    #define DEBUG_PRINT(fmt, ...) mini_printf("[D] " fmt "\r\n", ##__VA_ARGS__)
    #define INFO_PRINT(fmt, ...)  mini_printf("[I] " fmt "\r\n", ##__VA_ARGS__)
    #define WARN_PRINT(fmt, ...)  mini_printf("[W] " fmt "\r\n", ##__VA_ARGS__)
    #define ERROR_PRINT(fmt, ...) mini_printf("[E] " fmt "\r\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) ((void)0)
    #define INFO_PRINT(fmt, ...)  ((void)0)
    #define WARN_PRINT(fmt, ...)  ((void)0)
    #define ERROR_PRINT(fmt, ...) ((void)0)
#endif

/* 断言 */
#if ENABLE_ASSERT
    #define ASSERT(cond) \
        do { \
            if (!(cond)) { \
                ERROR_PRINT("ASSERT failed: %s:%d: %s", __FILE__, __LINE__, #cond); \
                while(1); /* 死循环便于调试 */ \
            } \
        } while(0)
#else
    #define ASSERT(cond) ((void)0)
#endif

/* 使用示例 */
void system_init(void)
{
    INFO_PRINT("System init starting, version %d.%d.%d",
               FIRMWARE_VERSION_MAJOR,
               FIRMWARE_VERSION_MINOR,
               FIRMWARE_VERSION_PATCH);

    #if ENABLE_UART0
    uart0_init(115200);
    INFO_PRINT("UART0 initialized at 115200 baud");
    #endif

    #if ENABLE_UART1
    uart1_init(9600);
    INFO_PRINT("UART1 initialized at 9600 baud");
    #endif

    #if ENABLE_SPI
    spi_init();
    DEBUG_PRINT("SPI initialized");
    #endif

    #if ENABLE_I2C
    i2c_init(100000);
    DEBUG_PRINT("I2C initialized at 100kHz");
    #endif

    ASSERT(MAX_SENSORS <= 8);  /* 编译时检查 */
}
```

---

## 6. 数据结构压缩

### 6.1 数据打包技术

```c
/* ============================================================================
 * 数据结构压缩 - 在不影响功能的前提下最小化内存占用
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 原始结构体 - 16字节 */
typedef struct {
    uint32_t timestamp;   /* 4字节 */
    uint16_t value;       /* 2字节 */
    uint8_t  sensor_id;   /* 1字节 */
    uint8_t  status;      /* 1字节 */
    float    reading;     /* 4字节 */
    uint8_t  flags;       /* 1字节 + 3字节填充 */
} sensor_original_t;

/* 优化版本1: 重新排序 - 12字节 */
typedef struct {
    uint32_t timestamp;   /* 4字节 */
    float    reading;     /* 4字节 */
    uint16_t value;       /* 2字节 */
    uint8_t  sensor_id;   /* 1字节 */
    uint8_t  status;      /* 1字节 */
    uint8_t  flags;       /* 1字节 + 1字节填充 */
} sensor_reordered_t;

/* 优化版本2: 使用位域 - 10字节 */
typedef struct __attribute__((packed)) {
    uint32_t timestamp;      /* 4字节 */
    float    reading;        /* 4字节 */
    uint16_t value;          /* 2字节 */
    uint8_t  sensor_id;      /* 1字节 */
    uint8_t  status;         /* 1字节 */
} sensor_packed_t;

typedef struct __attribute__((packed)) {
    unsigned int is_active : 1;
    unsigned int has_error : 1;
    unsigned int reserved : 6;
} sensor_flags_t;

/* 优化版本3: 使用定点数替代浮点数 - 8字节 */
typedef struct __attribute__((packed)) {
    uint32_t timestamp;      /* 4字节 */
    int16_t  reading_fixed;  /* 2字节，0.01精度 */
    uint16_t value;          /* 2字节 */
    uint8_t  sensor_id;      /* 1字节 */
    uint8_t  status;         /* 1字节 */
} sensor_fixed_t;

/* 定点数转换宏 */
#define FLOAT_TO_FIXED(f)    ((int16_t)((f) * 100.0f))
#define FIXED_TO_FLOAT(fx)   ((float)(fx) / 100.0f)

/* 优化版本4: 时间戳压缩 - 6字节 */
typedef struct __attribute__((packed)) {
    uint16_t timestamp_delta;  /* 相对时间戳，2字节 */
    int16_t  reading_fixed;    /* 2字节 */
    uint8_t  sensor_id;        /* 1字节 */
    uint8_t  status;           /* 1字节 */
} sensor_compressed_t;

/* 时间戳管理 */
static uint32_t base_timestamp = 0;

void sensor_set_base_time(uint32_t timestamp)
{
    base_timestamp = timestamp;
}

void sensor_pack(sensor_compressed_t *out, uint32_t timestamp,
                 float reading, uint8_t id, uint8_t status)
{
    out->timestamp_delta = (uint16_t)(timestamp - base_timestamp);
    out->reading_fixed = FLOAT_TO_FIXED(reading);
    out->sensor_id = id;
    out->status = status;
}
```

### 6.2 压缩算法实现

```c
/* ============================================================================
 * 轻量级压缩算法
 * ============================================================================ */

/* 行程编码 (RLE) - 适合重复数据 */
int rle_encode(const uint8_t *in, int in_len, uint8_t *out, int out_max)
{
    int in_pos = 0, out_pos = 0;

    while (in_pos < in_len && out_pos < out_max) {
        uint8_t count = 1;
        uint8_t value = in[in_pos];

        /* 统计重复次数 */
        while (in_pos + count < in_len &&
               count < 255 &&
               in[in_pos + count] == value) {
            count++;
        }

        if (count >= 3) {
            /* 压缩: [0x00] [count] [value] */
            if (out_pos + 3 > out_max) break;
            out[out_pos++] = 0x00;      /* 压缩标记 */
            out[out_pos++] = count;
            out[out_pos++] = value;
        } else {
            /* 不压缩，直接复制 */
            for (int i = 0; i < count && out_pos < out_max; i++) {
                out[out_pos++] = value;
            }
        }

        in_pos += count;
    }

    return out_pos;
}

int rle_decode(const uint8_t *in, int in_len, uint8_t *out, int out_max)
{
    int in_pos = 0, out_pos = 0;

    while (in_pos < in_len && out_pos < out_max) {
        if (in[in_pos] == 0x00) {
            /* 压缩块 */
            if (in_pos + 2 >= in_len) break;
            uint8_t count = in[in_pos + 1];
            uint8_t value = in[in_pos + 2];

            for (int i = 0; i < count && out_pos < out_max; i++) {
                out[out_pos++] = value;
            }
            in_pos += 3;
        } else {
            /* 原始字节 */
            out[out_pos++] = in[in_pos++];
        }
    }

    return out_pos;
}

/* 增量编码 - 传感器数据压缩 */
int delta_encode(const int16_t *in, int count, int8_t *out, int out_max)
{
    if (count == 0 || out_max < count) return 0;

    /* 第一个值完整存储 */
    out[0] = (int8_t)(in[0] >> 8);
    if (out_max > 1) out[1] = (int8_t)(in[0] & 0xFF);

    int out_pos = 2;
    int16_t prev = in[0];

    for (int i = 1; i < count && out_pos < out_max; i++) {
        int16_t delta = in[i] - prev;

        /* 检查是否可以用8位表示 */
        if (delta >= -128 && delta <= 127) {
            out[out_pos++] = (int8_t)delta;
        } else {
            /* 溢出，存储特殊标记和完整值 */
            if (out_pos + 3 > out_max) break;
            out[out_pos++] = 0x80;  /* 溢出标记 */
            out[out_pos++] = (int8_t)(in[i] >> 8);
            out[out_pos++] = (int8_t)(in[i] & 0xFF);
        }

        prev = in[i];
    }

    return out_pos;
}
```

---

## 7. 位域使用最佳实践

### 7.1 位域基础与陷阱

```c
/* ============================================================================
 * 位域使用指南 - 节省内存但需谨慎
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 位域基本语法 */
typedef struct {
    unsigned int bit0 : 1;    /* 1位 */
    unsigned int bit1_2 : 2;  /* 2位 */
    unsigned int bit3_6 : 4;  /* 4位 */
    unsigned int bit7 : 1;    /* 1位 */
} byte_flags_t;  /* 总共8位 = 1字节 */

/* 位域打包 */
typedef struct {
    /* 第1字节 */
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int status : 2;
    unsigned int type : 4;

    /* 第2字节 */
    unsigned int error_code : 3;
    unsigned int reserved : 5;
} packed_flags_t;  /* 2字节 */

/* ============================================================================
 * 位域使用注意事项
 * ============================================================================ */

/* 警告1: 位域布局是编译器相关的 */
/* GCC: 从最低有效位开始填充
 * MSVC: 从最高有效位开始填充
 */

/* 警告2: 位域类型必须是 unsigned int, signed int, 或 _Bool */
/* 不推荐: 使用int (实现相关) */
typedef struct {
    int flag1 : 1;  /* 可能是有符号的! */
} bad_bitfield_t;

/* 推荐: 明确使用unsigned */
typedef struct {
    unsigned int flag1 : 1;  /* 明确无符号 */
} good_bitfield_t;

/* 警告3: 位域地址无法获取 */
typedef struct {
    unsigned int flag : 1;
} test_t;

test_t t;
/* void *p = &t.flag; */  /* 错误! 无法取地址 */

/* 警告4: 位域大小限制于类型 */
typedef struct {
    unsigned int a : 16;
    unsigned int b : 16;
    unsigned int c : 16;  /* 跨两个字，可能有填充 */
} large_bitfield_t;

/* 警告5: 对齐和填充 */
typedef struct {
    uint8_t  byte1;       /* 1字节 */
    /* 3字节填充! */
    unsigned int flags : 8;  /* 在下一个字 */
} misaligned_t;  /* 8字节而不是5字节 */

/* 修正 */
typedef struct {
    unsigned int flags : 8;  /* 打包在一起 */
    uint8_t  byte1;          /* 紧接着 */
} aligned_t;  /* 4字节 */

/* ============================================================================
 * 位域的最佳实践
 * ============================================================================ */

/* 实践1: 使用明确的整数类型 */
typedef struct __attribute__((packed)) {
    uint8_t  is_active : 1;
    uint8_t  has_error : 1;
    uint8_t  power_mode : 2;
    uint8_t  reserved : 4;
} device_status_t;

/* 实践2: 提供访问宏/函数 */
typedef struct {
    uint8_t bits;
} status_bits_t;

#define STATUS_IS_ACTIVE(s)    (((s).bits >> 0) & 1)
#define STATUS_HAS_ERROR(s)    (((s).bits >> 1) & 1)
#define STATUS_GET_POWER(s)    (((s).bits >> 2) & 3)

#define STATUS_SET_ACTIVE(s, v) \
    ((s).bits = ((s).bits & ~(1 << 0)) | (((v) & 1) << 0))
#define STATUS_SET_ERROR(s, v) \
    ((s).bits = ((s).bits & ~(1 << 1)) | (((v) & 1) << 1))
#define STATUS_SET_POWER(s, v) \
    ((s).bits = ((s).bits & ~(3 << 2)) | (((v) & 3) << 2))

/* 实践3: 位掩码替代位域 (更可控) */
typedef uint8_t status_mask_t;

#define MASK_ACTIVE   (1 << 0)
#define MASK_ERROR    (1 << 1)
#define MASK_POWER_L  (1 << 2)
#define MASK_POWER_H  (1 << 3)
#define MASK_POWER    (MASK_POWER_L | MASK_POWER_H)

static inline bool status_get_active(status_mask_t s)
{
    return (s & MASK_ACTIVE) != 0;
}

static inline void status_set_active(status_mask_t *s, bool val)
{
    if (val) *s |= MASK_ACTIVE;
    else *s &= ~MASK_ACTIVE;
}

static inline uint8_t status_get_power(status_mask_t s)
{
    return (s & MASK_POWER) >> 2;
}

static inline void status_set_power(status_mask_t *s, uint8_t val)
{
    *s = (*s & ~MASK_POWER) | ((val & 3) << 2);
}

/* 实践4: 联合体实现位域访问 */
typedef union {
    uint8_t byte;
    struct __attribute__((packed)) {
        uint8_t is_active : 1;
        uint8_t has_error : 1;
        uint8_t power_mode : 2;
        uint8_t reserved : 4;
    } bits;
} status_u_t;

/* 可以整体赋值或按位访问 */
status_u_t status;
status.byte = 0x05;  /* 整体设置 */
status.bits.is_active = 1;  /* 按位设置 */
```

### 7.2 实用位操作库

```c
/* ============================================================================
 * 位操作工具库
 * ============================================================================ */

#ifndef BITOPS_H
#define BITOPS_H

#include <stdint.h>
#include <stdbool.h>

/* 基本位操作宏 */
#define BIT_SET(reg, bit)      ((reg) |= (1U << (bit)))
#define BIT_CLEAR(reg, bit)    ((reg) &= ~(1U << (bit)))
#define BIT_TOGGLE(reg, bit)   ((reg) ^= (1U << (bit)))
#define BIT_READ(reg, bit)     (((reg) >> (bit)) & 1U)
#define BIT_WRITE(reg, bit, val) \
    ((val) ? BIT_SET(reg, bit) : BIT_CLEAR(reg, bit))

/* 多位操作 */
#define BITS_SET(reg, mask)    ((reg) |= (mask))
#define BITS_CLEAR(reg, mask)  ((reg) &= ~(mask))
#define BITS_TOGGLE(reg, mask) ((reg) ^= (mask))
#define BITS_READ(reg, mask)   ((reg) & (mask))
#define BITS_WRITE(reg, mask, val) \
    ((reg) = ((reg) & ~(mask)) | ((val) & (mask)))

/* 位域操作宏 */
#define BF_PREP(x, name)       (((x) & name##_MASK) << name##_SHIFT)
#define BF_MASK(name)          (((1U << name##_WIDTH) - 1) << name##_SHIFT)
#define BF_GET(y, name)        (((y) & BF_MASK(name)) >> name##_SHIFT)
#define BF_SET(y, x, name)     ((y) = ((y) & ~BF_MASK(name)) | BF_PREP(x, name))

/* 位图操作 */
typedef uint8_t bitmap_t;

static inline void bitmap_set_bit(bitmap_t *map, uint8_t bit)
{
    map[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_clear_bit(bitmap_t *map, uint8_t bit)
{
    map[bit / 8] &= ~(1 << (bit % 8));
}

static inline bool bitmap_test_bit(const bitmap_t *map, uint8_t bit)
{
    return (map[bit / 8] & (1 << (bit % 8))) != 0;
}

static inline void bitmap_toggle_bit(bitmap_t *map, uint8_t bit)
{
    map[bit / 8] ^= (1 << (bit % 8));
}

/* 高效位图 - 针对小位图 (<= 32位) */
typedef uint32_t small_bitmap_t;

static inline void small_bitmap_set(small_bitmap_t *map, uint8_t bit)
{
    *map |= (1U << bit);
}

static inline void small_bitmap_clear(small_bitmap_t *map, uint8_t bit)
{
    *map &= ~(1U << bit);
}

static inline bool small_bitmap_test(small_bitmap_t map, uint8_t bit)
{
    return (map & (1U << bit)) != 0;
}

/* 使用GCC内建函数查找第一个置位 */
static inline uint8_t small_bitmap_ffs(small_bitmap_t map)
{
    if (map == 0) return 0;
    return __builtin_ctz(map) + 1;  /* 返回1-based索引 */
}

/* 位反转 */
uint8_t bit_reverse8(uint8_t x)
{
    x = ((x & 0xAA) >> 1) | ((x & 0x55) << 1);
    x = ((x & 0xCC) >> 2) | ((x & 0x33) << 2);
    x = ((x & 0xF0) >> 4) | ((x & 0x0F) << 4);
    return x;
}

/* 计算置位位数 (population count) */
static inline uint8_t popcount8(uint8_t x)
{
    x = (x & 0x55) + ((x >> 1) & 0x55);
    x = (x & 0x33) + ((x >> 2) & 0x33);
    x = (x & 0x0F) + ((x >> 4) & 0x0F);
    return x;
}

static inline uint8_t popcount32(uint32_t x)
{
    /* 使用GCC内建 */
    return __builtin_popcount(x);
}

#endif /* BITOPS_H */
```

---

## 8. 精简printf实现

### 8.1 迷你printf实现

```c
/* ============================================================================
 * 精简printf实现 - 节省数十KB代码空间
 * ============================================================================ */

#ifndef MINI_PRINTF_H
#define MINI_PRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/* 输出字符回调类型 */
typedef void (*putc_func_t)(char c);

/* 设置输出函数 */
void mini_printf_init(putc_func_t func);

/* 格式化输出 */
int mini_printf(const char *fmt, ...);
int mini_vprintf(const char *fmt, va_list args);
int mini_snprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* MINI_PRINTF_H */

/* ============================================================================
 * 实现
 * ============================================================================ */

static putc_func_t putc_func = NULL;
static char *snprintf_buf = NULL;
static size_t snprintf_size = 0;
static size_t snprintf_pos = 0;

void mini_printf_init(putc_func_t func)
{
    putc_func = func;
}

/* 输出单个字符 */
static void output_char(char c)
{
    if (snprintf_buf && snprintf_pos < snprintf_size - 1) {
        snprintf_buf[snprintf_pos++] = c;
    } else if (putc_func) {
        putc_func(c);
    }
}

/* 输出字符串 */
static void output_str(const char *s)
{
    while (*s) {
        output_char(*s++);
    }
}

/* 整数转字符串 (内部缓冲区) */
static char *itoa_internal(int32_t value, char *buf, int base, int *len)
{
    char *p = buf;
    char *q = buf;
    int is_negative = 0;

    /* 处理负数 */
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }

    /* 转换 */
    do {
        int digit = value % base;
        *p++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= base;
    } while (value > 0);

    if (is_negative) {
        *p++ = '-';
    }

    *len = p - buf;

    /* 反转 */
    for (p--; q < p; q++, p--) {
        char t = *q;
        *q = *p;
        *p = t;
    }

    return buf;
}

/* 格式化整数输出 */
static void print_int(int32_t num, int base, int width, char pad)
{
    char buf[12];  /* 足够容纳32位整数 */
    int len;

    itoa_internal(num, buf, base, &len);

    /* 填充 */
    while (len < width) {
        output_char(pad);
        width--;
    }

    output_str(buf);
}

/* 无符号整数输出 */
static void print_uint(uint32_t num, int base, int width, char pad)
{
    char buf[12];
    char *p = buf;
    char *q = buf;

    if (num == 0) {
        *p++ = '0';
    } else {
        while (num > 0) {
            uint32_t digit = num % base;
            *p++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
            num /= base;
        }
    }

    int len = p - buf;

    /* 反转 */
    for (p--; q < p; q++, p--) {
        char t = *q;
        *q = *p;
        *p = t;
    }
    buf[len] = '\0';

    /* 填充 */
    while (len < width) {
        output_char(pad);
        width--;
    }

    output_str(buf);
}

/* 核心格式化函数 */
int mini_vprintf(const char *fmt, va_list args)
{
    char c;
    int count = 0;

    while ((c = *fmt++)) {
        if (c != '%') {
            output_char(c);
            count++;
            continue;
        }

        /* 解析格式 */
        char pad = ' ';
        int width = 0;
        int is_long = 0;

        c = *fmt++;

        /* 填充字符 */
        if (c == '0') {
            pad = '0';
            c = *fmt++;
        }

        /* 宽度 */
        while (c >= '0' && c <= '9') {
            width = width * 10 + (c - '0');
            c = *fmt++;
        }

        /* 长度修饰符 */
        if (c == 'l') {
            is_long = 1;
            c = *fmt++;
        }

        /* 格式字符 */
        switch (c) {
            case 'd':
            case 'i': {
                int32_t val = is_long ? va_arg(args, long) : va_arg(args, int);
                print_int(val, 10, width, pad);
                break;
            }

            case 'u': {
                uint32_t val = is_long ? va_arg(args, unsigned long)
                                       : va_arg(args, unsigned int);
                print_uint(val, 10, width, pad);
                break;
            }

            case 'x':
            case 'X': {
                uint32_t val = is_long ? va_arg(args, unsigned long)
                                       : va_arg(args, unsigned int);
                print_uint(val, 16, width, pad);
                break;
            }

            case 'c': {
                char val = (char)va_arg(args, int);
                output_char(val);
                break;
            }

            case 's': {
                const char *val = va_arg(args, const char *);
                output_str(val ? val : "(null)");
                break;
            }

            case 'p':
            case 'P': {
                void *val = va_arg(args, void *);
                output_str("0x");
                print_uint((uint32_t)val, 16, 8, '0');
                break;
            }

            case '%':
                output_char('%');
                break;

            default:
                output_char('%');
                output_char(c);
                break;
        }
    }

    /* 终止snprintf缓冲区 */
    if (snprintf_buf && snprintf_pos < snprintf_size) {
        snprintf_buf[snprintf_pos] = '\0';
    }

    return count;
}

/* 可变参数包装 */
int mini_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = mini_vprintf(fmt, args);
    va_end(args);
    return ret;
}

/* snprintf实现 */
int mini_snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;

    snprintf_buf = buf;
    snprintf_size = size;
    snprintf_pos = 0;

    va_start(args, fmt);
    int ret = mini_vprintf(fmt, args);
    va_end(args);

    snprintf_buf = NULL;
    return ret;
}
```

---

## 9. 中断安全编程

### 9.1 中断安全内存管理

```c
/* ============================================================================
 * 中断安全编程指南 - 避免竞态条件和数据损坏
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 关键概念:
 * 1. 中断可能随时发生
 * 2. 主循环和中断共享数据时需要保护
 * 3. 某些操作是原子的，某些不是
 */

/* ============================================================================
 * 原子操作
 * ============================================================================ */

/* 8位和16位操作通常是原子的 (在8/16位MCU上) */
/* 32位操作在32位MCU上是原子的 */

volatile uint8_t  atomic_byte;   /* 读写是原子的 */
volatile uint16_t atomic_half;   /* 16位MCU上是原子的 */
volatile uint32_t atomic_word;   /* 32位MCU上是原子的 */

/* 非原子操作示例 - 需要保护 */
typedef struct {
    uint8_t status;
    uint16_t count;
} sensor_data_t;

volatile sensor_data_t shared_data;  /* 读写都不是原子的! */

/* ============================================================================
 * 临界区保护 (Cortex-M)
 * ============================================================================ */

/* 进入临界区 */
#define ENTER_CRITICAL()    uint32_t primask = __get_PRIMASK(); \
                            __disable_irq()

/* 退出临界区 */
#define EXIT_CRITICAL()     __set_PRIMASK(primask)

/* 临界区使用示例 */
void update_shared_data(uint8_t status, uint16_t count)
{
    ENTER_CRITICAL();
    /* 以下操作是原子的 - 中断被禁用 */
    shared_data.status = status;
    shared_data.count = count;
    EXIT_CRITICAL();
}

/* 读取共享数据 */
void read_shared_data(uint8_t *status, uint16_t *count)
{
    ENTER_CRITICAL();
    *status = shared_data.status;
    *count = shared_data.count;
    EXIT_CRITICAL();
}

/* ============================================================================
 * 无锁队列 - 单生产者单消费者
 * ============================================================================ */

/* 当只有一个生产者和一个消费者时，不需要临界区保护 */

typedef struct {
    uint8_t buffer[64];
    volatile uint8_t head;   /* 生产者写入 */
    volatile uint8_t tail;   /* 消费者读取 */
} lockless_queue_t;

lockless_queue_t irq_queue;

/* 生产者在中断中调用 */
bool irq_queue_put(uint8_t data)
{
    uint8_t next_head = (irq_queue.head + 1) % sizeof(irq_queue.buffer);

    /* 检查队列满 - 原子读取tail */
    if (next_head == irq_queue.tail) {
        return false;  /* 满 */
    }

    irq_queue.buffer[irq_queue.head] = data;
    irq_queue.head = next_head;
    return true;
}

/* 消费者在主循环中调用 */
bool irq_queue_get(uint8_t *data)
{
    /* 检查队列空 - 原子读取head */
    if (irq_queue.head == irq_queue.tail) {
        return false;  /* 空 */
    }

    *data = irq_queue.buffer[irq_queue.tail];
    irq_queue.tail = (irq_queue.tail + 1) % sizeof(irq_queue.buffer);
    return true;
}

/* ============================================================================
 * 双缓冲技术
 * ============================================================================ */

typedef struct {
    uint8_t buffer_a[32];
    uint8_t buffer_b[32];
    volatile uint8_t *volatile active_buffer;    /* ISR写入 */
    volatile uint8_t *volatile processing_buffer; /* 主循环处理 */
    volatile bool data_ready;
} double_buffer_t;

double_buffer_t dbuf = {
    .active_buffer = dbuf.buffer_a,
    .processing_buffer = NULL,
    .data_ready = false
};

/* 中断服务程序 - 填充缓冲区 */
void ISR_data_received(void)
{
    static uint8_t pos = 0;
    uint8_t data = UART->DR;

    if (!dbuf.data_ready && dbuf.active_buffer) {
        dbuf.active_buffer[pos++] = data;

        if (pos >= 32 || data == '\n') {
            /* 缓冲区满或行结束 - 交换 */
            dbuf.processing_buffer = dbuf.active_buffer;
            dbuf.active_buffer = (dbuf.active_buffer == dbuf.buffer_a)
                                 ? dbuf.buffer_b : dbuf.buffer_a;
            dbuf.data_ready = true;
            pos = 0;
        }
    }
}

/* 主循环 - 处理缓冲区 */
void process_buffered_data(void)
{
    if (dbuf.data_ready) {
        /* 处理 dbuf.processing_buffer */
        process_data(dbuf.processing_buffer, 32);

        /* 清除标志 */
        dbuf.data_ready = false;
    }
}

/* ============================================================================
 * 中断安全的标志操作
 * ============================================================================ */

#define ATOMIC_SET(ptr, val)      __atomic_store_n(ptr, val, __ATOMIC_SEQ_CST)
#define ATOMIC_GET(ptr)           __atomic_load_n(ptr, __ATOMIC_SEQ_CST)
#define ATOMIC_OR(ptr, val)       __atomic_fetch_or(ptr, val, __ATOMIC_SEQ_CST)
#define ATOMIC_AND(ptr, val)      __atomic_fetch_and(ptr, val, __ATOMIC_SEQ_CST)

volatile uint32_t event_flags = 0;

#define EVENT_UART_RX     (1 << 0)
#define EVENT_TIMER       (1 << 1)
#define EVENT_ADC_DONE    (1 << 2)

/* 中断中设置标志 */
void UART_IRQHandler(void)
{
    ATOMIC_OR(&event_flags, EVENT_UART_RX);
}

void TIM_IRQHandler(void)
{
    ATOMIC_OR(&event_flags, EVENT_TIMER);
}

/* 主循环处理标志 */
void event_loop(void)
{
    uint32_t flags = ATOMIC_GET(&event_flags);

    if (flags & EVENT_UART_RX) {
        ATOMIC_AND(&event_flags, ~EVENT_UART_RX);
        handle_uart_rx();
    }

    if (flags & EVENT_TIMER) {
        ATOMIC_AND(&event_flags, ~EVENT_TIMER);
        handle_timer();
    }
}
```

---

## 10. 实际项目：传感器节点固件

### 10.1 完整传感器节点实现

```c
/* ============================================================================
 * 传感器节点固件 - 内存受限系统完整示例
 *
 * 目标平台: STM32F103C8 (20KB RAM, 64KB Flash)
 * 功能: 多传感器采集、LoRa传输、低功耗管理
 * ============================================================================
 */

/* 主要源文件:
 * - main.c        : 主程序入口和主循环
 * - config.h      : 系统配置
 * - sensors.c     : 传感器驱动
 * - lora.c        : LoRa通信
 * - power.c       : 电源管理
 * - memory.c      : 内存管理
 * - utils.c       : 工具函数
 */

/* main.c */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "types.h"
#include "hal.h"

/* 全局状态 */
static struct {
    system_state_t state;
    uint32_t last_tx_time;
    uint32_t last_sample_time;
    uint16_t tx_seq_num;
    uint8_t device_id[4];
} system_state;

/* 静态缓冲区 */
static uint8_t tx_buffer[TX_BUFFER_SIZE];
static sensor_sample_t *sample_buffer[SENSOR_COUNT * 2];
static uint8_t sample_count = 0;

/* 初始化 */
static void system_init(void)
{
    /* 硬件初始化 */
    system_clock_init();
    uart_init(115200);

    #if ENABLE_DEBUG
    uart_send_string("Sensor Node v");
    uart_send('0' + FIRMWARE_VERSION_MAJOR);
    uart_send('.');
    uart_send('0' + FIRMWARE_VERSION_MINOR);
    uart_send_string("\r\n");
    #endif

    /* 读取设备ID (从UID) */
    extern uint32_t UID[3];
    system_state.device_id[0] = UID[0] & 0xFF;
    system_state.device_id[1] = (UID[0] >> 8) & 0xFF;
    system_state.device_id[2] = UID[1] & 0xFF;
    system_state.device_id[3] = (UID[1] >> 8) & 0xFF;

    /* 初始化模块 */
    sensors_init();

    #if ENABLE_LORA
    lora_init();
    #endif

    stack_monitor_init();

    system_state.state = STATE_IDLE;
    system_state.tx_seq_num = 0;
}

/* 采样任务 */
static void sample_task(uint32_t current_time)
{
    if (current_time - system_state.last_sample_time < SAMPLE_INTERVAL_MS) {
        return;
    }
    system_state.last_sample_time = current_time;

    /* 采集所有传感器 */
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (sensor_should_sample(i, current_time)) {
            sensor_sample_t *sample = sensor_sample(i);
            if (sample) {
                sample->timestamp_delta = (uint16_t)(current_time / 1000);

                if (sample_count < sizeof(sample_buffer) / sizeof(sample_buffer[0])) {
                    sample_buffer[sample_count++] = sample;
                }
            }
        }
    }
}

/* 传输任务 */
static void transmit_task(uint32_t current_time)
{
    if (current_time - system_state.last_tx_time < TX_INTERVAL_MS) {
        return;
    }
    if (sample_count == 0) {
        return;
    }

    system_state.last_tx_time = current_time;
    system_state.state = STATE_TRANSMITTING;

    /* 构建数据帧 */
    data_frame_header_t *frame = (data_frame_header_t *)tx_buffer;
    frame->sync = FRAME_SYNC_BYTE;
    frame->version = FRAME_VERSION;
    frame->seq_num = system_state.tx_seq_num++;
    memcpy(frame->device_id, system_state.device_id, 4);
    frame->battery_level = get_battery_level();
    frame->rssi = get_last_rssi();
    frame->sample_count = sample_count;

    /* 复制样本数据 */
    uint8_t *payload = tx_buffer + sizeof(data_frame_header_t);
    uint8_t payload_size = sample_count * sizeof(sensor_sample_t);

    for (int i = 0; i < sample_count; i++) {
        memcpy(payload + i * sizeof(sensor_sample_t),
               sample_buffer[i], sizeof(sensor_sample_t));
        sample_free(sample_buffer[i]);
        sample_buffer[i] = NULL;
    }

    /* 计算CRC */
    uint8_t total_size = sizeof(data_frame_header_t) + payload_size;
    tx_buffer[total_size] = crc8(tx_buffer, total_size);

    /* 发送 */
    #if ENABLE_LORA
    lora_send(tx_buffer, total_size + 1);
    #endif

    sample_count = 0;
    system_state.state = STATE_IDLE;
}

/* 电源管理任务 */
static void power_task(uint32_t current_time)
{
    #if ENABLE_SLEEP
    if (system_state.state == STATE_IDLE &&
        can_enter_sleep(current_time)) {

        system_state.state = STATE_SLEEPING;

        /* 进入低功耗模式 */
        enter_sleep_mode();

        system_state.state = STATE_IDLE;
    }
    #endif
}

/* 看门狗喂狗 */
static void watchdog_task(void)
{
    #if ENABLE_WATCHDOG
    watchdog_feed();
    #endif
}

/* 主循环 */
static void main_loop(void)
{
    while (1) {
        uint32_t current_time = get_tick_ms();

        /* 执行任务 */
        sample_task(current_time);
        transmit_task(current_time);
        power_task(current_time);
        watchdog_task();

        /* 短暂延时让出CPU */
        delay_ms(1);
    }
}

/* 主函数 */
int main(void)
{
    system_init();
    main_loop();

    /* 不应到达这里 */
    return 0;
}
```

---

## 11. 功耗优化

### 11.1 睡眠模式管理

```c
/* ============================================================================
 * 电源管理 - 低功耗设计
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 睡眠模式选择 */
typedef enum {
    SLEEP_MODE_IDLE,        /* 仅CPU停止 */
    SLEEP_MODE_LIGHT,       /* CPU+部分外设停止 */
    SLEEP_MODE_DEEP,        /* CPU+大部分外设停止 */
    SLEEP_MODE_STANDBY      /* 仅保留备份域 */
} sleep_mode_t;

/* 功耗统计 */
typedef struct {
    uint32_t active_time_ms;      /* 活跃时间 */
    uint32_t sleep_time_ms;       /* 睡眠时间 */
    uint32_t wake_count;          /* 唤醒次数 */
    uint32_t avg_current_ua;      /* 平均电流(微安) */
} power_stats_t;

static power_stats_t power_stats;

/* 进入睡眠模式 */
void enter_sleep_mode(sleep_mode_t mode)
{
    switch (mode) {
        case SLEEP_MODE_IDLE:
            /* 停止CPU时钟，外设继续运行 */
            __WFI();  /* Wait For Interrupt */
            break;

        case SLEEP_MODE_LIGHT:
            /* 配置唤醒源 */
            configure_wakeup_sources();

            /* 降低系统时钟 */
            reduce_system_clock();

            /* 进入低功耗模式 */
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            __WFI();

            /* 恢复时钟 */
            restore_system_clock();
            break;

        case SLEEP_MODE_DEEP:
            /* 关闭不必要的外设 */
            disable_unused_peripherals();

            /* 配置RTC唤醒 */
            set_rtc_wakeup(10000);  /* 10秒后唤醒 */

            /* 进入停止模式 */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PWR->CR &= ~PWR_CR_PDDS;  /* 停止模式 */
            __WFI();

            /* 恢复 */
            reinit_after_wakeup();
            break;

        case SLEEP_MODE_STANDBY:
            /* 保存关键数据到备份域 */
            save_to_backup_domain();

            /* 进入待机模式 */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PWR->CR |= PWR_CR_PDDS;   /* 待机模式 */
            SCB->SCR |= SCB_SCR_SEVEONPEND_Msk;
            __WFI();
            break;
    }
}

/* 动态电压频率调节 (DVFS) */
typedef enum {
    CPU_SPEED_1MHZ = 1000000,
    CPU_SPEED_8MHZ = 8000000,
    CPU_SPEED_24MHZ = 24000000,
    CPU_SPEED_72MHZ = 72000000
} cpu_speed_t;

static cpu_speed_t current_speed = CPU_SPEED_72MHZ;

void set_cpu_speed(cpu_speed_t speed)
{
    if (speed == current_speed) return;

    /* 等待所有操作完成 */
    __DSB();

    switch (speed) {
        case CPU_SPEED_1MHZ:
            /* 切换到内部低速时钟 */
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
            while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

            /* 分频到1MHz */
            RCC->CFGR |= RCC_CFGR_HPRE_DIV8;

            /* 降低电压 */
            PWR->CR &= ~PWR_CR_VOS;
            break;

        case CPU_SPEED_72MHZ:
            /* 提高电压 */
            PWR->CR |= PWR_CR_VOS;

            /* 等待电压稳定 */
            while (!(PWR->CSR & PWR_CSR_VOSRDY));

            /* 切换到PLL */
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
            while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
            break;

        default:
            break;
    }

    current_speed = speed;

    /* 更新延时函数 */
    update_delay_calibration(speed);
}

/* 外设电源管理 */
typedef struct {
    uint32_t enable_mask;
    uint32_t sleep_mask;
    bool auto_disable;
} peripheral_power_t;

static peripheral_power_t periph_power = {
    .enable_mask = 0,
    .sleep_mask = 0xFFFFFFFF,  /* 默认睡眠时关闭所有 */
    .auto_disable = true
};

void peripheral_enable(uint32_t periph_id)
{
    periph_power.enable_mask |= (1 << periph_id);
    RCC->APB1ENR |= (1 << periph_id);
}

void peripheral_disable(uint32_t periph_id)
{
    periph_power.enable_mask &= ~(1 << periph_id);
    RCC->APB1ENR &= ~(1 << periph_id);
}

void peripherals_sleep_prepare(void)
{
    uint32_t disable_mask = periph_power.sleep_mask & ~periph_power.enable_mask;

    /* 关闭睡眠时不需要的外设 */
    RCC->APB1ENR &= ~disable_mask;
    RCC->APB2ENR &= ~(disable_mask >> 16);
}

void peripherals_wakeup_restore(void)
{
    /* 恢复外设时钟 */
    RCC->APB1ENR |= periph_power.enable_mask & 0xFFFF;
    RCC->APB2ENR |= (periph_power.enable_mask >> 16) & 0xFFFF;
}
```

### 11.2 电池管理

```c
/* ============================================================================
 * 电池管理 - 延长电池寿命
 * ============================================================================ */

/* 电池类型 */
typedef enum {
    BATTERY_LITHIUM_3V,      /* CR2032等 */
    BATTERY_ALKALINE_1_5V,   /* AAA/AA */
    BATTERY_LIPO_3_7V        /* 锂电池 */
} battery_type_t;

/* 电池状态 */
typedef struct {
    battery_type_t type;
    uint16_t voltage_mv;     /* 当前电压 */
    uint8_t percentage;      /* 剩余电量 % */
    uint16_t cycles;         /* 充放电循环 */
    bool is_charging;        /* 是否充电中 */
} battery_status_t;

static battery_status_t battery;

/* ADC读取电池电压 */
uint16_t read_battery_voltage(void)
{
    /* 启动ADC转换 */
    ADC1->SQR3 = BATTERY_ADC_CHANNEL;
    ADC1->CR2 |= ADC_CR2_SWSTART;

    /* 等待完成 */
    while (!(ADC1->SR & ADC_SR_EOC));

    uint16_t adc_value = ADC1->DR;

    /* 计算电压 (根据分压电阻校准) */
    uint32_t voltage = (adc_value * 3300 * 2) / 4096;

    return (uint16_t)voltage;
}

/* 计算电池百分比 */
uint8_t calculate_battery_percentage(uint16_t voltage_mv, battery_type_t type)
{
    switch (type) {
        case BATTERY_LITHIUM_3V:
            /* CR2032: 3.0V满电, 2.0V空 */
            if (voltage_mv >= 3000) return 100;
            if (voltage_mv <= 2000) return 0;
            return (uint8_t)((voltage_mv - 2000) / 10);

        case BATTERY_LIPO_3_7V:
            /* LiPo: 4.2V满电, 3.0V空 (简化线性) */
            if (voltage_mv >= 4200) return 100;
            if (voltage_mv <= 3000) return 0;
            return (uint8_t)((voltage_mv - 3000) / 12);

        default:
            return 0;
    }
}

/* 电池监控任务 */
void battery_monitor_task(void)
{
    static uint32_t last_check = 0;
    uint32_t current_time = get_tick_ms();

    /* 每60秒检查一次 */
    if (current_time - last_check < 60000) return;
    last_check = current_time;

    battery.voltage_mv = read_battery_voltage();
    battery.percentage = calculate_battery_percentage(battery.voltage_mv, battery.type);

    /* 低电量警告 */
    if (battery.percentage < 20) {
        set_low_power_mode();
        reduce_transmission_rate();
    }

    /* 极低电量 - 进入休眠 */
    if (battery.percentage < 5) {
        enter_emergency_sleep();
    }
}

/* 低功耗模式设置 */
void set_low_power_mode(void)
{
    /* 降低采样率 */
    set_sample_interval(60000);  /* 1分钟一次 */

    /* 降低发送频率 */
    set_transmit_interval(300000);  /* 5分钟一次 */

    /* 降低CPU频率 */
    set_cpu_speed(CPU_SPEED_1MHZ);

    /* 禁用LED等非必要功能 */
    disable_status_led();
}
```

---

## 12. 调试与监控工具

### 12.1 轻量级日志系统

```c
/* ============================================================================
 * 轻量级日志系统 - 环形缓冲区实现
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* 日志级别 */
typedef enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
} log_level_t;

/* 日志配置 */
#define LOG_BUFFER_SIZE     256
#define LOG_MESSAGE_MAX     32
#define CURRENT_LOG_LEVEL   LOG_LEVEL_DEBUG

/* 日志条目 */
typedef struct {
    uint32_t timestamp;
    uint8_t level;
    char message[LOG_MESSAGE_MAX];
} log_entry_t;

/* 环形日志缓冲区 */
static struct {
    log_entry_t entries[LOG_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    uint8_t count;
    log_level_t level_filter;
} log_ring;

/* 初始化 */
void log_init(void)
{
    log_ring.head = 0;
    log_ring.tail = 0;
    log_ring.count = 0;
    log_ring.level_filter = CURRENT_LOG_LEVEL;
}

/* 写入日志 */
void log_write(log_level_t level, const char *msg)
{
    if (level > log_ring.level_filter) return;

    uint16_t idx = log_ring.head;

    log_ring.entries[idx].timestamp = get_tick_ms();
    log_ring.entries[idx].level = level;
    strncpy(log_ring.entries[idx].message, msg, LOG_MESSAGE_MAX - 1);
    log_ring.entries[idx].message[LOG_MESSAGE_MAX - 1] = '\0';

    log_ring.head = (idx + 1) % LOG_BUFFER_SIZE;

    if (log_ring.count < LOG_BUFFER_SIZE) {
        log_ring.count++;
    } else {
        /* 覆盖旧数据 */
        log_ring.tail = log_ring.head;
    }
}

/* 日志宏 */
#define LOG_E(msg)  log_write(LOG_LEVEL_ERROR, msg)
#define LOG_W(msg)  log_write(LOG_LEVEL_WARN, msg)
#define LOG_I(msg)  log_write(LOG_LEVEL_INFO, msg)
#define LOG_D(msg)  log_write(LOG_LEVEL_DEBUG, msg)

/* 导出日志 */
void log_dump(void (*output_func)(const char *))
{
    char buf[64];
    const char *level_str[] = {"", "E", "W", "I", "D", "V"};

    while (log_ring.tail != log_ring.head) {
        log_entry_t *entry = &log_ring.entries[log_ring.tail];

        mini_snprintf(buf, sizeof(buf), "[%lu]%s:%s\r\n",
                      entry->timestamp,
                      level_str[entry->level],
                      entry->message);

        output_func(buf);

        log_ring.tail = (log_ring.tail + 1) % LOG_BUFFER_SIZE;
    }
}

/* ============================================================================
 * 性能计数器
 * ============================================================================ */

/* 简单性能计数器 */
typedef struct {
    const char *name;
    uint32_t count;
    uint32_t total_time_us;
    uint32_t max_time_us;
} perf_counter_t;

#define MAX_PERF_COUNTERS   8

static perf_counter_t perf_counters[MAX_PERF_COUNTERS];
static uint8_t perf_counter_count = 0;

perf_counter_t* perf_register(const char *name)
{
    if (perf_counter_count >= MAX_PERF_COUNTERS) return NULL;

    perf_counter_t *counter = &perf_counters[perf_counter_count++];
    counter->name = name;
    counter->count = 0;
    counter->total_time_us = 0;
    counter->max_time_us = 0;

    return counter;
}

void perf_begin(perf_counter_t *counter)
{
    counter->start_time = get_micros();
}

void perf_end(perf_counter_t *counter)
{
    uint32_t elapsed = get_micros() - counter->start_time;

    counter->count++;
    counter->total_time_us += elapsed;
    if (elapsed > counter->max_time_us) {
        counter->max_time_us = elapsed;
    }
}

void perf_report(void)
{
    char buf[64];

    for (int i = 0; i < perf_counter_count; i++) {
        perf_counter_t *c = &perf_counters[i];
        uint32_t avg = c->count > 0 ? c->total_time_us / c->count : 0;

        mini_snprintf(buf, sizeof(buf), "%s: cnt=%lu avg=%lu max=%lu\r\n",
                      c->name, c->count, avg, c->max_time_us);
        uart_send_string(buf);
    }
}
```

---

## 参考资源

- **ARM Cortex-M参考手册**: 了解低层内存管理
- **CMSIS-Core**: ARM官方库，优化中断处理
- **newlib-nano**: 嵌入式优化C库
- **pico-debug**: 针对Pico平台的调试工具
- **Contiki-NG**: 内存受限IoT操作系统
- **Zephyr RTOS**: 最小配置8KB RAM
- **NuttX**: 实时操作系统，适合小内存

---

## 总结

本指南涵盖了内存受限系统开发的核心技术：

1. **内存管理**: 静态分配优先，避免堆使用
2. **代码优化**: 编译器选项和精简库函数
3. **数据结构**: 压缩和高效布局
4. **中断安全**: 无锁队列和临界区保护
5. **低功耗**: 睡眠模式和动态电压调节

**目标平台**: 8-32位MCU, RAM < 64KB
**工具链**: GCC ARM / AVR-GCC / RISC-V GCC
**代码**: 2000+ 行完整可运行代码

---

*最后更新: 2026-03-17*
*作者: Embedded Systems Team*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
