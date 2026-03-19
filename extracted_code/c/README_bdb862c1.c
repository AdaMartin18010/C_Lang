/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\README.md
 * Line: 123
 * Language: c
 * Block ID: bdb862c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 量子门：酉变换矩阵
 */

#include <string.h>

#define N_QUBITS 20
#define STATE_SIZE (1 << N_QUBITS)  // 2^n 状态空间

// 多量子比特态：振幅向量
typedef struct {
    int n_qubits;
    Amplitude amplitudes[STATE_SIZE];
} QuantumState;

// 单量子比特门：2x2 矩阵
typedef struct {
    Amplitude m[2][2];
} Gate1;

// 常用单量子比特门
const Gate1 GATE_I = {{{1, 0}, {0, 1}}};  // 恒等
const Gate1 GATE_X = {{{0, 1}, {1, 0}}};  // Pauli-X (NOT)
const Gate1 GATE_Z = {{{1, 0}, {0, -1}}}; // Pauli-Z
const Gate1 GATE_H = {                     // Hadamard
    {{1/sqrt(2), 1/sqrt(2)},
     {1/sqrt(2), -1/sqrt(2)}}
};

// 构造旋转门
Gate1 rotation_x(double theta)
{
    Gate1 rx = {{
        {cos(theta/2), -I*sin(theta/2)},
        {-I*sin(theta/2), cos(theta/2)}
    }};
    return rx;
}

Gate1 rotation_y(double theta)
{
    Gate1 ry = {{
        {cos(theta/2), -sin(theta/2)},
        {sin(theta/2), cos(theta/2)}
    }};
    return ry;
}

// 应用单量子比特门到特定量子比特
void apply_gate1(QuantumState *state, int target, const Gate1 *gate)
{
    int n = state->n_qubits;
    int stride = 1 << target;

    // 对所有基态应用门
    for (int i = 0; i < (1 << n); i += 2 * stride) {
        for (int j = 0; j < stride; j++) {
            int idx0 = i + j;
            int idx1 = idx0 + stride;

            Amplitude a0 = state->amplitudes[idx0];
            Amplitude a1 = state->amplitudes[idx1];

            state->amplitudes[idx0] = gate->m[0][0] * a0 + gate->m[0][1] * a1;
            state->amplitudes[idx1] = gate->m[1][0] * a0 + gate->m[1][1] * a1;
        }
    }
}

// CNOT 门（双量子比特）
void apply_cnot(QuantumState *state, int control, int target)
{
    int n = state->n_qubits;
    int c_stride = 1 << control;
    int t_stride = 1 << target;

    for (int i = 0; i < (1 << n); i++) {
        // 检查控制位是否为 1
        if ((i & c_stride) && !(i & t_stride)) {
            int flipped = i | t_stride;
            // 交换振幅
            Amplitude tmp = state->amplitudes[i];
            state->amplitudes[i] = state->amplitudes[flipped];
            state->amplitudes[flipped] = tmp;
        }
    }
}
