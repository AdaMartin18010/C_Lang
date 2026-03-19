/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 173
 * Language: c
 * Block ID: 712e50e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 单量子门：2x2酉矩阵
typedef struct {
    Complex m[2][2];
} SingleQubitGate;

// 泡利门
const SingleQubitGate PAULI_X = {  // NOT门
    {{0, 1},
     {1, 0}}
};

const SingleQubitGate PAULI_Y = {
    {{0, -I},
     {I, 0}}
};

const SingleQubitGate PAULI_Z = {  // 相位翻转
    {{1, 0},
     {0, -1}}
};

// Hadamard门：创建叠加态
const SingleQubitGate HADAMARD = {
    {{1.0/sqrt(2), 1.0/sqrt(2)},
     {1.0/sqrt(2), -1.0/sqrt(2)}}
};

// 相位门 S
const SingleQubitGate PHASE_S = {
    {{1, 0},
     {0, I}}
};

// T门
const SingleQubitGate PHASE_T = {
    {{1, 0},
     {0, cexp(I * M_PI / 4.0)}}
};

// 应用单量子门
Qubit apply_single_gate(const SingleQubitGate *gate, const Qubit *q) {
    Qubit result;
    result.alpha = gate->m[0][0] * q->alpha + gate->m[0][1] * q->beta;
    result.beta  = gate->m[1][0] * q->alpha + gate->m[1][1] * q->beta;
    return result;
}
