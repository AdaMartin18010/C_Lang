/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\README.md
 * Line: 591
 * Language: c
 * Block ID: 962dfbed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 简单量子模拟器核心
 */

#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUBITS 20

typedef struct {
    int num_qubits;
    double complex amplitudes[1 << MAX_QUBITS];
} QSimulator;

// 初始化模拟器
void qs_init(QSimulator *qs, int n)
{
    qs->num_qubits = n;
    memset(qs->amplitudes, 0, sizeof(qs->amplitudes));
    qs->amplitudes[0] = 1.0;  // |0...0⟩
}

// 获取状态大小
static inline int qs_state_size(const QSimulator *qs)
{
    return 1 << qs->num_qubits;
}

// 应用单量子比特门
void qs_apply_u(QSimulator *qs, int target,
                double complex u00, double complex u01,
                double complex u10, double complex u11)
{
    int n = qs->num_qubits;
    int mask = 1 << target;

    for (int i = 0; i < (1 << n); i++) {
        if (!(i & mask)) {
            int j = i | mask;
            double complex ai = qs->amplitudes[i];
            double complex aj = qs->amplitudes[j];

            qs->amplitudes[i] = u00 * ai + u01 * aj;
            qs->amplitudes[j] = u10 * ai + u11 * aj;
        }
    }
}

// 预定义门
void qs_hadamard(QSimulator *qs, int target)
{
    double s = 1.0 / sqrt(2.0);
    qs_apply_u(qs, target, s, s, s, -s);
}

void qs_pauli_x(QSimulator *qs, int target)
{
    qs_apply_u(qs, target, 0, 1, 1, 0);
}

void qs_pauli_z(QSimulator *qs, int target)
{
    qs_apply_u(qs, target, 1, 0, 0, -1);
}

void qs_phase(QSimulator *qs, int target, double theta)
{
    qs_apply_u(qs, target, 1, 0, 0, cexp(I * theta));
}

// CNOT 门
void qs_cnot(QSimulator *qs, int control, int target)
{
    int n = qs->num_qubits;
    int c_mask = 1 << control;
    int t_mask = 1 << target;

    for (int i = 0; i < (1 << n); i++) {
        if ((i & c_mask) && !(i & t_mask)) {
            int j = i | t_mask;
            double complex tmp = qs->amplitudes[i];
            qs->amplitudes[i] = qs->amplitudes[j];
            qs->amplitudes[j] = tmp;
        }
    }
}

// Toffoli 门 (CCNOT)
void qs_toffoli(QSimulator *qs, int c1, int c2, int target)
{
    int n = qs->num_qubits;
    int m1 = 1 << c1;
    int m2 = 1 << c2;
    int mt = 1 << target;

    for (int i = 0; i < (1 << n); i++) {
        if ((i & m1) && (i & m2) && !(i & mt)) {
            int j = i | mt;
            double complex tmp = qs->amplitudes[i];
            qs->amplitudes[i] = qs->amplitudes[j];
            qs->amplitudes[j] = tmp;
        }
    }
}

// SWAP 门
void qs_swap(QSimulator *qs, int q1, int q2)
{
    qs_cnot(qs, q1, q2);
    qs_cnot(qs, q2, q1);
    qs_cnot(qs, q1, q2);
}

// 测量
int qs_measure(QSimulator *qs, int target)
{
    int n = qs->num_qubits;
    int mask = 1 << target;

    double prob0 = 0.0;
    for (int i = 0; i < (1 << n); i++) {
        if (!(i & mask)) {
            prob0 += cabs(qs->amplitudes[i]) * cabs(qs->amplitudes[i]);
        }
    }

    int result = ((double)rand() / RAND_MAX) < prob0 ? 0 : 1;

    // 坍缩
    for (int i = 0; i < (1 << n); i++) {
        int bit = (i & mask) ? 1 : 0;
        if (bit != result) {
            qs->amplitudes[i] = 0;
        }
    }

    // 归一化
    double norm = 0.0;
    for (int i = 0; i < (1 << n); i++) {
        norm += cabs(qs->amplitudes[i]) * cabs(qs->amplitudes[i]);
    }
    norm = sqrt(norm);
    for (int i = 0; i < (1 << n); i++) {
        qs->amplitudes[i] /= norm;
    }

    return result;
}

// 打印状态（用于调试）
void qs_print_state(const QSimulator *qs)
{
    printf("Quantum state:\n");
    for (int i = 0; i < (1 << qs->num_qubits); i++) {
        double prob = cabs(qs->amplitudes[i]) * cabs(qs->amplitudes[i]);
        if (prob > 1e-10) {
            printf("|%d⟩: %.4f %+.4fi (prob: %.4f)\n",
                   i, creal(qs->amplitudes[i]),
                   cimag(qs->amplitudes[i]), prob);
        }
    }
}
