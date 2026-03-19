/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 200
 * Language: c
 * Block ID: 46bc2b5f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 位域: 赋予每一位语义含义
// ============================================================

// 传统掩码方式 (容易出错)
#define ADC_CR1_ADON    (1U << 0)   // 开启ADC
#define ADC_CR1_CONT    (1U << 1)   // 连续转换
#define ADC_CR1_CAL     (1U << 2)   // 校准
#define ADC_CR1_RES_12B (0U << 24)  // 12位分辨率
#define ADC_CR1_RES_10B (1U << 24)  // 10位分辨率
#define ADC_CR1_RES_8B  (2U << 24)  // 8位分辨率

ADC->CR1 = ADC_CR1_ADON | ADC_CR1_CONT | ADC_CR1_RES_12B;  // 一堆或操作

// 位域结构方式 (C99, 可移植性差，不适合跨平台)
typedef struct {
    uint32_t ADON     : 1;   // bit 0: 开启
    uint32_t CONT     : 1;   // bit 1: 连续模式
    uint32_t CAL      : 1;   // bit 2: 校准
    uint32_t          : 21;  // bits 3-23: 保留
    uint32_t RES      : 2;   // bits 24-25: 分辨率 (0=12b, 1=10b, 2=8b, 3=6b)
    uint32_t          : 6;   // bits 26-31: 保留
} ADC_CR1_Bits;

// Union方式访问 (兼顾语义和效率)
typedef union {
    uint32_t reg;           // 整体访问
    ADC_CR1_Bits bits;      // 位域访问
} ADC_CR1_Type;

// 使用
ADC_CR1_Type cr1;
cr1.reg = 0;
cr1.bits.ADON = 1;      // 开启ADC
cr1.bits.CONT = 1;      // 连续模式
cr1.bits.RES = 0;       // 12位分辨率
ADC->CR1 = cr1.reg;     // 一次性写入

// 宏封装方式 (推荐，兼顾可读性和可移植性)
#define ADC_SET_RESOLUTION(adc, res) do { \
    uint32_t tmp = (adc)->CR1; \
    tmp &= ~(3U << 24); \
    tmp |= ((res) & 3U) << 24; \
    (adc)->CR1 = tmp; \
} while(0)

#define ADC_ENABLE(adc)     ((adc)->CR1 |= (1U << 0))
#define ADC_DISABLE(adc)    ((adc)->CR1 &= ~(1U << 0))
#define ADC_IS_ENABLED(adc) ((adc)->CR1 & (1U << 0))
