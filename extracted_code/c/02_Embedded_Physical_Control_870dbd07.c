/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 637
 * Language: c
 * Block ID: 870dbd07
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 物理量类型安全: 防止单位混淆
// 技巧: 用结构体包装，编译期检查
// ============================================================

typedef struct { int32_t raw; } Millivolt;      // 毫伏
typedef struct { int32_t raw; } Milliampere;    // 毫安
typedef struct { int32_t raw; } Millicelsius;   // 毫摄氏度
typedef struct { int32_t raw; } Millibar;       // 毫巴（压力）

#define MV(val)  ((Millivolt){(val)})
#define MA(val)  ((Milliampere){(val)})
#define MC(val)  ((Millicelsius){(val)})

// 类型安全运算
static inline Millivolt mv_add(Millivolt a, Millivolt b) {
    return MV(a.raw + b.raw);
}

// 转换函数 (显式)
Millivolt adc_to_voltage(uint16_t adc_raw, uint16_t vref_mv) {
    // ADC 12bit, Vref = vref_mv mV
    // Vout = adc_raw * vref_mv / 4095
    return MV((int32_t)adc_raw * vref_mv / 4095);
}

// 错误: 编译失败！类型不匹配
// Millivolt v = MA(100);  // 错误: 不能混用

// 正确: 显式转换
Milliampere ma = MA(100);
// Millivolt v = (Millivolt){ma.raw};  // 可以，但显式
