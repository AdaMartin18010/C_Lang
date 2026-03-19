/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 152
 * Language: c
 * Block ID: 80107ab3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux内核 8259A 相关定义 (arch/x86/include/asm/i8259.h)
#define PIC_MASTER_CMD      0x20    // 主片命令口
#define PIC_MASTER_DATA     0x21    // 主片数据口
#define PIC_SLAVE_CMD       0xA0    // 从片命令口
#define PIC_SLAVE_DATA      0xA1    // 从片数据口

// ICW1 - 初始化命令字1
#define ICW1_ICW4       0x01    // 需要ICW4
#define ICW1_SINGLE     0x02    // 单片模式
#define ICW1_INTERVAL4  0x04    // 中断向量间隔4
#define ICW1_LEVEL      0x08    // 电平触发模式
#define ICW1_INIT       0x10    // 初始化命令

// ICW4 - 初始化命令字4
#define ICW4_8086       0x01    // 8086/88模式
#define ICW4_AUTO       0x02    // 自动EOI
#define ICW4_BUF_SLAVE  0x08    // 缓冲模式-从片
#define ICW4_BUF_MASTER 0x0C    // 缓冲模式-主片
#define ICW4_SFNM       0x10    // 特殊全嵌套模式

// OCW2 - 操作命令字2 (EOI)
#define PIC_EOI         0x20    // 非特定EOI
