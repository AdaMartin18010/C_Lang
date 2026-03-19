/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_26262\01_ASIL_Decomposition.md
 * Line: 157
 * Language: c
 * Block ID: ad59d5fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 确保分解元素间的充分独立性
 */

/* 1. 开发独立性 - 不同团队 */
/* 团队A开发 */
// brake_channel_a.c - ASIL B

/* 团队B开发 */
// brake_channel_b.c - ASIL B (完全不同的设计)

/* 2. 资源独立性 - 独立硬件 */
typedef struct {
    /* 通道A使用ADC0 */
    volatile uint16_t *adc_ch_a = (uint16_t *)0x40001000;
    /* 通道B使用ADC1 */
    volatile uint16_t *adc_ch_b = (uint16_t *)0x40002000;
} AdcChannels;

/* 3. 时钟独立性 */
void independent_sampling(void)
{
    /* 不同时钟源触发 */
    trigger_adc_a();  /* 主时钟 */
    delay_us(10);     /* 时间偏移 */
    trigger_adc_b();  /* 相同时钟但偏移 */
}

/* 4. 软件独立性 - 不同算法 */
int32_t filter_a(int32_t input) {
    /* FIR滤波器 */
    return fir_filter(input);
}

int32_t filter_b(int32_t input) {
    /* IIR滤波器 */
    return iir_filter(input);
}
