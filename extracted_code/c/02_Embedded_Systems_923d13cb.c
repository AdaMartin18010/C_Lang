/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Systems.md
 * Line: 415
 * Language: c
 * Block ID: 923d13cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：未使能时钟就访问外设
void bad_init(void) {
    GPIOA->MODER = 0x01;  // 配置GPIOA前未使能时钟！
}

// ✅ 正确：先使能时钟
void good_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // 使能GPIOA时钟

    // 插入延时等待时钟稳定（某些MCU需要）
    volatile uint32_t dummy = RCC->AHB1ENR;
    (void)dummy;

    GPIOA->MODER = 0x01;
}
