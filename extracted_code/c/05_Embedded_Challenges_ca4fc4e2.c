/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 281
 * Language: c
 * Block ID: ca4fc4e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define ADC_BUFFER_SIZE 1024
uint16_t adc_buffer[ADC_BUFFER_SIZE];

void adc_dma_init(void) {
    // 使能DMA2时钟
    RCC_AHB1ENR |= (1 << 22);

    // DMA2 Stream0, Channel 0 (ADC1)
    DMA2_Stream0_CR = 0;  // 禁用
    while (DMA2_Stream0_CR & (1 << 0));  // 等待禁用

    // 配置
    DMA2_Stream0_PAR = (uint32_t)&ADC1_DR;
    DMA2_Stream0_M0AR = (uint32_t)adc_buffer;
    DMA2_Stream0_NDTR = ADC_BUFFER_SIZE;

    // 配置寄存器
    DMA2_Stream0_CR = (0 << 25) |   // Channel 0
                      (1 << 10) |   // 内存递增
                      (1 << 8) |    // 循环模式
                      (1 << 6) |    // 外设到内存
                      (1 << 5) |    // 中断使能
                      (0 << 13);    // 半字传输

    // 使能DMA流
    DMA2_Stream0_CR |= (1 << 0);

    // ADC配置为DMA模式
    ADC1_CR2 |= (1 << 8);  // DMA
    ADC1_CR2 |= (1 << 1);  // CONT (连续模式)
}
