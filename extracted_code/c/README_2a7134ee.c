/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\README.md
 * Line: 385
 * Language: c
 * Block ID: 2a7134ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 超导-经典接口电平转换
#define SFQ_PULSE_VOLTAGE 2e-3f   // 2 mV
#define CMOS_LOGIC_HIGH 0.3f      // 300 mV at 4K
#define CMOS_LOGIC_LOW 0.0f

// SFQ 到 CMOS 转换器
typedef struct {
    int num_stages;
    float gain_per_stage;
    float bandwidth;
} SFQ_to_CMOS_Converter;

void sfq_to_cmos(SFQ_Pulse *sfq_input, float *cmos_output,
                 SFQ_to_CMOS_Converter *converter) {
    float signal = sfq_input->pulse_amplitude;

    // 多级放大
    for (int i = 0; i < converter->num_stages; i++) {
        signal *= converter->gain_per_stage;
    }

    // 阈值判决
    *cmos_output = (signal > (CMOS_LOGIC_HIGH + CMOS_LOGIC_LOW) / 2) ?
                   CMOS_LOGIC_HIGH : CMOS_LOGIC_LOW;
}

// CMOS 到 SFQ 转换器
void cmos_to_sfq(float cmos_input, RSFQ_Pulse *sfq_output,
                 float bias_current) {
    if (cmos_input > (CMOS_LOGIC_HIGH + CMOS_LOGIC_LOW) / 2) {
        // 产生 SFQ 脉冲
        sfq_output->pulse_amplitude = SFQ_PULSE_VOLTAGE;
        sfq_output->pulse_duration = 2e-12f;  // 2 ps
    } else {
        sfq_output->pulse_amplitude = 0;
    }
}
