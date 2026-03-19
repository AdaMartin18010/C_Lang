/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 733
 * Language: c
 * Block ID: 9feebee4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 临界区: 保护共享资源的不可分割操作
// ============================================================

// 方法1: 全局中断开关 (最粗粒度)
#define CRITICAL_SECTION_BEGIN()  __disable_irq()
#define CRITICAL_SECTION_END()    __enable_irq()

// 问题: 如果嵌套，外层的enable会提前开启中断

// 改进: 嵌套安全版本
static volatile uint32_t critical_nesting = 0;

void enter_critical(void) {
    __disable_irq();
    critical_nesting++;
}

void exit_critical(void) {
    if (--critical_nesting == 0) {
        __enable_irq();
    }
}

// 使用
void update_shared_data(void) {
    enter_critical();
    // ... 访问共享资源
    exit_critical();
}

// 方法2: 基 PriMask (Cortex-M特有，更快)
// 只屏蔽 configurable 优先级的中断，NMI和HardFault仍然响应

static inline uint32_t primask_get(void) {
    uint32_t primask;
    __asm volatile ("mrs %0, primask" : "=r" (primask));
    return primask;
}

static inline void primask_set(uint32_t pri) {
    __asm volatile ("msr primask, %0" : : "r" (pri));
}

#define CRITICAL_SECTION_PM() \
    for (uint32_t _pm_save = primask_get(), ##__COUNTER__ = 0; \
         __COUNTER__ == 0; \
         primask_set(_pm_save), ##__COUNTER__ = 1) \
        for (primask_set(1); __COUNTER__ == 0; __COUNTER__ = 1)

// 使用
void foo(void) {
    CRITICAL_SECTION_PM() {
        // 临界区代码
        shared_var++;
    }
}

// 方法3: 使用LDREX/STREX指令 (原子操作，无需关中断)
// Cortex-M3/M4支持

// 原子加
uint32_t atomic_add(volatile uint32_t *ptr, uint32_t val) {
    uint32_t result;
    uint32_t tmp;

    __asm volatile (
        "1: ldrex %0, [%2]\n"
        "   add   %0, %0, %3\n"
        "   strex %1, %0, [%2]\n"
        "   teq   %1, #0\n"
        "   bne   1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr), "r" (val)
        : "cc", "memory"
    );
    return result;
}
