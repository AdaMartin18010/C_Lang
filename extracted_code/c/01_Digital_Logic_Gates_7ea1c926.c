/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 504
 * Language: c
 * Block ID: 7ea1c926
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CMOS功耗计算

typedef struct {
    double c_load;      // 负载电容 (F)
    double vdd;         // 供电电压 (V)
    double f_clock;     // 时钟频率 (Hz)
    double activity;    // 开关活动因子 (0-1)
} PowerParams;

// 动态功耗：P_dyn = C × V² × f × α
double dynamic_power(PowerParams *p) {
    return p->c_load * p->vdd * p->vdd * p->f_clock * p->activity;
}

// 静态功耗（漏电流）
double static_power(double i_leak, double vdd) {
    return i_leak * vdd;
}

// 总功耗
double total_power(PowerParams *p, double i_leak) {
    return dynamic_power(p) + static_power(i_leak, p->vdd);
}

// 示例：45nm工艺，1GHz时钟，1V供电
double example_power() {
    PowerParams p = {
        .c_load = 1e-12,    // 1pF
        .vdd = 1.0,         // 1V
        .f_clock = 1e9,     // 1GHz
        .activity = 0.2     // 20%开关活动
    };
    double i_leak = 1e-6;  // 1uA漏电流

    double p_dyn = dynamic_power(&p);   // ~0.2mW
    double p_static = static_power(i_leak, p.vdd);  // ~1uW

    return p_dyn + p_static;
}
