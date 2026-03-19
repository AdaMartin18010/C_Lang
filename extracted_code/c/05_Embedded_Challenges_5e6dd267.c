/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 368
 * Language: c
 * Block ID: 5e6dd267
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 进入Sleep模式 (CPU停止，外设运行)
void enter_sleep_mode(void) {
    SCB_SCR &= ~(1 << 2);  // 清除SLEEPDEEP
    __WFI();  // Wait For Interrupt
}

// 进入Stop模式 (CPU和大部分时钟停止)
void enter_stop_mode(void) {
    SCB_SCR |= (1 << 2);   // SLEEPDEEP
    PWR_CR &= ~(3 << 0);   // 清除PDDS和LPMS
    PWR_CR |= (1 << 1);    // PDDS=0, 电压调节器开
    __WFI();
}

// 进入Standby模式 (最低功耗，RAM丢失)
void enter_standby_mode(void) {
    SCB_SCR |= (1 << 2);   // SLEEPDEEP
    PWR_CR |= (1 << 1);    // PDDS=1
    SCB_AIRCR = (0x5FA << 16) | (1 << 2);  // 系统复位
}
