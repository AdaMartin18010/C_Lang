/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 689
 * Language: c
 * Block ID: f5eb0ff2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// adc_driver.c

void ADC_Init(ADC_TypeDef *ADCx) {
    // 使能时钟
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 配置ADC
    ADCx->CR1 = 0;  // 12位分辨率
    ADCx->CR2 = ADC_CR2_ADON;  // 上电

    // 校准
    ADCx->CR2 |= ADC_CR2_CAL;
    while (ADCx->CR2 & ADC_CR2_CAL);

    // 配置通道
    ADCx->SMPR2 = ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_2;
    // 239.5周期采样时间

    // 单通道序列
    ADCx->SQR1 = 0;  // 1个转换
    ADCx->SQR3 = 0;  // 通道0
}

uint16_t ADC_Read(ADC_TypeDef *ADCx, uint8_t channel) {
    // 设置通道
    ADCx->SQR3 = channel;

    // 启动转换
    ADCx->CR2 |= ADC_CR2_SWSTART;

    // 等待完成
    while (!(ADCx->SR & ADC_SR_EOC));

    // 读取结果
    return (uint16_t)ADCx->DR;
}

// 使用DMA的多通道扫描
void ADC_Scan_Init(ADC_TypeDef *ADCx, uint16_t *buffer, uint8_t num_channels) {
    // 配置DMA
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR = (uint32_t)buffer;
    DMA1_Channel1->CNDTR = num_channels;
    DMA1_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_CIRC |
                         DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0;
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    // 配置ADC扫描模式
    ADCx->CR1 |= ADC_CR1_SCAN;
    ADCx->CR2 |= ADC_CR2_DMA | ADC_CR2_CONT;

    // 配置通道序列
    ADCx->SQR1 = ((num_channels - 1) << 20);
    for (int i = 0; i < num_channels; i++) {
        if (i < 6) {
            ADCx->SQR3 |= (i << (i * 5));
        } else {
            ADCx->SQR2 |= (i << ((i - 6) * 5));
        }
    }

    // 启动
    ADCx->CR2 |= ADC_CR2_ADON;
    ADCx->CR2 |= ADC_CR2_SWSTART;
}
