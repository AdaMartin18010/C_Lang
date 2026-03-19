/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\README.md
 * Line: 113
 * Language: c
 * Block ID: 6d07fafb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file qubit.c
 * @brief 量子比特基础实现
 */

#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>

/* 定义复数类型 */
typedef double complex Complex;

/* 量子比特结构体 */
typedef struct {
    Complex alpha;  /* |0⟩ 的振幅 */
    Complex beta;   /* |1⟩ 的振幅 */
} Qubit;

/* 量子态向量（多量子比特） */
typedef struct {
    int n_qubits;       /* 量子比特数量 */
    int dim;            /* 状态空间维度 = 2^n */
    Complex *amplitudes; /* 振幅数组 */
} QuantumState;

/**
 * @brief 初始化量子比特到 |0⟩ 状态
 */
void qubit_init(Qubit *q) {
    q->alpha = 1.0 + 0.0*I;
    q->beta = 0.0 + 0.0*I;
}

/**
 * @brief 初始化量子比特到 |1⟩ 状态
 */
void qubit_init_one(Qubit *q) {
    q->alpha = 0.0 + 0.0*I;
    q->beta = 1.0 + 0.0*I;
}

/**
 * @brief 创建叠加态 |ψ⟩ = α|0⟩ + β|1⟩
 */
void qubit_set_superposition(Qubit *q, Complex alpha, Complex beta) {
    /* 归一化 */
    double norm = cabs(alpha) * cabs(alpha) + cabs(beta) * cabs(beta);
    norm = sqrt(norm);
    q->alpha = alpha / norm;
    q->beta = beta / norm;
}

/**
 * @brief 测量量子比特，返回 0 或 1
 */
int qubit_measure(Qubit *q) {
    double prob0 = cabs(q->alpha) * cabs(q->alpha);
    double random = (double)rand() / RAND_MAX;

    /* 测量后量子态坍缩 */
    if (random < prob0) {
        q->alpha = 1.0 + 0.0*I;
        q->beta = 0.0 + 0.0*I;
        return 0;
    } else {
        q->alpha = 0.0 + 0.0*I;
        q->beta = 1.0 + 0.0*I;
        return 1;
    }
}

/**
 * @brief 打印量子比特状态
 */
void qubit_print(const Qubit *q, const char *label) {
    printf("%s: |ψ⟩ = ", label);
    printf("(%.3f%+.3fi)|0⟩ + ", creal(q->alpha), cimag(q->alpha));
    printf("(%.3f%+.3fi)|1⟩\n", creal(q->beta), cimag(q->beta));
    printf("       P(|0⟩) = %.3f, P(|1⟩) = %.3f\n",
           cabs(q->alpha)*cabs(q->alpha),
           cabs(q->beta)*cabs(q->beta));
}

/* 使用示例 */
int main() {
    srand(42);

    Qubit q;

    /* 初始化到 |0⟩ */
    qubit_init(&q);
    qubit_print(&q, "初始状态");

    /* 创建叠加态 |+⟩ = (|0⟩ + |1⟩)/√2 */
    qubit_set_superposition(&q, 1.0+0.0*I, 1.0+0.0*I);
    qubit_print(&q, "叠加态 |+⟩");

    /* 测量 */
    int result = qubit_measure(&q);
    printf("测量结果: %d\n", result);
    qubit_print(&q, "测量后状态");

    return 0;
}
