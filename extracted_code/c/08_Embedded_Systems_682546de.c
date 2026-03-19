/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 302
 * Language: c
 * Block ID: 682546de
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Cortex-M4 向量表
typedef void (* const Handler)(void);

__attribute__((section(".isr_vector")))
Handler vector_table[] = {
    // Cortex-M 系统异常 (前16个)
    (Handler)&_estack,          // 0: 初始栈指针
    Reset_Handler,               // 1: 复位
    NMI_Handler,                 // 2: 不可屏蔽中断
    HardFault_Handler,           // 3: 硬故障
    MemManage_Handler,           // 4: 内存管理故障
    BusFault_Handler,            // 5: 总线故障
    UsageFault_Handler,          // 6: 使用故障
    0, 0, 0, 0,                  // 7-10: 保留
    SVC_Handler,                 // 11: SVCall
    DebugMon_Handler,            // 12: 调试监控
    0,                           // 13: 保留
    PendSV_Handler,              // 14: PendSV
    SysTick_Handler,             // 15: SysTick

    // 外设中断 (设备特定)
    WWDG_IRQHandler,             // 0: 窗口看门狗
    PVD_IRQHandler,              // 1: PVD
    TAMP_STAMP_IRQHandler,       // 2: RTC Tamper
    // ... 更多中断
};
