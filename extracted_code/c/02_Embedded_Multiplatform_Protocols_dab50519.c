/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 550
 * Language: c
 * Block ID: dab50519
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// RP2040核心特性
// - 双核Cortex-M0+ (无FPU)
// - 可编程I/O (PIO): 最大特色
// - 所有SRAM在统一地址空间
// - 启动必须从外部QSPI Flash
// ============================================================

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/sync.h"

// RP2040启动代码特殊性
// main() 在核心0上运行，核心1默认处于等待状态
int main(void) {
    // 初始化Pico SDK
    stdio_init_all();

    // 启动核心1
    multicore_launch_core1(core1_main);

    // 核心0主循环
    while (1) {
        // 核心0工作
        tight_loop_contents();
    }
}

void core1_main(void) {
    // 核心1从这里开始
    while (1) {
        // 核心1工作
        tight_loop_contents();
    }
}

// 双核FIFO通信 (RP2040特有的硬件FIFO)
void multicore_fifo_example(void) {
    // 核心0发送
    multicore_fifo_push_blocking(0x12345678);

    // 核心1接收
    uint32_t data = multicore_fifo_pop_blocking();
}

// 原子操作 (Cortex-M0+没有LDREX/STREX，需要特殊处理)
// RP2040提供硬件互斥锁
void atomic_operation_example(void) {
    static uint32_t shared_counter = 0;

    // 获取硬件互斥锁
    uint32_t save = spin_lock_blocking(spinlock);

    // 临界区
    shared_counter++;

    // 释放锁
    spin_unlock(spinlock, save);
}
