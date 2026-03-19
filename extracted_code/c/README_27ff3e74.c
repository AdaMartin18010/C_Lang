/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\README.md
 * Line: 342
 * Language: c
 * Block ID: 27ff3e74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// startup_stm32f103.c
#include <stdint.h>

// 链接器符号
extern uint32_t _sidata, _sdata, _edata;
extern uint32_t _sbss, _ebss;
extern uint32_t _estack;
extern int main(void);

// 默认中断处理
void Default_Handler(void) {
    while (1);
}

// 弱定义异常处理
void Reset_Handler(void);
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

// 中断向量表
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))&_estack,    // 栈顶
    Reset_Handler,               // 复位
    NMI_Handler,                 // NMI
    HardFault_Handler,           // Hard Fault
    0, 0, 0, 0,                  // 保留
    SVC_Handler,                 // SVCall
    0, 0,                        // 保留
    PendSV_Handler,              // PendSV
    SysTick_Handler,             // SysTick
};

// 复位处理
void Reset_Handler(void) {
    // 复制 .data
    uint32_t *pSrc = &_sidata;
    uint32_t *pDest = &_sdata;
    while (pDest < &_edata) {
        *pDest++ = *pSrc++;
    }

    // 清零 .bss
    pDest = &_sbss;
    while (pDest < &_ebss) {
        *pDest++ = 0;
    }

    // 跳转到 main
    main();

    // 不应该到达这里
    while (1);
}
