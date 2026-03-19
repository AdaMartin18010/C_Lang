/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 790
 * Language: c
 * Block ID: bd1f2b47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file nvic_config.c
 * @brief NVIC中断控制器配置
 */

#include <stdint.h>

/* ============================================================================
 * 寄存器定义
 * ============================================================================ */

/* NVIC 寄存器基地址 */
#define NVIC_BASE           0xE000E100
#define SCB_BASE            0xE000ED00

/* NVIC 寄存器 */
#define NVIC_ISER(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x00 + (n) * 4))
#define NVIC_ICER(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x80 + (n) * 4))
#define NVIC_ISPR(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x100 + (n) * 4))
#define NVIC_ICPR(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x180 + (n) * 4))
#define NVIC_IABR(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x200 + (n) * 4))
#define NVIC_IPR(n)         (*(volatile uint8_t  *)(NVIC_BASE + 0x300 + (n)))

/* SCB 寄存器 */
#define SCB_CPUID           (*(volatile uint32_t *)(SCB_BASE + 0x00))
#define SCB_ICSR            (*(volatile uint32_t *)(SCB_BASE + 0x04))
#define SCB_VTOR            (*(volatile uint32_t *)(SCB_BASE + 0x08))
#define SCB_AIRCR           (*(volatile uint32_t *)(SCB_BASE + 0x0C))
#define SCB_SCR             (*(volatile uint32_t *)(SCB_BASE + 0x10))
#define SCB_CCR             (*(volatile uint32_t *)(SCB_BASE + 0x14))
#define SCB_SHPR(n)         (*(volatile uint8_t  *)(SCB_BASE + 0x18 + (n)))
#define SCB_SHCSR           (*(volatile uint32_t *)(SCB_BASE + 0x24))

/* ============================================================================
 * 中断号定义
 * ============================================================================ */

typedef enum {
    /* Cortex-M3 内部异常 */
    NonMaskableInt_IRQn     = -14,  /* 非屏蔽中断 */
    HardFault_IRQn          = -13,  /* 硬件错误 */
    MemoryManagement_IRQn   = -12,  /* 存储器管理错误 */
    BusFault_IRQn           = -11,  /* 总线错误 */
    UsageFault_IRQn         = -10,  /* 用法错误 */
    SVCall_IRQn             = -5,   /* SVCall */
    DebugMonitor_IRQn       = -4,   /* 调试监控 */
    PendSV_IRQn             = -2,   /* PendSV */
    SysTick_IRQn            = -1,   /* SysTick */

    /* STM32 外部中断 */
    WWDG_IRQn               = 0,    /* 窗口看门狗 */
    PVD_IRQn                = 1,    /* PVD */
    TAMPER_IRQn             = 2,    /* 篡改检测 */
    RTC_IRQn                = 3,    /* RTC */
    FLASH_IRQn              = 4,    /* Flash */
    RCC_IRQn                = 5,    /* RCC */
    EXTI0_IRQn              = 6,    /* EXTI线0 */
    EXTI1_IRQn              = 7,    /* EXTI线1 */
    EXTI2_IRQn              = 8,    /* EXTI线2 */
    EXTI3_IRQn              = 9,    /* EXTI线3 */
    EXTI4_IRQn              = 10,   /* EXTI线4 */
    DMA1_Channel1_IRQn      = 11,   /* DMA1通道1 */
    DMA1_Channel2_IRQn      = 12,   /* DMA1通道2 */
    DMA1_Channel3_IRQn      = 13,   /* DMA1通道3 */
    DMA1_Channel4_IRQn      = 14,   /* DMA1通道4 */
    DMA1_Channel5_IRQn      = 15,   /* DMA1通道5 */
    DMA1_Channel6_IRQn      = 16,   /* DMA1通道6 */
    DMA1_Channel7_IRQn      = 17,   /* DMA1通道7 */
    ADC1_2_IRQn             = 18,   /* ADC1和ADC2 */
    USB_HP_CAN1_TX_IRQn     = 19,   /* USB高优先级/CAN1 TX */
    USB_LP_CAN1_RX0_IRQn    = 20,   /* USB低优先级/CAN1 RX0 */
    CAN1_RX1_IRQn           = 21,   /* CAN1 RX1 */
    CAN1_SCE_IRQn           = 22,   /* CAN1 SCE */
    EXTI9_5_IRQn            = 23,   /* EXTI线[9:5] */
    TIM1_BRK_IRQn           = 24,   /* TIM1中断 */
    TIM1_UP_IRQn            = 25,   /* TIM1更新 */
    TIM1_TRG_COM_IRQn       = 26,   /* TIM1触发和通信 */
    TIM1_CC_IRQn            = 27,   /* TIM1捕获比较 */
    TIM2_IRQn               = 28,   /* TIM2 */
    TIM3_IRQn               = 29,   /* TIM3 */
    TIM4_IRQn               = 30,   /* TIM4 */
    I2C1_EV_IRQn            = 31,   /* I2C1事件 */
    I2C1_ER_IRQn            = 32,   /* I2C1错误 */
    I2C2_EV_IRQn            = 33,   /* I2C2事件 */
    I2C2_ER_IRQn            = 34,   /* I2C2错误 */
    SPI1_IRQn               = 35,   /* SPI1 */
    SPI2_IRQn               = 36,   /* SPI2 */
    USART1_IRQn             = 37,   /* USART1 */
    USART2_IRQn             = 38,   /* USART2 */
    USART3_IRQn             = 39,   /* USART3 */
    EXTI15_10_IRQn          = 40,   /* EXTI线[15:10] */
    RTCAlarm_IRQn           = 41,   /* RTC闹钟 */
    USBWakeUp_IRQn          = 42,   /* USB唤醒 */
} IRQn_Type;

/* ============================================================================
 * NVIC 操作函数
 * ============================================================================ */

/**
 * @brief 使能指定中断
 * @param IRQn 中断号
 */
static inline void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC_ISER((uint32_t)IRQn >> 5) = (1UL << ((uint32_t)IRQn & 0x1F));
    }
}

/**
 * @brief 禁用指定中断
 * @param IRQn 中断号
 */
static inline void NVIC_DisableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC_ICER((uint32_t)IRQn >> 5) = (1UL << ((uint32_t)IRQn & 0x1F));
    }
}

/**
 * @brief 获取中断使能状态
 * @param IRQn 中断号
 * @return 0=禁用, 1=使能
 */
static inline uint32_t NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        return ((NVIC_ISER((uint32_t)IRQn >> 5) &
                 (1UL << ((uint32_t)IRQn & 0x1F))) != 0UL) ? 1UL : 0UL;
    }
    return 0UL;
}

/**
 * @brief 设置中断挂起状态
 * @param IRQn 中断号
 */
static inline void NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC_ISPR((uint32_t)IRQn >> 5) = (1UL << ((uint32_t)IRQn & 0x1F));
    }
}

/**
 * @brief 清除中断挂起状态
 * @param IRQn 中断号
 */
static inline void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC_ICPR((uint32_t)IRQn >> 5) = (1UL << ((uint32_t)IRQn & 0x1F));
    }
}

/**
 * @brief 获取中断挂起状态
 * @param IRQn 中断号
 * @return 0=未挂起, 1=挂起
 */
static inline uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        return ((NVIC_ISPR((uint32_t)IRQn >> 5) &
                 (1UL << ((uint32_t)IRQn & 0x1F))) != 0UL) ? 1UL : 0UL;
    }
    return 0UL;
}

/**
 * @brief 获取中断活动状态
 * @param IRQn 中断号
 * @return 0=未活动, 1=活动
 */
static inline uint32_t NVIC_GetActive(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        return ((NVIC_IABR((uint32_t)IRQn >> 5) &
                 (1UL << ((uint32_t)IRQn & 0x1F))) != 0UL) ? 1UL : 0UL;
    }
    return 0UL;
}

/**
 * @brief 设置中断优先级
 * @param IRQn 中断号
 * @param priority 优先级(0-15，数值越小优先级越高)
 */
static inline void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
    if ((int32_t)IRQn >= 0) {
        /* 外部中断: 使用NVIC_IPR */
        NVIC_IPR((uint32_t)IRQn) = (uint8_t)((priority << 4) & 0xFF);
    } else {
        /* 内部异常: 使用SCB_SHPR */
        SCB_SHPR(((uint32_t)IRQn & 0xFUL) - 4UL) =
            (uint8_t)((priority << 4) & 0xFF);
    }
}

/**
 * @brief 获取中断优先级
 * @param IRQn 中断号
 * @return 优先级值
 */
static inline uint32_t NVIC_GetPriority(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        return ((uint32_t)NVIC_IPR((uint32_t)IRQn) >> 4);
    } else {
        return ((uint32_t)SCB_SHPR(((uint32_t)IRQn & 0xFUL) - 4UL) >> 4);
    }
}

/**
 * @brief 设置中断优先级分组
 * @param PriorityGroup 优先级分组(0-7)
 * @note STM32F103使用4位优先级，实际有效值为3-7
 */
static inline void NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
    uint32_t reg_value;
    uint32_t PriorityGroupTmp = (PriorityGroup & 0x07UL);

    reg_value  = SCB_AIRCR;
    reg_value &= ~(0xFFFFUL << 16) | (7UL << 8);
    reg_value  = (reg_value |
                  (0x05FAUL << 16) |
                  (PriorityGroupTmp << 8));
    SCB_AIRCR = reg_value;
}

/**
 * @brief 获取中断优先级分组
 * @return 优先级分组值
 */
static inline uint32_t NVIC_GetPriorityGrouping(void)
{
    return ((SCB_AIRCR & (7UL << 8)) >> 8);
}

/**
 * @brief 系统复位
 */
static inline void NVIC_SystemReset(void)
{
    SCB_AIRCR = (0x05FAUL << 16) | (1UL << 2);
    __asm volatile ("dsb");
    while (1) { __asm volatile ("nop"); }
}

/* ============================================================================
 * SysTick 定时器配置
 * ============================================================================ */

#define SysTick_BASE        0xE000E010

#define SysTick_CTRL        (*(volatile uint32_t *)(SysTick_BASE + 0x00))
#define SysTick_LOAD        (*(volatile uint32_t *)(SysTick_BASE + 0x04))
#define SysTick_VAL         (*(volatile uint32_t *)(SysTick_BASE + 0x08))
#define SysTick_CALIB       (*(volatile uint32_t *)(SysTick_BASE + 0x0C))

/* SysTick控制寄存器位定义 */
#define SysTick_CTRL_COUNTFLAG_Pos  16
#define SysTick_CTRL_CLKSOURCE_Pos  2
#define SysTick_CTRL_TICKINT_Pos    1
#define SysTick_CTRL_ENABLE_Pos     0

/**
 * @brief 配置SysTick定时器
 * @param ticks 重装载值(时钟周期数)
 * @return 0=成功, 1=失败(重装载值超出范围)
 */
static inline uint32_t SysTick_Config(uint32_t ticks)
{
    /* 重装载值必须在1-0xFFFFFF之间 */
    if ((ticks - 1) > 0xFFFFFFUL) {
        return 1;
    }

    /* 设置重装载寄存器 */
    SysTick_LOAD = ticks - 1;

    /* 清零当前值寄存器 */
    SysTick_VAL = 0;

    /* 使能SysTick，使用处理器时钟，使能中断 */
    SysTick_CTRL = (1UL << SysTick_CTRL_CLKSOURCE_Pos) |
                   (1UL << SysTick_CTRL_TICKINT_Pos) |
                   (1UL << SysTick_CTRL_ENABLE_Pos);

    return 0;
}

/**
 * @brief 获取SysTick当前值
 * @return 当前计数值
 */
static inline uint32_t SysTick_GetValue(void)
{
    return SysTick_VAL;
}

/**
 * @brief 检查SysTick是否计数到0
 * @return 1=已计数到0, 0=未计数到0
 */
static inline uint32_t SysTick_GetCountFlag(void)
{
    return ((SysTick_CTRL >> SysTick_CTRL_COUNTFLAG_Pos) & 1UL);
}

/**
 * @brief 禁用SysTick
 */
static inline void SysTick_Disable(void)
{
    SysTick_CTRL &= ~(1UL << SysTick_CTRL_ENABLE_Pos);
}

/**
 * @brief 使能SysTick
 */
static inline void SysTick_Enable(void)
{
    SysTick_CTRL |= (1UL << SysTick_CTRL_ENABLE_Pos);
}
