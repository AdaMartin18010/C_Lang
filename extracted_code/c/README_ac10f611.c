/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\README.md
 * Line: 60
 * Language: c
 * Block ID: ac10f611
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * C 语言中的复数与量子态表示 (C99)
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>

// 量子振幅：复数
typedef double complex Amplitude;

// 单量子比特态：|ψ⟩ = α|0⟩ + β|1⟩
typedef struct {
    Amplitude alpha;  // |0⟩ 的振幅
    Amplitude beta;   // |1⟩ 的振幅
} Qubit;

// 计算基态
const Qubit ZERO  = { 1.0, 0.0 };  // |0⟩
const Qubit ONE   = { 0.0, 1.0 };  // |1⟩
const Qubit PLUS  = { 1.0/sqrt(2), 1.0/sqrt(2) };  // |+⟩
const Qubit MINUS = { 1.0/sqrt(2), -1.0/sqrt(2) }; // |−⟩

// 归一化检查
bool is_normalized(const Qubit *q)
{
    double norm = cabs(q->alpha) * cabs(q->alpha) +
                  cabs(q->beta) * cabs(q->beta);
    return fabs(norm - 1.0) < 1e-10;
}

// 归一化
void normalize(Qubit *q)
{
    double norm = sqrt(cabs(q->alpha) * cabs(q->alpha) +
                       cabs(q->beta) * cabs(q->beta));
    q->alpha /= norm;
    q->beta /= norm;
}

// 打印量子态
void print_qubit(const Qubit *q)
{
    printf("|ψ⟩ = (%.4f %+.4fi)|0⟩ + (%.4f %+.4fi)|1⟩\n",
           creal(q->alpha), cimag(q->alpha),
           creal(q->beta), cimag(q->beta));
    printf("    P(|0⟩) = %.4f, P(|1⟩) = %.4f\n",
           cabs(q->alpha) * cabs(q->alpha),
           cabs(q->beta) * cabs(q->beta));
}

// 布洛赫球面坐标
void bloch_coordinates(const Qubit *q, double *theta, double *phi)
{
    // |ψ⟩ = cos(θ/2)|0⟩ + e^(iφ)sin(θ/2)|1⟩
    *theta = 2 * acos(cabs(q->alpha));
    *phi = carg(q->beta) - carg(q->alpha);
}
