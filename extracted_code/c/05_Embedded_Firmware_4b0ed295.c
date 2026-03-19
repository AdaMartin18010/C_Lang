/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\05_Embedded_Firmware.md
 * Line: 146
 * Language: c
 * Block ID: 4b0ed295
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// system_stm32f4xx.c
// 系统初始化

#define HSE_VALUE 8000000U
#define PLL_M 8
#define PLL_N 336
#define PLL_P 2
#define PLL_Q 7

void SystemInit(void) {
    // 1. 使能FPU
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));

    // 2. 配置Flash预取取和等待状态
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |
                 FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

    // 3. 使能HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 4. 配置PLL
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) |
                   (((PLL_P >> 1) - 1) << 16) |
                   RCC_PLLCFGR_PLLSRC_HSE | (PLL_Q << 24);

    // 5. 使能PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 6. 切换系统时钟到PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // 7. 更新SystemCoreClock变量
    SystemCoreClockUpdate();
}
