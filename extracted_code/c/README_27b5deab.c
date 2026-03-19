/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\README.md
 * Line: 91
 * Language: c
 * Block ID: 27b5deab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RSFQ 逻辑门基础结构
// 磁通量子作为信息载体：Φ0 存在为 "1"，不存在为 "0"

typedef enum {
    RSFQ_JTL,      // Josephson Transmission Line
    RSFQ_SPLIT,    // 分路器
    RSFQ_MERGE,    // 合并器
    RSFQ_D2,       // 双结触发器
    RSFQ_OR,       // 或门
    RSFQ_AND,      // 与门
    RSFQ_NOT,      // 非门
    RSFQ_XOR,      // 异或门
    RSFQ_DFF       // D 触发器
} RSFQ_GateType;

// RSFQ 脉冲参数
typedef struct {
    float pulse_amplitude;   // 脉冲幅度 (mV)
    float pulse_duration;    // 脉冲宽度 (ps)
    float clock_frequency;   // 时钟频率 (GHz)
} RSFQ_Pulse;

// JTL（约瑟夫森传输线）仿真
void jtl_propagate(RSFQ_Pulse *input, JosephsonJunction *junctions,
                   int num_junctions, RSFQ_Pulse *output) {
    // JTL 用于传输和放大 SFQ 脉冲
    float current = input->pulse_amplitude / junctions[0].normal_resistance;

    for (int i = 0; i < num_junctions; i++) {
        // 检查是否超过临界电流
        if (current > junctions[i].critical_current) {
            // 结切换到电压态，产生 SFQ 脉冲
            float v_pulse = input->pulse_amplitude * 1.1f;  // 放大
            current = v_pulse / junctions[i].normal_resistance;
        }
    }

    output->pulse_amplitude = current * junctions[num_junctions-1].normal_resistance;
    output->pulse_duration = input->pulse_duration;
}

// RSFQ D 触发器
#define SFQ_PRESENT 1
#define SFQ_ABSENT 0

typedef struct {
    int state;           // 当前状态
    JosephsonJunction storage_jj;
    JosephsonJunction read_jj;
} RSFQ_DFF;

void rsfq_dff_clock(RSFQ_DFF *dff, int d_input, int clock_pulse) {
    if (clock_pulse == SFQ_PRESENT) {
        // 时钟上升沿，锁存输入
        if (d_input == SFQ_PRESENT) {
            // 写入 "1"
            dff->state = 1;
            // 产生存储电流
            dff->storage_jj.critical_current =
                dff->storage_jj.critical_current * 1.2f;
        } else {
            dff->state = 0;
        }
    }
}

int rsfq_dff_output(RSFQ_DFF *dff, int read_clock) {
    if (read_clock == SFQ_PRESENT && dff->state == 1) {
        // 读出 "1"，产生 SFQ 脉冲
        dff->state = 0;  // 破坏性读出
        return SFQ_PRESENT;
    }
    return SFQ_ABSENT;
}
