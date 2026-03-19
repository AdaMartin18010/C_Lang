/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\README.md
 * Line: 378
 * Language: c
 * Block ID: e7f868bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 温差发电（TEG）管理
typedef struct {
    float seebeck_coeff;     // 塞贝克系数 (V/K)
    float thermal_resistance;
    float electrical_resistance;
    float max_power;
} TEG_Module;

// 计算 TEG 输出功率
float calculate_teg_power(TEG_Module *teg, float delta_t) {
    // 开路电压
    float v_open = teg->seebeck_coeff * delta_t;

    // 最大功率传输（负载匹配）
    float v_load = v_open / 2.0f;
    float i_load = v_load / teg->electrical_resistance;

    return v_load * i_load;
}

// 最大功率点跟踪（MPPT）
typedef struct {
    float voltage;
    float current;
    float power;
    float duty_cycle;
} MPPT_State;

void mppt_update(MPPT_State *state, float v_new, float i_new) {
    float p_new = v_new * i_new;

    // 扰动观察法
    if (p_new > state->power) {
        // 功率增加，继续同方向调整
        state->duty_cycle += (v_new > state->voltage) ? 0.01f : -0.01f;
    } else {
        // 功率减小，反向调整
        state->duty_cycle -= (v_new > state->voltage) ? 0.01f : -0.01f;
    }

    // 限幅
    if (state->duty_cycle > 0.95f) state->duty_cycle = 0.95f;
    if (state->duty_cycle < 0.05f) state->duty_cycle = 0.05f;

    state->voltage = v_new;
    state->current = i_new;
    state->power = p_new;
}
