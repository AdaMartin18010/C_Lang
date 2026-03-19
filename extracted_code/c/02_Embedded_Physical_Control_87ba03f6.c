/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 152
 * Language: c
 * Block ID: 87ba03f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// volatile = "编译器，不要优化我，我有副作用"
// ============================================================

// 场景1: 硬件寄存器 (读取有副作用)
// 物理现实: 读取状态寄存器可能清除标志位
volatile uint32_t *STATUS_REG = (uint32_t *)0x40001000;

// 错误: 编译器优化后只读一次
uint32_t status = *STATUS_REG;
if (status & FLAG_A) { /* ... */ }
if (status & FLAG_B) { /* ... */ }  // 可能错过B标志的变化

// 正确: 每次读取都是新的硬件状态
if (*STATUS_REG & FLAG_A) { /* ... */ }
if (*STATUS_REG & FLAG_B) { /* ... */ }

// 场景2: 内存映射I/O (写入必须发生)
// 物理现实: 写入GPIO就是改变引脚电平
volatile uint32_t *GPIO_OUT = (uint32_t *)0x40020014;

// 错误: 编译器认为无意义，优化掉
*GPIO_OUT = 1;
*GPIO_OUT = 0;  // 可能被优化，只保留最后一条

// 正确: 两次写入都会发生
volatile uint32_t * const GPIO_OUT = (uint32_t *)0x40020014;
*GPIO_OUT = 1;  // 引脚变高
*GPIO_OUT = 0;  // 引脚变低 (产生脉冲)

// 场景3: 多核/中断共享内存
volatile int data_ready = 0;
volatile int shared_data = 0;

// ISR (中断服务程序)
void ISR(void) {
    shared_data = sensor_read();  // 写入共享数据
    data_ready = 1;               // 标记就绪
}

// 主循环
while (!data_ready) { /* 等待 */ }  // 必须volatile，否则可能永远循环
int data = shared_data;              // 读取共享数据
