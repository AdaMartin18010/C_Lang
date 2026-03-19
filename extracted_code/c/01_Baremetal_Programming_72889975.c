/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 1173
 * Language: c
 * Block ID: 72889975
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file system_stm32f103.c
 * @brief STM32F103C8T6 系统时钟配置
 * @details 配置系统时钟为72MHz，使用外部8MHz晶振
 */

#include <stdint.h>

/* ============================================================================
 * 寄存器定义
 * ============================================================================ */

/* RCC 寄存器基地址 */
#define RCC_BASE            0x40021000

/* RCC 寄存器 */
#define RCC_CR              (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR            (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CIR             (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB2RSTR        (*(volatile uint32_t *)(RCC_BASE + 0x0C))
#define RCC_APB1RSTR        (*(volatile uint32_t *)(RCC_BASE + 0x10))
#define RCC_AHBENR          (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_APB2ENR         (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR         (*(volatile uint32_t *)(RCC_BASE + 0x1C))
#define RCC_BDCR            (*(volatile uint32_t *)(RCC_BASE + 0x20))
#define RCC_CSR             (*(volatile uint32_t *)(RCC_BASE + 0x24))

/* FLASH 寄存器基地址 */
#define FLASH_BASE          0x40022000
#define FLASH_ACR           (*(volatile uint32_t *)(FLASH_BASE + 0x00))

/* ============================================================================
 * RCC_CR 寄存器位定义
 * ============================================================================ */
#define RCC_CR_HSION_Pos    0
#define RCC_CR_HSIRDY_Pos   1
#define RCC_CR_HSITRIM_Pos  3
#define RCC_CR_HSICAL_Pos   8
#define RCC_CR_HSEON_Pos    16
#define RCC_CR_HSERDY_Pos   17
#define RCC_CR_HSEBYP_Pos   18
#define RCC_CR_CSSON_Pos    19
#define RCC_CR_PLLON_Pos    24
#define RCC_CR_PLLRDY_Pos   25

/* ============================================================================
 * RCC_CFGR 寄存器位定义
 * ============================================================================ */
#define RCC_CFGR_SW_Pos         0
#define RCC_CFGR_SW_HSI         0x0
#define RCC_CFGR_SW_HSE         0x1
#define RCC_CFGR_SW_PLL         0x2

#define RCC_CFGR_SWS_Pos        2
#define RCC_CFGR_SWS_HSI        0x0
#define RCC_CFGR_SWS_HSE        0x4
#define RCC_CFGR_SWS_PLL        0x8

#define RCC_CFGR_HPRE_Pos       4
#define RCC_CFGR_HPRE_DIV1      0x0
#define RCC_CFGR_HPRE_DIV2      0x8

#define RCC_CFGR_PPRE1_Pos      8
#define RCC_CFGR_PPRE1_DIV1     0x0
#define RCC_CFGR_PPRE1_DIV2     0x4

#define RCC_CFGR_PPRE2_Pos      11
#define RCC_CFGR_PPRE2_DIV1     0x0

#define RCC_CFGR_ADCPRE_Pos     14
#define RCC_CFGR_ADCPRE_DIV6    0x2

#define RCC_CFGR_PLLSRC_Pos     16
#define RCC_CFGR_PLLSRC_HSI_DIV2 0x0
#define RCC_CFGR_PLLSRC_HSE      0x1

#define RCC_CFGR_PLLXTPRE_Pos   17
#define RCC_CFGR_PLLXTPRE_HSE    0x0
#define RCC_CFGR_PLLXTPRE_HSE_DIV2 0x1

#define RCC_CFGR_PLLMULL_Pos    18
#define RCC_CFGR_PLLMULL9       0x7

#define RCC_CFGR_USBPRE_Pos     22
#define RCC_CFGR_USBPRE_DIV1_5  0x0

/* ============================================================================
 * FLASH_ACR 寄存器位定义
 * ============================================================================ */
#define FLASH_ACR_LATENCY_Pos   0
#define FLASH_ACR_LATENCY_2WS   0x2
#define FLASH_ACR_PRFTBE_Pos    4

/* ============================================================================
 * 系统时钟变量
 * ============================================================================ */
uint32_t SystemCoreClock = 72000000;  /* 系统时钟频率: 72MHz */
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

/* ============================================================================
 * 错误处理函数
 * ============================================================================ */

/**
 * @brief 时钟配置错误处理
 * @details 当HSE或PLL未能就绪时调用
 */
static void Error_Handler(void)
{
    /* 进入无限循环，可通过调试器检查 */
    while (1) {
        /* 可在此处添加LED闪烁指示错误 */
    }
}

/* ============================================================================
 * 系统时钟配置函数
 * ============================================================================ */

/**
 * @brief 系统时钟初始化
 * @details
 *   配置系统时钟为72MHz:
 *   - HSE = 8MHz (外部晶振)
 *   - PLLSRC = HSE
 *   - PLLMUL = ×9  → PLLCLK = 72MHz
 *   - SYSCLK = PLLCLK = 72MHz
 *   - HCLK = SYSCLK = 72MHz (AHB不分频)
 *   - PCLK2 = HCLK = 72MHz (APB2不分频)
 *   - PCLK1 = HCLK/2 = 36MHz (APB2二分频)
 */
void SystemInit(void)
{
    /* ========================================================================
     * 步骤1: 复位RCC寄存器到默认值
     * ======================================================================== */

    /* 使能内部高速时钟HSI */
    RCC_CR |= (1 << RCC_CR_HSION_Pos);

    /* 等待HSI就绪 */
    while (!(RCC_CR & (1 << RCC_CR_HSIRDY_Pos)));

    /* 复位CFGR寄存器: 切换到HSI */
    RCC_CFGR = 0x00000000;

    /* 关闭HSE, PLL, CSS */
    RCC_CR &= ~((1 << RCC_CR_HSEON_Pos) |
                (1 << RCC_CR_CSSON_Pos) |
                (1 << RCC_CR_PLLON_Pos));

    /* 复位HSEBYP位 */
    RCC_CR &= ~(1 << RCC_CR_HSEBYP_Pos);

    /* 复位CFGR寄存器的PLLSRC, PLLXTPRE, PLLMUL, USBPRE位 */
    RCC_CFGR &= ~(0x0F << RCC_CFGR_PLLMULL_Pos);

    /* 禁用所有中断和清除挂起位 */
    RCC_CIR = 0x009F0000;

    /* ========================================================================
     * 步骤2: 配置Flash等待状态
     * ======================================================================== */

    /* 系统时钟72MHz > 48MHz，需要2个等待状态 */
    FLASH_ACR = FLASH_ACR_LATENCY_2WS | (1 << FLASH_ACR_PRFTBE_Pos);

    /* ========================================================================
     * 步骤3: 使能HSE外部高速时钟
     * ======================================================================== */

    /* 使能HSE */
    RCC_CR |= (1 << RCC_CR_HSEON_Pos);

    /* 等待HSE就绪，超时处理 */
    uint32_t timeout = 0xFFFF;
    while (!(RCC_CR & (1 << RCC_CR_HSERDY_Pos))) {
        if (--timeout == 0) {
            Error_Handler();
        }
    }

    /* ========================================================================
     * 步骤4: 配置PLL
     * ======================================================================== */

    /*
     * PLL配置: HSE × 9 = 72MHz
     * PLLSRC = HSE (不分频)
     * PLLMUL = ×9
     */
    uint32_t cfgr_temp = RCC_CFGR;

    /* 清除PLL配置位 */
    cfgr_temp &= ~((1 << RCC_CFGR_PLLSRC_Pos) |
                   (1 << RCC_CFGR_PLLXTPRE_Pos) |
                   (0xF << RCC_CFGR_PLLMULL_Pos));

    /* 设置PLL配置: HSE作为PLL源，×9倍频 */
    cfgr_temp |= (RCC_CFGR_PLLSRC_HSE << RCC_CFGR_PLLSRC_Pos) |
                 (RCC_CFGR_PLLXTPRE_HSE << RCC_CFGR_PLLXTPRE_Pos) |
                 (RCC_CFGR_PLLMULL9 << RCC_CFGR_PLLMULL_Pos);

    RCC_CFGR = cfgr_temp;

    /* 使能PLL */
    RCC_CR |= (1 << RCC_CR_PLLON_Pos);

    /* 等待PLL就绪 */
    timeout = 0xFFFF;
    while (!(RCC_CR & (1 << RCC_CR_PLLRDY_Pos))) {
        if (--timeout == 0) {
            Error_Handler();
        }
    }

    /* ========================================================================
     * 步骤5: 配置总线时钟预分频
     * ======================================================================== */

    cfgr_temp = RCC_CFGR;

    /* AHB预分频: HCLK = SYSCLK = 72MHz (不分频) */
    cfgr_temp &= ~(0xF << RCC_CFGR_HPRE_Pos);
    cfgr_temp |= (RCC_CFGR_HPRE_DIV1 << RCC_CFGR_HPRE_Pos);

    /* APB2预分频: PCLK2 = HCLK = 72MHz (不分频) */
    cfgr_temp &= ~(0x7 << RCC_CFGR_PPRE2_Pos);
    cfgr_temp |= (RCC_CFGR_PPRE2_DIV1 << RCC_CFGR_PPRE2_Pos);

    /* APB1预分频: PCLK1 = HCLK/2 = 36MHz (二分频) */
    cfgr_temp &= ~(0x7 << RCC_CFGR_PPRE1_Pos);
    cfgr_temp |= (RCC_CFGR_PPRE1_DIV2 << RCC_CFGR_PPRE1_Pos);

    /* ADC预分频: ADCCLK = PCLK2/6 = 12MHz */
    cfgr_temp &= ~(0x3 << RCC_CFGR_ADCPRE_Pos);
    cfgr_temp |= (RCC_CFGR_ADCPRE_DIV6 << RCC_CFGR_ADCPRE_Pos);

    RCC_CFGR = cfgr_temp;

    /* ========================================================================
     * 步骤6: 切换系统时钟到PLL
     * ======================================================================== */

    /* 选择PLL作为系统时钟源 */
    cfgr_temp = RCC_CFGR;
    cfgr_temp &= ~(0x3 << RCC_CFGR_SW_Pos);
    cfgr_temp |= (RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos);
    RCC_CFGR = cfgr_temp;

    /* 等待系统时钟切换完成 */
    timeout = 0xFFFF;
    while ((RCC_CFGR & (0x3 << RCC_CFGR_SWS_Pos)) !=
           (RCC_CFGR_SWS_PLL << RCC_CFGR_SWS_Pos)) {
        if (--timeout == 0) {
            Error_Handler();
        }
    }

    /* 更新系统时钟变量 */
    SystemCoreClock = 72000000;
}

/**
 * @brief 更新SystemCoreClock变量
 * @details 根据当前寄存器配置计算系统时钟频率
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllmull = 0, pllsource = 0;

    /* 获取系统时钟源 */
    tmp = RCC_CFGR & (0x3 << RCC_CFGR_SWS_Pos);

    switch (tmp) {
        case RCC_CFGR_SWS_HSI:  /* HSI作为系统时钟 */
            SystemCoreClock = 8000000;  /* HSI = 8MHz */
            break;

        case RCC_CFGR_SWS_HSE:  /* HSE作为系统时钟 */
            SystemCoreClock = 8000000;  /* HSE = 8MHz */
            break;

        case RCC_CFGR_SWS_PLL:  /* PLL作为系统时钟 */
            /* 获取PLL倍频系数 */
            pllmull = RCC_CFGR & (0xF << RCC_CFGR_PLLMULL_Pos);
            pllmull = (pllmull >> RCC_CFGR_PLLMULL_Pos) + 2;

            /* 获取PLL时钟源 */
            pllsource = RCC_CFGR & (1 << RCC_CFGR_PLLSRC_Pos);

            if (pllsource == 0) {
                /* HSI/2作为PLL源 */
                SystemCoreClock = (8000000 / 2) * pllmull;
            } else {
                /* HSE作为PLL源 */
                uint32_t prediv = (RCC_CFGR & (1 << RCC_CFGR_PLLXTPRE_Pos)) ? 2 : 1;
                SystemCoreClock = (8000000 / prediv) * pllmull;
            }
            break;

        default:
            SystemCoreClock = 8000000;
            break;
    }

    /* 计算HCLK频率 */
    tmp = AHBPrescTable[((RCC_CFGR & (0xF << RCC_CFGR_HPRE_Pos)) >> RCC_CFGR_HPRE_Pos)];
    SystemCoreClock >>= tmp;
}

/* ============================================================================
 * 外设时钟使能函数
 * ============================================================================ */

/**
 * @brief 使能GPIO端口时钟
 * @param GPIOx 端口号: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G
 */
void RCC_EnableGPIO_Clock(uint8_t GPIOx)
{
    if (GPIOx <= 7) {
        RCC_APB2ENR |= (1 << (2 + GPIOx));
    }
}

/**
 * @brief 使能USART时钟
 * @param USARTx USART号: 1, 2, 3
 */
void RCC_EnableUSART_Clock(uint8_t USARTx)
{
    if (USARTx == 1) {
        RCC_APB2ENR |= (1 << 14);  /* USART1在APB2 */
    } else if (USARTx == 2) {
        RCC_APB1ENR |= (1 << 17);  /* USART2在APB1 */
    } else if (USARTx == 3) {
        RCC_APB1ENR |= (1 << 18);  /* USART3在APB1 */
    }
}

/**
 * @brief 使能TIM时钟
 * @param TIMx 定时器号: 1-8
 */
void RCC_EnableTIM_Clock(uint8_t TIMx)
{
    if (TIMx == 1) {
        RCC_APB2ENR |= (1 << 11);
    } else if (TIMx >= 2 && TIMx <= 7) {
        RCC_APB1ENR |= (1 << (TIMx - 2 + 0));
    } else if (TIMx == 8) {
        RCC_APB2ENR |= (1 << 13);
    }
}

/**
 * @brief 使能DMA时钟
 * @param DMAx DMA号: 1, 2
 */
void RCC_EnableDMA_Clock(uint8_t DMAx)
{
    if (DMAx == 1) {
        RCC_AHBENR |= (1 << 0);
    } else if (DMAx == 2) {
        RCC_AHBENR |= (1 << 1);
    }
}
