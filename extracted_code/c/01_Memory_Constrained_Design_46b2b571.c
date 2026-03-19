/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 257
 * Language: c
 * Block ID: 46b2b571
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
