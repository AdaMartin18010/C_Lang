/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 1119
 * Language: c
 * Block ID: dc85b4a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// ADC: 模拟到数字转换，传感器数据采集的基础
// ============================================================

// 启动ADC转换
uint16_t adc_read_channel(ADC_TypeDef *adc, uint8_t channel) {
    // 配置通道
    adc->SQR3 = channel;  // 设置转换序列

    // 启动转换
    adc->CR2 |= ADC_CR2_SWSTART;

    // 等待完成
    while (!(adc->SR & ADC_SR_EOC));

    // 读取结果
    return (uint16_t)adc->DR;
}

// 多次采样平均 (减少噪声)
uint16_t adc_read_average(ADC_TypeDef *adc, uint8_t channel, uint8_t samples) {
    uint32_t sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += adc_read_channel(adc, channel);
    }
    return (uint16_t)(sum / samples);
}

// 滑动平均滤波器 (实时滤波)
#define FILTER_SIZE 16

typedef struct {
    uint16_t buffer[FILTER_SIZE];
    uint8_t index;
    uint32_t sum;
} MovingAverageFilter;

void maf_init(MovingAverageFilter *f) {
    for (int i = 0; i < FILTER_SIZE; i++) {
        f->buffer[i] = 0;
    }
    f->index = 0;
    f->sum = 0;
}

uint16_t maf_update(MovingAverageFilter *f, uint16_t new_value) {
    // 减去最老的值
    f->sum -= f->buffer[f->index];

    // 加入新值
    f->buffer[f->index] = new_value;
    f->sum += new_value;

    // 更新索引
    f->index = (f->index + 1) % FILTER_SIZE;

    // 返回平均
    return (uint16_t)(f->sum / FILTER_SIZE);
}

// 中值滤波 (去除脉冲噪声)
uint16_t median_filter3(uint16_t a, uint16_t b, uint16_t c) {
    if (a > b) {
        if (b > c) return b;      // a > b > c
        if (a > c) return c;      // a > c > b
        return a;                 // c > a > b
    } else {
        if (a > c) return a;      // b > a > c
        if (b > c) return c;      // b > c > a
        return b;                 // c > b > a
    }
}
