/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\01_Quantum_Classical_Interface.md
 * Line: 390
 * Language: c
 * Block ID: 2a96315c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 量子控制脉冲生成
// 从门指令到实际AWG波形的转换
// ============================================================================

#define PULSE_SAMPLES       1024
#define MAX_CHANNELS        64      // 32Q * 2 (I+Q)
#define SAMPLING_RATE_GSPS  1.0     // 1 GSPS

// 脉冲形状类型
typedef enum {
    PULSE_GAUSSIAN,
    PULSE_DRAG,         // Derivative Removal by Adiabatic Gate
    PULSE_SQUARE,
    PULSE_COSINE
} PulseShape;

// 单个脉冲定义
typedef struct {
    PulseShape shape;
    double amplitude;       // 0-1 (归一化)
    double duration_ns;
    double sigma_ns;        // 高斯宽度
    double beta;            // DRAG参数
    double frequency_ghz;   // 载波频率
    double phase_rad;       // 载波相位
} PulseDef;

// AWG波形样本 (复数IQ)
typedef struct {
    double i[PULSE_SAMPLES];
    double q[PULSE_SAMPLES];
    uint32_t num_samples;
    double sample_rate_gsps;
} Waveform;

// ============================================================================
// 脉冲形状生成
// ============================================================================

void generate_gaussian_pulse(const PulseDef *def, Waveform *wf) {
    double dt_ns = 1000.0 / (def->duration_ns * SAMPLING_RATE_GSPS);
    uint32_t n_samples = (uint32_t)(def->duration_ns * SAMPLING_RATE_GSPS);

    if (n_samples > PULSE_SAMPLES) n_samples = PULSE_SAMPLES;

    double center = n_samples / 2.0;
    double sigma_samples = def->sigma_ns * SAMPLING_RATE_GSPS;

    for (uint32_t i = 0; i < n_samples; i++) {
        double t = i - center;
        double envelope = exp(-(t * t) / (2.0 * sigma_samples * sigma_samples));

        // 载波
        double carrier_phase = 2.0 * M_PI * def->frequency_ghz * i / SAMPLING_RATE_GSPS
                              + def->phase_rad;

        wf->i[i] = def->amplitude * envelope * cos(carrier_phase);
        wf->q[i] = def->amplitude * envelope * sin(carrier_phase);
    }

    wf->num_samples = n_samples;
    wf->sample_rate_gsps = SAMPLING_RATE_GSPS;
}

void generate_drag_pulse(const PulseDef *def, Waveform *wf) {
    double dt_ns = 1000.0 / SAMPLING_RATE_GSPS;
    uint32_t n_samples = (uint32_t)(def->duration_ns * SAMPLING_RATE_GSPS);

    if (n_samples > PULSE_SAMPLES) n_samples = PULSE_SAMPLES;

    double center = n_samples / 2.0;
    double sigma_samples = def->sigma_ns * SAMPLING_RATE_GSPS;

    for (uint32_t i = 0; i < n_samples; i++) {
        double t = i - center;
        double gauss = exp(-(t * t) / (2.0 * sigma_samples * sigma_samples));

        // 高斯包络
        double envelope_i = gauss;

        // DRAG修正 (导数项)
        double envelope_q = -def->beta * (t / (sigma_samples * sigma_samples)) * gauss;

        // 载波
        double carrier_phase = 2.0 * M_PI * def->frequency_ghz * i / SAMPLING_RATE_GSPS
                              + def->phase_rad;

        wf->i[i] = def->amplitude * (envelope_i * cos(carrier_phase) -
                                      envelope_q * sin(carrier_phase));
        wf->q[i] = def->amplitude * (envelope_i * sin(carrier_phase) +
                                      envelope_q * cos(carrier_phase));
    }

    wf->num_samples = n_samples;
    wf->sample_rate_gsps = SAMPLING_RATE_GSPS;
}

// ============================================================================
// 门到脉冲的映射
// ============================================================================

// 系统参数
typedef struct {
    double qubit_freq_ghz[MAX_QUBITS];      // 各量子比特频率
    double anharmonicity_mhz[MAX_QUBITS];   // 非简谐性
    double pulse_sigma_ns;                  // 默认脉冲宽度
    double drag_beta;                       // DRAG参数
} SystemParams;

void gate_to_pulse(const GateInstruction *gate, const SystemParams *sys,
                   PulseDef *pulse) {

    uint8_t q = gate->qubits[0];

    switch (gate->gate_id) {
        case GATE_X:
            pulse->shape = PULSE_DRAG;
            pulse->amplitude = 1.0;
            pulse->duration_ns = 25;
            pulse->sigma_ns = sys->pulse_sigma_ns;
            pulse->beta = sys->drag_beta;
            pulse->frequency_ghz = sys->qubit_freq_ghz[q];
            pulse->phase_rad = 0;
            break;

        case GATE_Y:
            pulse->shape = PULSE_DRAG;
            pulse->amplitude = 1.0;
            pulse->duration_ns = 25;
            pulse->sigma_ns = sys->pulse_sigma_ns;
            pulse->beta = sys->drag_beta;
            pulse->frequency_ghz = sys->qubit_freq_ghz[q];
            pulse->phase_rad = M_PI / 2;  // 90度相位
            break;

        case GATE_H:
            // Hadamard = Ry(pi/2) 然后 相位调整
            pulse->shape = PULSE_DRAG;
            pulse->amplitude = 1.0 / sqrt(2);
            pulse->duration_ns = 35;
            pulse->sigma_ns = sys->pulse_sigma_ns;
            pulse->beta = sys->drag_beta;
            pulse->frequency_ghz = sys->qubit_freq_ghz[q];
            pulse->phase_rad = 0;
            break;

        case GATE_RZ:
            // 虚拟Z门 - 仅相位调整，无实际脉冲
            pulse->shape = PULSE_SQUARE;
            pulse->amplitude = 0;
            pulse->duration_ns = 0;  // 零时间门
            pulse->phase_rad = gate->params[0];
            break;

        default:
            pulse->amplitude = 0;
            break;
    }
}

// ============================================================================
// 完整脉冲序列生成
// ============================================================================

typedef struct {
    Waveform channels[MAX_CHANNELS];
    uint32_t channel_mask;      // 哪些通道有数据
    uint64_t total_duration_ns;
} PulseSequence;

int compile_circuit_to_pulses(const QuantumCircuit *circ,
                               const SystemParams *sys,
                               PulseSequence *seq) {

    seq->channel_mask = 0;
    seq->total_duration_ns = circ->total_duration_ns;

    for (uint32_t g = 0; g < circ->num_gates; g++) {
        const GateInstruction *gate = &circ->gates[g];

        PulseDef pulse;
        gate_to_pulse(gate, sys, &pulse);

        if (pulse.amplitude > 0) {
            Waveform wf;

            if (pulse.shape == PULSE_DRAG) {
                generate_drag_pulse(&pulse, &wf);
            } else {
                generate_gaussian_pulse(&pulse, &wf);
            }

            // 映射到对应通道 (每个量子比特有I和Q两个通道)
            uint8_t q = gate->qubits[0];
            uint8_t i_channel = q * 2;
            uint8_t q_channel = q * 2 + 1;

            // 将波形写入对应时隙
            // (这里简化处理，实际需要精确的时序对齐)
            for (uint32_t s = 0; s < wf.num_samples; s++) {
                seq->channels[i_channel].i[s] = wf.i[s];
                seq->channels[q_channel].q[s] = wf.q[s];
            }

            seq->channel_mask |= (1 << i_channel);
            seq->channel_mask |= (1 << q_channel);
        }
    }

    return 0;
}
