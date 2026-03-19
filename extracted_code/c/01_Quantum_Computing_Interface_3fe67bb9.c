/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 90
 * Language: c
 * Block ID: 3fe67bb9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 量子态的C表示
#include <complex.h>
#include <math.h>

typedef double complex Complex;

// 量子比特状态
typedef struct {
    Complex alpha;  // |0⟩的振幅
    Complex beta;   // |1⟩的振幅
} Qubit;

// 基态
const Qubit QUBIT_0 = { 1.0 + 0.0*I, 0.0 + 0.0*I };
const Qubit QUBIT_1 = { 0.0 + 0.0*I, 1.0 + 0.0*I };

// 归一化检查
bool qubit_is_normalized(const Qubit *q) {
    double norm = cabs(q->alpha) * cabs(q->alpha) +
                  cabs(q->beta) * cabs(q->beta);
    return fabs(norm - 1.0) < 1e-10;
}

// 归一化
void qubit_normalize(Qubit *q) {
    double norm = sqrt(cabs(q->alpha) * cabs(q->alpha) +
                       cabs(q->beta) * cabs(q->beta));
    q->alpha /= norm;
    q->beta /= norm;
}
