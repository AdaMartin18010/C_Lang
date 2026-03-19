/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\02_Subthreshold_Optimization.md
 * Line: 266
 * Language: c
 * Block ID: fa0a5b9e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 工艺偏差和失配分析
// ============================================================================

// 阈值电压失配参数
#define AVTH_N      3.0e-9      // V·m (65nm)
#define AVTH_P      3.5e-9

// 计算阈值电压标准差
float calculate_vth_mismatch(float w, float l, bool is_nmos) {
    float avth = is_nmos ? AVTH_N : AVTH_P;
    return avth / sqrtf(w * l);
}

// 蒙特卡洛分析
typedef struct {
    float vth_shift_mean;
    float vth_shift_sigma;
    float u0_shift_mean;
    float u0_shift_sigma;
} MCCorners;

// 生成随机工艺角
void generate_mc_sample(const MCCorners *corners,
                        ProcessParams *sample,
                        const ProcessParams *nominal) {
    // Box-Muller变换生成高斯随机数
    float u1 = (float)rand() / RAND_MAX;
    float u2 = (float)rand() / RAND_MAX;

    float z1 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2);
    float z2 = sqrtf(-2.0f * logf(u1)) * sinf(2.0f * M_PI * u2);

    *sample = *nominal;
    sample->vth_n += corners->vth_shift_mean + corners->vth_shift_sigma * z1;
    sample->vth_p += corners->vth_shift_mean + corners->vth_shift_sigma * z1;
    sample->u0_n *= (1.0f + corners->u0_shift_mean + corners->u0_shift_sigma * z2);
    sample->u0_p *= (1.0f + corners->uth_shift_mean + corners->u0_shift_sigma * z2);
}

// 良率分析
float calculate_yield(const ProcessParams *nominal,
                      const MCCorners *corners,
                      int num_mc_runs,
                      float vdd, float target_delay_max) {
    int pass_count = 0;

    for (int i = 0; i < num_mc_runs; i++) {
        ProcessParams mc_sample;
        generate_mc_sample(corners, &mc_sample, nominal);

        OperatingPoint point = calculate_energy_point(&mc_sample, vdd, 0.1f,
                                                       1e-15f, 1e-6f, 60e-9f);

        if (point.delay <= target_delay_max) {
            pass_count++;
        }
    }

    return (float)pass_count / num_mc_runs;
}
