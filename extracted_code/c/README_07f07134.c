/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 150
 * Language: c
 * Block ID: 07f07134
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// TrueNorth 核心配置
#define CORE_SIZE 256
#define NUM_NEURONS 256
#define NUM_AXONS 256

typedef struct {
    uint8_t synaptic_weights[NUM_AXONS];
    uint8_t synaptic_delays[NUM_AXONS];
    uint16_t neuron_params[16];  // 神经元参数表索引
} TrueNorthCore;

// 突触交叉开关
void compute_core(TrueNorthCore *core,
                  uint8_t axon_inputs[NUM_AXONS],
                  uint8_t neuron_outputs[NUM_NEURONS]) {
    for (int n = 0; n < NUM_NEURONS; n++) {
        int32_t sum = 0;
        for (int a = 0; a < NUM_AXONS; a++) {
            if (axon_inputs[a]) {
                sum += core->synaptic_weights[a];
            }
        }
        neuron_outputs[n] = (sum > THRESHOLD) ? 1 : 0;
    }
}
