/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 325
 * Language: c
 * Block ID: 05ee3bba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void rtc_init(void) {
    // 使能PWR和BKP时钟
    RCC_APB1ENR |= (1 << 28) | (1 << 27);

    // 允许访问备份域
    PWR_CR |= (1 << 8);  // DBP

    // 选择LSE (32.768kHz)
    RCC_BDCR |= (1 << 0);  // LSEON
    while (!(RCC_BDCR & (1 << 1)));  // 等待就绪

    RCC_BDCR |= (1 << 8);  // RTCSEL = LSE
    RCC_BDCR |= (1 << 15); // RTCEN

    // 进入初始化模式
    RTC_ISR |= (1 << 7);  // INIT
    while (!(RTC_ISR & (1 << 6)));  // 等待INITF

    // 配置预分频器 (32768 / 128 / 256 = 1Hz)
    RTC_PRER = (127 << 16) | 255;

    // 退出初始化模式
    RTC_ISR &= ~(1 << 7);
}

void rtc_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
    uint32_t tr = ((hour / 10) << 20) | ((hour % 10) << 16) |
                  ((minute / 10) << 12) | ((minute % 10) << 8) |
                  ((second / 10) << 4) | (second % 10);
    RTC_TR = tr;
}

void rtc_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second) {
    uint32_t tr = RTC_TR;
    *hour = ((tr >> 20) & 0x3) * 10 + ((tr >> 16) & 0xF);
    *minute = ((tr >> 12) & 0x7) * 10 + ((tr >> 8) & 0xF);
    *second = ((tr >> 4) & 0x7) * 10 + (tr & 0xF);
}
