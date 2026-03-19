/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 245
 * Language: c
 * Block ID: 517a5103
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void adc_init(void) {
    // 使能GPIOA和ADC1时钟
    RCC_AHB1ENR |= (1 << 0);
    RCC_APB2ENR |= (1 << 8);

    // 配置PA0为模拟输入
    GPIOA_MODER |= (3 << 0);  // 模拟模式

    // ADC配置
    ADC1_CR1 = 0;
    ADC1_CR2 = (1 << 0);  // ADON

    // 采样时间
    ADC1_SMPR2 = (7 << 0);  // 480 cycles for channel 0

    // 软件启动转换
    ADC1_CR2 |= (1 << 30);  // SWSTART
}

uint16_t adc_read_channel(uint8_t channel) {
    // 设置通道
    ADC1_SQR3 = channel;

    // 开始转换
    ADC1_CR2 |= (1 << 30);

    // 等待完成
    while (!(ADC1_SR & (1 << 1)));  // EOC

    return ADC1_DR;
}
