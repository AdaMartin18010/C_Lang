/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\01_Quantum_Classical_Interface.md
 * Line: 116
 * Language: c
 * Block ID: d00c60da
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 量子电路表示与编译
// 支持OpenQASM 3.0子集
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <complex.h>
#include <math.h>

#define MAX_QUBITS      32
#define MAX_GATES       1024
#define MAX_PARAMS      4

// 标准单量子比特门
typedef enum {
    GATE_I,     // 恒等门
    GATE_X,     // Pauli-X (NOT)
    GATE_Y,     // Pauli-Y
    GATE_Z,     // Pauli-Z
    GATE_H,     // Hadamard
    GATE_S,     // Phase
    GATE_SDAG,  // S-dagger
    GATE_T,     // T gate
    GATE_TDAG,  // T-dagger
    GATE_RX,    // X旋转
    GATE_RY,    // Y旋转
    GATE_RZ,    // Z旋转
    GATE_U1,    // U1相位
    GATE_U2,    // U2通用
    GATE_U3,    // U3通用
    NUM_SINGLE_QUBIT_GATES
} SingleQubitGateType;

// 双量子比特门
typedef enum {
    GATE_CX,    // CNOT
    GATE_CY,    // C-Y
    GATE_CZ,    // C-Z
    GATE_SWAP,  // SWAP
    GATE_CPHASE,// 受控相位
    NUM_TWO_QUBIT_GATES
} TwoQubitGateType;

// 三量子比特门
typedef enum {
    GATE_CCX,   // Toffoli
    GATE_CCZ,   // 双控Z
    NUM_THREE_QUBIT_GATES
} ThreeQubitGateType;

// 门定义
typedef struct {
    uint8_t num_qubits;
    uint8_t num_params;
    char name[16];
    void (*matrix_func)(double params[], double complex matrix[]);
} GateDefinition;

// 门实例 (电路中的具体门)
typedef struct {
    uint16_t gate_id;       // 门的类型ID
    uint8_t qubits[3];      // 作用的量子比特 (最多3个)
    double params[MAX_PARAMS]; // 参数 (如旋转角度)
    uint32_t duration_ns;   // 持续时间(纳秒)
    uint32_t start_time_ns; // 开始时间
} GateInstruction;

// 量子电路
typedef struct {
    uint8_t num_qubits;
    uint32_t num_gates;
    GateInstruction gates[MAX_GATES];
    uint32_t total_duration_ns;
} QuantumCircuit;

// ============================================================================
// 标准门矩阵定义
// ============================================================================

void get_pauli_x_matrix(double complex m[4]) {
    m[0] = 0; m[1] = 1;
    m[2] = 1; m[3] = 0;
}

void get_pauli_y_matrix(double complex m[4]) {
    m[0] = 0;       m[1] = -I;
    m[2] = I;       m[3] = 0;
}

void get_pauli_z_matrix(double complex m[4]) {
    m[0] = 1; m[1] = 0;
    m[2] = 0; m[3] = -1;
}

void get_hadamard_matrix(double complex m[4]) {
    double inv_sqrt2 = 1.0 / sqrt(2.0);
    m[0] = inv_sqrt2;  m[1] = inv_sqrt2;
    m[2] = inv_sqrt2;  m[3] = -inv_sqrt2;
}

void get_rotation_x_matrix(double theta, double complex m[4]) {
    double c = cos(theta / 2);
    double s = sin(theta / 2);
    m[0] = c;    m[1] = -I * s;
    m[2] = -I * s; m[3] = c;
}

void get_rotation_y_matrix(double theta, double complex m[4]) {
    double c = cos(theta / 2);
    double s = sin(theta / 2);
    m[0] = c;  m[1] = -s;
    m[2] = s;  m[3] = c;
}

void get_rotation_z_matrix(double theta, double complex m[4]) {
    m[0] = cexp(-I * theta / 2);  m[1] = 0;
    m[2] = 0;                     m[3] = cexp(I * theta / 2);
}

// ============================================================================
// 通用U3门: U(θ, φ, λ) = Rz(φ)Ry(θ)Rz(λ)
// ============================================================================

void get_u3_matrix(double theta, double phi, double lambda, double complex m[4]) {
    double c = cos(theta / 2);
    double s = sin(theta / 2);

    m[0] = c;
    m[1] = -cexpl(I * lambda) * s;
    m[2] = cexpl(I * phi) * s;
    m[3] = cexpl(I * (phi + lambda)) * c;
}

// ============================================================================
// 电路构建API
// ============================================================================

void circuit_init(QuantumCircuit *circ, uint8_t n_qubits) {
    circ->num_qubits = n_qubits;
    circ->num_gates = 0;
    circ->total_duration_ns = 0;
}

int circuit_add_gate(QuantumCircuit *circ, SingleQubitGateType gate,
                     uint8_t qubit, double params[], uint32_t duration_ns) {
    if (circ->num_gates >= MAX_GATES) return -1;
    if (qubit >= circ->num_qubits) return -1;

    GateInstruction *inst = &circ->gates[circ->num_gates++];
    inst->gate_id = gate;
    inst->qubits[0] = qubit;
    inst->duration_ns = duration_ns;
    inst->start_time_ns = circ->total_duration_ns;

    // 复制参数
    int num_params = (gate == GATE_U3) ? 3 : (gate == GATE_U2) ? 2 :
                     (gate == GATE_RX || gate == GATE_RY || gate == GATE_RZ) ? 1 : 0;
    for (int i = 0; i < num_params && i < MAX_PARAMS; i++) {
        inst->params[i] = params[i];
    }

    circ->total_duration_ns += duration_ns;
    return 0;
}

int circuit_add_cnot(QuantumCircuit *circ, uint8_t control, uint8_t target,
                     uint32_t duration_ns) {
    if (circ->num_gates >= MAX_GATES) return -1;
    if (control >= circ->num_qubits || target >= circ->num_qubits) return -1;
    if (control == target) return -1;

    GateInstruction *inst = &circ->gates[circ->num_gates++];
    inst->gate_id = GATE_CX + NUM_SINGLE_QUBIT_GATES;
    inst->qubits[0] = control;
    inst->qubits[1] = target;
    inst->duration_ns = duration_ns;
    inst->start_time_ns = circ->total_duration_ns;

    circ->total_duration_ns += duration_ns;
    return 0;
}

// ============================================================================
// 电路优化: 门合并与消除
// ============================================================================

void optimize_circuit(QuantumCircuit *circ) {
    // 1. 恒等门消除
    for (int i = 0; i < (int)circ->num_gates; i++) {
        if (circ->gates[i].gate_id == GATE_I) {
            // 删除此门 (将后续门前移)
            for (int j = i; j < (int)circ->num_gates - 1; j++) {
                circ->gates[j] = circ->gates[j + 1];
            }
            circ->num_gates--;
            i--;  // 重新检查当前位置
        }
    }

    // 2. 连续旋转门合并
    for (int i = 0; i < (int)circ->num_gates - 1; i++) {
        GateInstruction *g1 = &circ->gates[i];
        GateInstruction *g2 = &circ->gates[i + 1];

        // 同一量子比特上的连续Rz门可以合并
        if (g1->gate_id == GATE_RZ && g2->gate_id == GATE_RZ &&
            g1->qubits[0] == g2->qubits[0]) {
            g1->params[0] += g2->params[0];  // 角度相加
            // 删除g2
            for (int j = i + 1; j < (int)circ->num_gates - 1; j++) {
                circ->gates[j] = circ->gates[j + 1];
            }
            circ->num_gates--;
        }
    }

    // 3. 重新计算时间
    circ->total_duration_ns = 0;
    for (int i = 0; i < (int)circ->num_gates; i++) {
        circ->gates[i].start_time_ns = circ->total_duration_ns;
        circ->total_duration_ns += circ->gates[i].duration_ns;
    }
}

// ============================================================================
// OpenQASM解析器 (简化版)
// ============================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int parse_openqasm_line(const char *line, QuantumCircuit *circ) {
    char gate_name[32];
    int qubits[3];
    double params[3];
    int n;

    // 跳过空白和注释
    while (*line == ' ' || *line == '\t') line++;
    if (*line == '\0' || *line == '/' || *line == '\n') return 0;

    // 解析门名称
    n = sscanf(line, "%31s", gate_name);
    if (n != 1) return -1;

    // 解析不同门
    if (strcmp(gate_name, "h") == 0) {
        if (sscanf(line, "h q[%d]", &qubits[0]) == 1) {
            return circuit_add_gate(circ, GATE_H, qubits[0], NULL, 35);
        }
    } else if (strcmp(gate_name, "x") == 0) {
        if (sscanf(line, "x q[%d]", &qubits[0]) == 1) {
            return circuit_add_gate(circ, GATE_X, qubits[0], NULL, 25);
        }
    } else if (strcmp(gate_name, "cx") == 0) {
        if (sscanf(line, "cx q[%d], q[%d]", &qubits[0], &qubits[1]) == 2) {
            return circuit_add_cnot(circ, qubits[0], qubits[1], 400);
        }
    } else if (strcmp(gate_name, "rz") == 0) {
        if (sscanf(line, "rz(%lf) q[%d]", &params[0], &qubits[0]) == 2) {
            return circuit_add_gate(circ, GATE_RZ, qubits[0], params, 0);
        }
    }
    // 更多门类型...

    return 0;
}
