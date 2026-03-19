/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\01_Cryo_Serial_Interface.md
 * Line: 207
 * Language: c
 * Block ID: eadaa170
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 低温CDR (Clock Data Recovery)
// 二阶PLL结构
// ============================================================================

typedef struct {
    // PLL参数
    float kp;               // 比例增益
    float ki;               // 积分增益

    // 状态
    float integrator;
    float vco_freq;
    float vco_phase;

    // 输入
    float data_sample;
    float clock_edge;
} CryoCDR;

// 初始化CDR
void cdr_init(CryoCDR *cdr, float nominal_freq) {
    cdr->kp = 0.1f;
    cdr->ki = 0.01f;
    cdr->integrator = 0.0f;
    cdr->vco_freq = nominal_freq;
    cdr->vco_phase = 0.0f;
}

// CDR更新 (每符号)
void cdr_update(CryoCDR *cdr, float sample) {
    // 相位检测 (Bang-Bang或线性)
    float phase_error;
    if (sample > 0) {
        phase_error = (cdr->vco_phase > 0) ? -1.0f : 1.0f;
    } else {
        phase_error = (cdr->vco_phase > 0) ? 1.0f : -1.0f;
    }

    // 环路滤波器
    cdr->integrator += cdr->ki * phase_error;
    float control = cdr->kp * phase_error + cdr->integrator;

    // 更新VCO
    cdr->vco_freq += control * 0.001f;  // 频率调整
    cdr->vco_phase += cdr->vco_freq;

    // 相位归一化
    if (cdr->vco_phase > M_PI) cdr->vco_phase -= 2.0f * M_PI;
    if (cdr->vco_phase < -M_PI) cdr->vco_phase += 2.0f * M_PI;
}
