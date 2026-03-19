/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 1996
 * Language: c
 * Block ID: f51626df
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file timer_driver.c
 * @brief STM32F103 定时器驱动程序
 * @details 提供基础定时器(TIM2/3/4)和通用定时器的完整驱动
 */

#include <stdint.h>

/* ============================================================================
 * 定时器寄存器定义
 * ============================================================================ */

typedef struct {
    volatile uint32_t CR1;      /* 控制寄存器1 (0x00) */
    volatile uint32_t CR2;      /* 控制寄存器2 (0x04) */
    volatile uint32_t SMCR;     /* 从模式控制寄存器 (0x08) */
    volatile uint32_t DIER;     /* DMA/中断使能寄存器 (0x0C) */
    volatile uint32_t SR;       /* 状态寄存器 (0x10) */
    volatile uint32_t EGR;      /* 事件生成寄存器 (0x14) */
    volatile uint32_t CCMR1;    /* 捕获/比较模式寄存器1 (0x18) */
    volatile uint32_t CCMR2;    /* 捕获/比较模式寄存器2 (0x1C) */
    volatile uint32_t CCER;     /* 捕获/比较使能寄存器 (0x20) */
    volatile uint32_t CNT;      /* 计数器 (0x24) */
    volatile uint32_t PSC;      /* 预分频器 (0x28) */
    volatile uint32_t ARR;      /* 自动重装载寄存器 (0x2C) */
    volatile uint32_t RCR;      /* 重复计数寄存器 (TIM1 only, 0x30) */
    volatile uint32_t CCR1;     /* 捕获/比较寄存器1 (0x34) */
    volatile uint32_t CCR2;     /* 捕获/比较寄存器2 (0x38) */
    volatile uint32_t CCR3;     /* 捕获/比较寄存器3 (0x3C) */
    volatile uint32_t CCR4;     /* 捕获/比较寄存器4 (0x40) */
    volatile uint32_t BDTR;     /* 刹车和死区寄存器 (TIM1 only, 0x44) */
    volatile uint32_t DCR;      /* DMA控制寄存器 (0x48) */
    volatile uint32_t DMAR;     /* DMA地址全传输寄存器 (0x4C) */
} TIM_TypeDef;

/* 定时器基地址 */
#define TIM1_BASE           0x40010000
#define TIM2_BASE           0x40000000
#define TIM3_BASE           0x40000400
#define TIM4_BASE           0x40000800
#define TIM5_BASE           0x40000C00
#define TIM6_BASE           0x40001000
#define TIM7_BASE           0x40001400
#define TIM8_BASE           0x40010400

#define TIM1                ((TIM_TypeDef *)TIM1_BASE)
#define TIM2                ((TIM_TypeDef *)TIM2_BASE)
#define TIM3                ((TIM_TypeDef *)TIM3_BASE)
#define TIM4                ((TIM_TypeDef *)TIM4_BASE)
#define TIM5                ((TIM_TypeDef *)TIM5_BASE)
#define TIM6                ((TIM_TypeDef *)TIM6_BASE)
#define TIM7                ((TIM_TypeDef *)TIM7_BASE)
#define TIM8                ((TIM_TypeDef *)TIM8_BASE)

/* ============================================================================
 * RCC 时钟使能 (与前面定义相同，此处简化)
 * ============================================================================ */
#define RCC_APB1ENR         (*(volatile uint32_t *)0x4002101C)
#define RCC_APB2ENR         (*(volatile uint32_t *)0x40021018)

#define RCC_APB1ENR_TIM2    (1 << 0)
#define RCC_APB1ENR_TIM3    (1 << 1)
#define RCC_APB1ENR_TIM4    (1 << 2)
#define RCC_APB1ENR_TIM5    (1 << 3)
#define RCC_APB1ENR_TIM6    (1 << 4)
#define RCC_APB1ENR_TIM7    (1 << 5)

#define RCC_APB2ENR_TIM1    (1 << 11)
#define RCC_APB2ENR_TIM8    (1 << 13)

/* ============================================================================
 * 定时器CR1寄存器位定义
 * ============================================================================ */
#define TIM_CR1_CEN_Pos     0   /* 计数器使能 */
#define TIM_CR1_UDIS_Pos    1   /* 更新禁用 */
#define TIM_CR1_URS_Pos     2   /* 更新请求源 */
#define TIM_CR1_OPM_Pos     3   /* 单脉冲模式 */
#define TIM_CR1_DIR_Pos     4   /* 计数方向 (仅通用定时器) */
#define TIM_CR1_CMS_Pos     5   /* 中央对齐模式选择 */
#define TIM_CR1_ARPE_Pos    7   /* 自动重装载预装载使能 */
#define TIM_CR1_CKD_Pos     8   /* 时钟分频 */

/* DIER寄存器位定义 */
#define TIM_DIER_UIE_Pos    0   /* 更新中断使能 */
#define TIM_DIER_CC1IE_Pos  1   /* 捕获/比较1中断使能 */
#define TIM_DIER_CC2IE_Pos  2   /* 捕获/比较2中断使能 */
#define TIM_DIER_CC3IE_Pos  3   /* 捕获/比较3中断使能 */
#define TIM_DIER_CC4IE_Pos  4   /* 捕获/比较4中断使能 */
#define TIM_DIER_TIE_Pos    6   /* 触发中断使能 */

/* SR寄存器位定义 */
#define TIM_SR_UIF_Pos      0   /* 更新中断标志 */
#define TIM_SR_CC1IF_Pos    1   /* 捕获/比较1中断标志 */
#define TIM_SR_CC2IF_Pos    2   /* 捕获/比较2中断标志 */
#define TIM_SR_CC3IF_Pos    3   /* 捕获/比较3中断标志 */
#define TIM_SR_CC4IF_Pos    4   /* 捕获/比较4中断标志 */

/* EGR寄存器位定义 */
#define TIM_EGR_UG_Pos      0   /* 更新事件生成 */

/* ============================================================================
 * 定时器配置结构体
 * ============================================================================ */

typedef enum {
    TIM_MODE_BASIC,         /* 基础定时器模式 */
    TIM_MODE_PWM,           /* PWM输出模式 */
    TIM_MODE_INPUT_CAPTURE, /* 输入捕获模式 */
    TIM_MODE_OUTPUT_COMPARE /* 输出比较模式 */
} TIM_ModeTypeDef;

typedef struct {
    TIM_TypeDef *TIMx;      /* 定时器指针 */
    uint16_t Prescaler;     /* 预分频值 (0-65535) */
    uint16_t Period;        /* 周期/重装载值 (0-65535) */
    TIM_ModeTypeDef Mode;   /* 工作模式 */
    uint8_t InterruptEnable;/* 是否使能中断 */
} TIM_ConfigTypeDef;

/* ============================================================================
 * 内部辅助函数
 * ============================================================================ */

/**
 * @brief 使能定时器时钟
 * @param TIMx 定时器指针
 */
static void TIM_EnableClock(TIM_TypeDef *TIMx)
{
    if (TIMx == TIM1) {
        RCC_APB2ENR |= RCC_APB2ENR_TIM1;
    } else if (TIMx == TIM2) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM2;
    } else if (TIMx == TIM3) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM3;
    } else if (TIMx == TIM4) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM4;
    } else if (TIMx == TIM5) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM5;
    } else if (TIMx == TIM6) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM6;
    } else if (TIMx == TIM7) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM7;
    } else if (TIMx == TIM8) {
        RCC_APB2ENR |= RCC_APB2ENR_TIM8;
    }
}

/**
 * @brief 获取定时器时钟频率
 * @param TIMx 定时器指针
 * @return 定时器时钟频率(Hz)
 * @note 假设系统时钟为72MHz
 */
static uint32_t TIM_GetClockFreq(TIM_TypeDef *TIMx)
{
    /*
     * TIM1和TIM8在APB2上，最高72MHz
     * TIM2-7在APB1上，最高36MHz
     */
    if (TIMx == TIM1 || TIMx == TIM8) {
        return 72000000;    /* 72MHz */
    } else {
        return 36000000;    /* 36MHz */
    }
}

/* ============================================================================
 * 基础定时器功能
 * ============================================================================ */

/**
 * @brief 初始化基础定时器(时基模式)
 * @param TIMx 定时器指针
 * @param Prescaler 预分频值
 * @param Period 周期值
 * @details 定时频率 = 定时器时钟 / ((Prescaler+1) * (Period+1))
 *          使用示例:
 *          TIM_BaseInit(TIM2, 35999, 999);  // 1ms中断 (36MHz/(36000*1000)=1Hz)
 */
void TIM_BaseInit(TIM_TypeDef *TIMx, uint16_t Prescaler, uint16_t Period)
{
    /* 使能时钟 */
    TIM_EnableClock(TIMx);

    /* 禁用定时器 */
    TIMx->CR1 &= ~(1 << TIM_CR1_CEN_Pos);

    /* 设置预分频器 */
    TIMx->PSC = Prescaler;

    /* 设置自动重装载值 */
    TIMx->ARR = Period;

    /* 使能自动重装载预装载 */
    TIMx->CR1 |= (1 << TIM_CR1_ARPE_Pos);

    /* 产生更新事件，重新装载寄存器 */
    TIMx->EGR |= (1 << TIM_EGR_UG_Pos);

    /* 清除更新标志 */
    TIMx->SR &= ~(1 << TIM_SR_UIF_Pos);
}

/**
 * @brief 初始化定时器以产生特定频率
 * @param TIMx 定时器指针
 * @param FreqHz 目标频率(Hz)
 * @return 实际配置频率(Hz)
 * @note 最大误差约0.1%，适用于音频、PWM等应用
 */
uint32_t TIM_InitForFrequency(TIM_TypeDef *TIMx, uint32_t FreqHz)
{
    uint32_t timerClk = TIM_GetClockFreq(TIMx);
    uint32_t period, prescaler;

    /* 计算预分频和周期，使误差最小 */
    /* timerClk / ((prescaler+1) * (period+1)) = FreqHz */

    if (FreqHz > timerClk) {
        FreqHz = timerClk;
    }

    /* 计算总分频系数 */
    uint32_t totalDiv = timerClk / FreqHz;

    if (totalDiv > 65536 * 65536) {
        /* 超出范围，使用最大值 */
        prescaler = 65535;
        period = 65535;
    } else {
        /* 寻找最佳分频组合 */
        /* 优先使用较小的预分频以获得更高分辨率 */
        prescaler = 0;
        period = totalDiv - 1;

        while (period > 65535 && prescaler < 65535) {
            prescaler++;
            period = (totalDiv / (prescaler + 1)) - 1;
        }

        if (prescaler > 65535) {
            prescaler = 65535;
            period = 65535;
        }
    }

    /* 初始化定时器 */
    TIM_BaseInit(TIMx, (uint16_t)prescaler, (uint16_t)period);

    /* 返回实际频率 */
    return timerClk / ((prescaler + 1) * (period + 1));
}

/**
 * @brief 启动定时器
 * @param TIMx 定时器指针
 */
static inline void TIM_Start(TIM_TypeDef *TIMx)
{
    TIMx->CR1 |= (1 << TIM_CR1_CEN_Pos);
}

/**
 * @brief 停止定时器
 * @param TIMx 定时器指针
 */
static inline void TIM_Stop(TIM_TypeDef *TIMx)
{
    TIMx->CR1 &= ~(1 << TIM_CR1_CEN_Pos);
}

/**
 * @brief 获取当前计数值
 * @param TIMx 定时器指针
 * @return 当前计数器值
 */
static inline uint16_t TIM_GetCounter(TIM_TypeDef *TIMx)
{
    return (uint16_t)TIMx->CNT;
}

/**
 * @brief 设置当前计数值
 * @param TIMx 定时器指针
 * @param Value 要设置的值
 */
static inline void TIM_SetCounter(TIM_TypeDef *TIMx, uint16_t Value)
{
    TIMx->CNT = Value;
}

/**
 * @brief 获取自动重装载值
 * @param TIMx 定时器指针
 * @return ARR值
 */
static inline uint16_t TIM_GetPeriod(TIM_TypeDef *TIMx)
{
    return (uint16_t)TIMx->ARR;
}

/**
 * @brief 设置自动重装载值
 * @param TIMx 定时器指针
 * @param Period 新的周期值
 */
static inline void TIM_SetPeriod(TIM_TypeDef *TIMx, uint16_t Period)
{
    TIMx->ARR = Period;
}

/* ============================================================================
 * 定时器中断功能
 * ============================================================================ */

/**
 * @brief 使能定时器更新中断
 * @param TIMx 定时器指针
 */
static inline void TIM_EnableIT_Update(TIM_TypeDef *TIMx)
{
    TIMx->DIER |= (1 << TIM_DIER_UIE_Pos);
}

/**
 * @brief 禁用定时器更新中断
 * @param TIMx 定时器指针
 */
static inline void TIM_DisableIT_Update(TIM_TypeDef *TIMx)
{
    TIMx->DIER &= ~(1 << TIM_DIER_UIE_Pos);
}

/**
 * @brief 检查更新中断标志
 * @param TIMx 定时器指针
 * @return 1=有更新中断, 0=无
 */
static inline uint8_t TIM_GetITStatus_Update(TIM_TypeDef *TIMx)
{
    return (TIMx->SR & (1 << TIM_SR_UIF_Pos)) ? 1 : 0;
}

/**
 * @brief 清除更新中断标志
 * @param TIMx 定时器指针
 */
static inline void TIM_ClearITPendingBit_Update(TIM_TypeDef *TIMx)
{
    TIMx->SR &= ~(1 << TIM_SR_UIF_Pos);
}

/**
 * @brief 生成软件更新事件
 * @param TIMx 定时器指针
 */
static inline void TIM_GenerateEvent_Update(TIM_TypeDef *TIMx)
{
    TIMx->EGR |= (1 << TIM_EGR_UG_Pos);
}

/* ============================================================================
 * 毫秒延时定时器 (基于TIM2)
 * ============================================================================ */

static volatile uint32_t g_tim2_ms_count = 0;

/**
 * @brief TIM2中断处理函数
 * @note 需要在启动文件中定义TIM2_IRQHandler指向此函数
 */
void TIM2_IRQHandler_User(void)
{
    if (TIM_GetITStatus_Update(TIM2)) {
        TIM_ClearITPendingBit_Update(TIM2);
        g_tim2_ms_count++;
    }
}

/**
 * @brief 初始化毫秒延时定时器
 * @details 配置TIM2产生1ms周期中断
 */
void TIM_Delay_Init(void)
{
    /* 配置TIM2: 36MHz / 36000 = 1kHz (1ms) */
    TIM_BaseInit(TIM2, 35999, 0);

    /* 使能更新中断 */
    TIM_EnableIT_Update(TIM2);

    /* 启动定时器 */
    TIM_Start(TIM2);
}

/**
 * @brief 获取毫秒计数
 * @return 从启动开始的毫秒数
 */
uint32_t TIM_GetMillis(void)
{
    return g_tim2_ms_count;
}

/**
 * @brief 毫秒延时
 * @param ms 延时毫秒数
 */
void TIM_Delay_ms(uint32_t ms)
{
    uint32_t start = g_tim2_ms_count;
    while ((g_tim2_ms_count - start) < ms) {
        __asm volatile ("nop");
    }
}

/**
 * @brief 微秒延时(忙等待)
 * @param us 延时微秒数
 * @note 仅适用于短延时，会阻塞CPU
 */
void TIM_Delay_us(uint32_t us)
{
    /* 假设72MHz，约72个时钟周期1us */
    /* 使用简单的循环，需要根据实际情况校准 */
    for (volatile uint32_t i = 0; i < us * 8; i++) {
        __asm volatile ("nop");
    }
}

/* ============================================================================
 * PWM功能 (TIM2-TIM4)
 * ============================================================================ */

/**
 * @brief 初始化PWM输出
 * @param TIMx 定时器指针
 * @param Channel PWM通道 (1-4)
 * @param Frequency PWM频率(Hz)
 * @param DutyCycle 初始占空比 (0-1000，表示0.0%-100.0%)
 * @details 使用示例:
 *          TIM_PWM_Init(TIM2, 1, 1000, 500);  // PA0输出1kHz PWM, 50%占空比
 */
void TIM_PWM_Init(TIM_TypeDef *TIMx, uint8_t Channel,
                  uint32_t Frequency, uint16_t DutyCycle)
{
    uint32_t timerClk = TIM_GetClockFreq(TIMx);
    uint32_t period = (timerClk / Frequency) - 1;

    /* 限制周期值 */
    if (period > 65535) period = 65535;

    /* 基础定时器初始化 (预分频=0) */
    TIM_BaseInit(TIMx, 0, (uint16_t)period);

    /* 配置PWM模式 */
    uint32_t ccmr = 0;
    uint32_t ccer = 0;

    switch (Channel) {
        case 1:
            /* 通道1: PWM模式1, 预装载使能 */
            ccmr = (6 << 4) | (1 << 3);  /* OC1M = 110 (PWM模式1), OC1PE = 1 */
            TIMx->CCMR1 = (TIMx->CCMR1 & 0xFF00) | ccmr;
            ccer = (1 << 0);  /* CC1E = 1 (输出使能) */
            TIMx->CCER |= ccer;
            /* 设置占空比 */
            TIMx->CCR1 = (period * DutyCycle) / 1000;
            break;

        case 2:
            /* 通道2 */
            ccmr = (6 << 12) | (1 << 11);  /* OC2M = 110, OC2PE = 1 */
            TIMx->CCMR1 = (TIMx->CCMR1 & 0x00FF) | ccmr;
            ccer = (1 << 4);  /* CC2E = 1 */
            TIMx->CCER |= ccer;
            TIMx->CCR2 = (period * DutyCycle) / 1000;
            break;

        case 3:
            /* 通道3 */
            ccmr = (6 << 4) | (1 << 3);
            TIMx->CCMR2 = (TIMx->CCMR2 & 0xFF00) | ccmr;
            ccer = (1 << 8);  /* CC3E = 1 */
            TIMx->CCER |= ccer;
            TIMx->CCR3 = (period * DutyCycle) / 1000;
            break;

        case 4:
            /* 通道4 */
            ccmr = (6 << 12) | (1 << 11);
            TIMx->CCMR2 = (TIMx->CCMR2 & 0x00FF) | ccmr;
            ccer = (1 << 12);  /* CC4E = 1 */
            TIMx->CCER |= ccer;
            TIMx->CCR4 = (period * DutyCycle) / 1000;
            break;
    }

    /* 使能主输出 (对于高级定时器TIM1/TIM8) */
    if (TIMx == TIM1 || TIMx == TIM8) {
        TIMx->BDTR |= (1 << 15);  /* MOE = 1 */
    }

    /* 启动定时器 */
    TIM_Start(TIMx);
}

/**
 * @brief 设置PWM占空比
 * @param TIMx 定时器指针
 * @param Channel PWM通道 (1-4)
 * @param DutyCycle 占空比 (0-1000，表示0.0%-100.0%)
 */
void TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx, uint8_t Channel, uint16_t DutyCycle)
{
    uint16_t pulse = (uint16_t)((TIMx->ARR * DutyCycle) / 1000);

    switch (Channel) {
        case 1: TIMx->CCR1 = pulse; break;
        case 2: TIMx->CCR2 = pulse; break;
        case 3: TIMx->CCR3 = pulse; break;
        case 4: TIMx->CCR4 = pulse; break;
    }
}

/**
 * @brief 设置PWM频率
 * @param TIMx 定时器指针
 * @param Frequency PWM频率(Hz)
 * @note 会保持当前占空比比例
 */
void TIM_PWM_SetFrequency(TIM_TypeDef *TIMx, uint32_t Frequency)
{
    uint32_t timerClk = TIM_GetClockFreq(TIMx);
    uint32_t newPeriod = (timerClk / Frequency) - 1;

    if (newPeriod > 65535) newPeriod = 65535;

    /* 保存当前各通道占空比比例 */
    uint16_t duty1 = (uint16_t)((TIMx->CCR1 * 1000) / TIMx->ARR);
    uint16_t duty2 = (uint16_t)((TIMx->CCR2 * 1000) / TIMx->ARR);
    uint16_t duty3 = (uint16_t)((TIMx->CCR3 * 1000) / TIMx->ARR);
    uint16_t duty4 = (uint16_t)((TIMx->CCR4 * 1000) / TIMx->ARR);

    /* 设置新周期 */
    TIMx->ARR = (uint16_t)newPeriod;

    /* 恢复占空比 */
    TIMx->CCR1 = (newPeriod * duty1) / 1000;
    TIMx->CCR2 = (newPeriod * duty2) / 1000;
    TIMx->CCR3 = (newPeriod * duty3) / 1000;
    TIMx->CCR4 = (newPeriod * duty4) / 1000;
}
