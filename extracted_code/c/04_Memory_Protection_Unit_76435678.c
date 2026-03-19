/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\04_Memory_Protection_Unit.md
 * Line: 1453
 * Language: c
 * Block ID: 76435678
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * MPU配置对编译器优化的影响
 */

/*
 * 内存类型属性
 * 告诉编译器内存区域的属性，帮助优化
 */

// 强序内存 (设备寄存器)
#define STRONGLY_ORDERED __attribute__((section(".strongly_ordered")))

// 设备内存
#define DEVICE_MEMORY __attribute__((section(".device")))

// 可缓存内存
#define CACHEABLE_MEMORY __attribute__((section(".cacheable")))

/*
 * 使用volatile防止编译器优化设备访问
 */
volatile uint32_t * const TIMER_REG = (uint32_t *)0x40000000;

void timer_delay(uint32_t ticks) {
    uint32_t start = *TIMER_REG;
    // 如果不加volatile，编译器可能优化掉这个循环
    while ((*TIMER_REG - start) < ticks) {
        // 等待
    }
}

/*
 * 内存屏障与MPU
 * 当MPU将内存映射为不同属性时，需要屏障确保一致性
 */
void memory_barrier_example(void) {
    // 写入设备寄存器
    *DEVICE_REG = value;

    // 内存屏障 - 确保写操作完成
    __DSB();  // 数据同步屏障

    // 现在可以安全读取
    uint32_t status = *STATUS_REG;
}

/*
 * 链接器脚本与MPU区域对齐
 * 确保区域满足MPU对齐要求
 */

/*
 * linker_script.ld:
 *
 * MEMORY {
 *   FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
 *   RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 128K
 * }
 *
 * SECTIONS {
 *   .text : {
 *     *(.text*)
 *   } > FLASH
 *
 *   .data : {
 *     *(.data*)
 *   } > RAM AT > FLASH
 *
 *   // 确保栈底对齐到4KB边界 (MPU要求)
 *   __stack_bottom = ALIGN(4096);
 * }
 */
