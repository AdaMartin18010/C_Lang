/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 120
 * Language: c
 * Block ID: d3ebcdd2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file startup_stm32f103.c
 * @brief STM32F103C8T6 启动代码
 * @details 包含复位处理、中断向量表、系统初始化
 */

#include <stdint.h>

/* ============================================================================
 * 链接器脚本定义的外部符号
 * ============================================================================ */

/* 初始化值存储在Flash中，运行时需要复制到RAM */
extern uint32_t _sidata;    /* .data段起始地址(Flash) */
extern uint32_t _sdata;     /* .data段起始地址(RAM) */
extern uint32_t _edata;     /* .data段结束地址(RAM) */

/* BSS段在RAM中，需要清零 */
extern uint32_t _sbss;      /* .bss段起始地址 */
extern uint32_t _ebss;      /* .bss段结束地址 */

/* 栈底初始化值 */
extern uint32_t _estack;    /* 栈顶地址 */

/* 主函数入口 */
extern int main(void);

/* 系统初始化函数(可选) */
extern void SystemInit(void);

/* ============================================================================
 * 函数原型声明
 * ============================================================================ */

/* 复位处理函数 */
void Reset_Handler(void);

/* 默认中断处理函数 */
void Default_Handler(void);

/* 弱定义的异常处理函数，可被用户覆盖 */
void NMI_Handler(void)          __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)          __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)      __attribute__((weak, alias("Default_Handler")));

/* 外部中断处理函数 */
void WWDG_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void TAMPER_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void RTC_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTCAlarm_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

/* ============================================================================
 * 中断向量表
 * ============================================================================ */

/**
 * @brief 中断向量表
 * @details 必须放在Flash的起始地址(0x0800_0000)
 *          第0项: 初始栈顶地址
 *          第1项: 复位处理函数地址
 *          第2-15项: Cortex-M3 系统异常
 *          第16项起: 外设中断
 */
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    /* Cortex-M3 异常 */
    (void (*)(void))&_estack,    /* 栈顶地址 */
    Reset_Handler,               /* 复位处理 */
    NMI_Handler,                 /* 非屏蔽中断 */
    HardFault_Handler,           /* 硬件错误 */
    MemManage_Handler,           /* 存储器管理错误 */
    BusFault_Handler,            /* 总线错误 */
    UsageFault_Handler,          /* 用法错误 */
    0, 0, 0, 0,                  /* 保留 */
    SVC_Handler,                 /* SVCall */
    DebugMon_Handler,            /* 调试监控 */
    0,                           /* 保留 */
    PendSV_Handler,              /* PendSV */
    SysTick_Handler,             /* SysTick */

    /* 外部中断 */
    WWDG_IRQHandler,             /* 窗口看门狗 */
    PVD_IRQHandler,              /* PVD通过EXTI线检测 */
    TAMPER_IRQHandler,           /* 篡改检测 */
    RTC_IRQHandler,              /* RTC全局中断 */
    FLASH_IRQHandler,            /* Flash全局中断 */
    RCC_IRQHandler,              /* RCC全局中断 */
    EXTI0_IRQHandler,            /* EXTI线0 */
    EXTI1_IRQHandler,            /* EXTI线1 */
    EXTI2_IRQHandler,            /* EXTI线2 */
    EXTI3_IRQHandler,            /* EXTI线3 */
    EXTI4_IRQHandler,            /* EXTI线4 */
    DMA1_Channel1_IRQHandler,    /* DMA1通道1 */
    DMA1_Channel2_IRQHandler,    /* DMA1通道2 */
    DMA1_Channel3_IRQHandler,    /* DMA1通道3 */
    DMA1_Channel4_IRQHandler,    /* DMA1通道4 */
    DMA1_Channel5_IRQHandler,    /* DMA1通道5 */
    DMA1_Channel6_IRQHandler,    /* DMA1通道6 */
    DMA1_Channel7_IRQHandler,    /* DMA1通道7 */
    ADC1_2_IRQHandler,           /* ADC1和ADC2全局中断 */
    USB_HP_CAN1_TX_IRQHandler,   /* USB高优先级/CAN1 TX */
    USB_LP_CAN1_RX0_IRQHandler,  /* USB低优先级/CAN1 RX0 */
    CAN1_RX1_IRQHandler,         /* CAN1 RX1 */
    CAN1_SCE_IRQHandler,         /* CAN1 SCE */
    EXTI9_5_IRQHandler,          /* EXTI线[9:5] */
    TIM1_BRK_IRQHandler,         /* TIM1中断 */
    TIM1_UP_IRQHandler,          /* TIM1更新 */
    TIM1_TRG_COM_IRQHandler,     /* TIM1触发和通信 */
    TIM1_CC_IRQHandler,          /* TIM1捕获比较 */
    TIM2_IRQHandler,             /* TIM2全局中断 */
    TIM3_IRQHandler,             /* TIM3全局中断 */
    TIM4_IRQHandler,             /* TIM4全局中断 */
    I2C1_EV_IRQHandler,          /* I2C1事件 */
    I2C1_ER_IRQHandler,          /* I2C1错误 */
    I2C2_EV_IRQHandler,          /* I2C2事件 */
    I2C2_ER_IRQHandler,          /* I2C2错误 */
    SPI1_IRQHandler,             /* SPI1全局中断 */
    SPI2_IRQHandler,             /* SPI2全局中断 */
    USART1_IRQHandler,           /* USART1全局中断 */
    USART2_IRQHandler,           /* USART2全局中断 */
    USART3_IRQHandler,           /* USART3全局中断 */
    EXTI15_10_IRQHandler,        /* EXTI线[15:10] */
    RTCAlarm_IRQHandler,         /* RTC闹钟通过EXTI线 */
    USBWakeUp_IRQHandler,        /* USB唤醒通过EXTI线 */
};

/* ============================================================================
 * 复位处理函数
 * ============================================================================ */

/**
 * @brief 复位处理函数
 * @details 上电或复位后第一个执行的函数
 *          1. 初始化.data段(从Flash复制到RAM)
 *          2. 清零.bss段
 *          3. 调用系统初始化
 *          4. 跳转到main函数
 */
void Reset_Handler(void)
{
    /* 将.data段从Flash复制到RAM */
    uint32_t *pSrc = &_sidata;
    uint32_t *pDest = &_sdata;

    while (pDest < &_edata) {
        *pDest++ = *pSrc++;
    }

    /* 清零.bss段 */
    pDest = &_sbss;
    while (pDest < &_ebss) {
        *pDest++ = 0;
    }

    /* 调用系统初始化(时钟配置等) */
    /* SystemInit(); */

    /* 跳转到main函数 */
    main();

    /* main不应返回，如果返回则进入无限循环 */
    while (1) {
        __asm volatile ("nop");
    }
}

/**
 * @brief 默认中断处理函数
 * @details 未定义的中断处理函数都指向这里
 */
void Default_Handler(void)
{
    /* 无限循环，便于调试 */
    while (1) {
        __asm volatile ("nop");
    }
}

/* ============================================================================
 * 附加功能: 软件复位
 * ============================================================================ */

/* Cortex-M3 应用中断和复位控制寄存器 */
#define SCB_AIRCR   (*(volatile uint32_t *)0xE000ED0C)

#define SCB_AIRCR_VECTKEY_POS       16
#define SCB_AIRCR_VECTKEY_MASK      (0xFFFFUL << SCB_AIRCR_VECTKEY_POS)
#define SCB_AIRCR_VECTKEYSTAT_POS   16
#define SCB_AIRCR_VECTKEYSTAT_MASK  (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_POS)
#define SCB_AIRCR_ENDIANNESS_POS    15
#define SCB_AIRCR_ENDIANNESS_MASK   (1UL << SCB_AIRCR_ENDIANNESS_POS)
#define SCB_AIRCR_PRIGROUP_POS      8
#define SCB_AIRCR_PRIGROUP_MASK     (7UL << SCB_AIRCR_PRIGROUP_POS)
#define SCB_AIRCR_SYSRESETREQ_POS   2
#define SCB_AIRCR_SYSRESETREQ_MASK  (1UL << SCB_AIRCR_SYSRESETREQ_POS)
#define SCB_AIRCR_VECTCLRACTIVE_POS 1
#define SCB_AIRCR_VECTCLRACTIVE_MASK (1UL << SCB_AIRCR_VECTCLRACTIVE_POS)
#define SCB_AIRCR_VECTRESET_POS     0
#define SCB_AIRCR_VECTRESET_MASK    (1UL << SCB_AIRCR_VECTRESET_POS)

/**
 * @brief 系统复位函数
 * @details 通过写入AIRCR寄存器请求系统复位
 */
void NVIC_SystemReset(void)
{
    /* 写入复位密钥和复位请求位 */
    SCB_AIRCR = (0x05FAUL << SCB_AIRCR_VECTKEY_POS) |
                SCB_AIRCR_SYSRESETREQ_MASK;

    /* 等待复位完成 */
    __asm volatile ("dsb");

    /* 无限循环防止继续执行 */
    while (1) {
        __asm volatile ("nop");
    }
}

/* ============================================================================
 * 附加功能: 中断使能/禁用
 * ============================================================================ */

/* 中断控制状态寄存器 */
#define SCB_ICSR    (*(volatile uint32_t *)0xE000ED04)

/* 中断使能寄存器 */
#define NVIC_ISER0  (*(volatile uint32_t *)0xE000E100)
#define NVIC_ISER1  (*(volatile uint32_t *)0xE000E104)

/* 中断禁用寄存器 */
#define NVIC_ICER0  (*(volatile uint32_t *)0xE000E180)
#define NVIC_ICER1  (*(volatile uint32_t *)0xE000E184)

/* 中断挂起设置寄存器 */
#define NVIC_ISPR0  (*(volatile uint32_t *)0xE000E200)
#define NVIC_ISPR1  (*(volatile uint32_t *)0xE000E204)

/* 中断挂起清除寄存器 */
#define NVIC_ICPR0  (*(volatile uint32_t *)0xE000E280)
#define NVIC_ICPR1  (*(volatile uint32_t *)0xE000E284)

/* 中断活动位寄存器 */
#define NVIC_IABR0  (*(volatile uint32_t *)0xE000E300)
#define NVIC_IABR1  (*(volatile uint32_t *)0xE000E304)

/* 中断优先级寄存器 */
#define NVIC_IPR_BASE   ((volatile uint8_t *)0xE000E400)

/**
 * @brief 使能中断
 * @param IRQn 中断号(0-67)
 */
void NVIC_EnableIRQ(uint8_t IRQn)
{
    if (IRQn < 32) {
        NVIC_ISER0 = (1UL << IRQn);
    } else if (IRQn < 64) {
        NVIC_ISER1 = (1UL << (IRQn - 32));
    }
}

/**
 * @brief 禁用中断
 * @param IRQn 中断号(0-67)
 */
void NVIC_DisableIRQ(uint8_t IRQn)
{
    if (IRQn < 32) {
        NVIC_ICER0 = (1UL << IRQn);
    } else if (IRQn < 64) {
        NVIC_ICER1 = (1UL << (IRQn - 32));
    }
}

/**
 * @brief 设置中断优先级
 * @param IRQn 中断号
 * @param priority 优先级(0-15，数值越小优先级越高)
 * @note STM32F103使用4位优先级，NVIC_PRIO_BITS = 4
 */
void NVIC_SetPriority(uint8_t IRQn, uint8_t priority)
{
    /* STM32F103: 4位优先级，左移4位对齐 */
    NVIC_IPR_BASE[IRQn] = (priority << 4) & 0xFF;
}

/**
 * @brief 设置中断优先级分组
 * @param PriorityGroup 优先级分组
 *        - 0: 0位抢占优先级，4位响应优先级
 *        - 1: 1位抢占优先级，3位响应优先级
 *        - 2: 2位抢占优先级，2位响应优先级
 *        - 3: 3位抢占优先级，1位响应优先级
 *        - 4: 4位抢占优先级，0位响应优先级
 */
void NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
    uint32_t reg_value;
    uint32_t PriorityGroupTmp = (PriorityGroup & 0x07UL);

    reg_value  = SCB_AIRCR;
    reg_value &= ~(SCB_AIRCR_VECTKEY_MASK | SCB_AIRCR_PRIGROUP_MASK);
    reg_value  = (reg_value |
                  (0x05FAUL << SCB_AIRCR_VECTKEY_POS) |
                  (PriorityGroupTmp << SCB_AIRCR_PRIGROUP_POS));
    SCB_AIRCR = reg_value;
}
