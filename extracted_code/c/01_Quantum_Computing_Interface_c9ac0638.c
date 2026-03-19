/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 224
 * Language: c
 * Block ID: c9ac0638
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CNOT门（受控非门）
// 控制位 |0⟩: 目标位不变
// 控制位 |1⟩: 目标位翻转

// 对于两量子比特系统，使用4x4矩阵
typedef struct {
    Complex m[4][4];
} TwoQubitGate;

const TwoQubitGate CNOT = {
    {{1, 0, 0, 0},
     {0, 1, 0, 0},
     {0, 0, 0, 1},
     {0, 0, 1, 0}}
};

// SWAP门
const TwoQubitGate SWAP = {
    {{1, 0, 0, 0},
     {0, 0, 1, 0},
     {0, 1, 0, 0},
     {0, 0, 0, 1}}
};

// 两量子比特状态
typedef struct {
    Complex amp[4];  // |00⟩, |01⟩, |10⟩, |11⟩的振幅
} TwoQubitState;

// 应用双量子门
TwoQubitState apply_two_qubit_gate(const TwoQubitGate *gate,
                                    const TwoQubitState *state) {
    TwoQubitState result = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.amp[i] += gate->m[i][j] * state->amp[j];
        }
    }
    return result;
}

// 创建贝尔态（最大纠缠态）
TwoQubitState create_bell_state(void) {
    // |Φ+⟩ = (|00⟩ + |11⟩) / √2
    // H|0⟩ ⊗ |0⟩ = |+0⟩ = (|00⟩ + |10⟩) / √2
    // CNOT(|+0⟩) = (|00⟩ + |11⟩) / √2

    TwoQubitState state = {0};
    state.amp[0] = 1.0 / sqrt(2.0);  // |00⟩
    state.amp[3] = 1.0 / sqrt(2.0);  // |11⟩
    return state;
}
