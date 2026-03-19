/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\01_Quantum_Simulation_C.md
 * Line: 193
 * Language: c
 * Block ID: a744ea29
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 单量子比特门
typedef struct {
    Complex m00, m01, m10, m11;
} Gate1Q;

// 标准门
static const Gate1Q GATE_X = {{0,0}, {1,0}, {1,0}, {0,0}};  // Pauli-X
static const Gate1Q GATE_Y = {{0,0}, {0,-1}, {0,1}, {0,0}}; // Pauli-Y
static const Gate1Q GATE_Z = {{1,0}, {0,0}, {0,0}, {-1,0}}; // Pauli-Z
static const Gate1Q GATE_H = {{0.707107,0}, {0.707107,0}, {0.707107,0}, {-0.707107,0}}; // Hadamard

static const Gate1Q GATE_S = {{1,0}, {0,0}, {0,0}, {0,1}};  // Phase
static const Gate1Q GATE_T = {{1,0}, {0,0}, {0,0}, {0.707107,0.707107}}; // T gate

// 应用单量子比特门
void apply_gate_1q(QuantumState *qs, int target, const Gate1Q *gate) {
    int mask = 1 << target;

    // 对所有状态对 (i, i|mask) 应用门
    for (int i = 0; i < qs->n_states; i++) {
        if ((i & mask) == 0) {  // target位为0的状态
            int j = i | mask;    // target位为1的状态

            Complex a0 = qs->amplitudes[i];
            Complex a1 = qs->amplitudes[j];

            qs->amplitudes[i] = cmplx_add(cmplx_mul(gate->m00, a0),
                                           cmplx_mul(gate->m01, a1));
            qs->amplitudes[j] = cmplx_add(cmplx_mul(gate->m10, a0),
                                           cmplx_mul(gate->m11, a1));
        }
    }
}

// CNOT门（两量子比特）
void apply_cnot(QuantumState *qs, int control, int target) {
    int c_mask = 1 << control;
    int t_mask = 1 << target;

    for (int i = 0; i < qs->n_states; i++) {
        // 当控制位为1时，翻转目标位
        if ((i & c_mask) != 0 && (i & t_mask) == 0) {
            int j = i | t_mask;
            // 交换振幅
            Complex tmp = qs->amplitudes[i];
            qs->amplitudes[i] = qs->amplitudes[j];
            qs->amplitudes[j] = tmp;
        }
    }
}

// 旋转门
Gate1Q rotation_gate(double theta, char axis) {
    double c = cos(theta / 2);
    double s = sin(theta / 2);

    switch (axis) {
        case 'X': return (Gate1Q){{c,0}, {0,-s}, {0,-s}, {c,0}};
        case 'Y': return (Gate1Q){{c,0}, {-s,0}, {s,0}, {c,0}};
        case 'Z': return (Gate1Q){{c,-s}, {0,0}, {0,0}, {c,s}};
        default: return GATE_Z;
    }
}
