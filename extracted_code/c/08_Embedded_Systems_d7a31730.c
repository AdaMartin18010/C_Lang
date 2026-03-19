/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 82
 * Language: c
 * Block ID: d7a31730
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// startup.c - 简化的启动代码

// 链接器脚本定义的符号
extern uint32_t _estack;        // 栈顶
extern uint32_t _sidata;        // ROM中的数据起始
extern uint32_t _sdata;         // RAM数据起始
extern uint32_t _edata;         // RAM数据结束
extern uint32_t _sbss;          // BSS起始
extern uint32_t _ebss;          // BSS结束

// 主函数声明
extern int main(void);

// 复位处理程序
void Reset_Handler(void) {
    // 1. 设置栈指针
    __asm volatile ("ldr sp, =_estack");

    // 2. 复制.data段从Flash到RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // 3. 清零.bss段
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    // 4. 调用系统初始化 (可选)
    SystemInit();

    // 5. 调用main
    main();

    // 6. 如果main返回，无限循环
    while (1);
}

// 向量表
__attribute__((section(".isr_vector")))
void (*const vector_table[])(void) = {
    (void (*)(void))&_estack,   // 栈顶
    Reset_Handler,               // 复位
    NMI_Handler,                 // NMI
    HardFault_Handler,           // 硬 fault
    // ... 更多中断
};
