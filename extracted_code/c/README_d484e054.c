/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\README.md
 * Line: 276
 * Language: c
 * Block ID: d484e054
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file quantum_gates.c
 * @brief 量子门操作实现
 */

#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <string.h>

#define SQRT2 1.4142135623730951
typedef double complex Complex;

/* 2x2 复数矩阵表示单量子比特门 */
typedef struct {
    Complex m[2][2];
} Gate2x2;

/* 定义常用量子门 */
const Gate2x2 GATE_I = {{{1, 0}, {0, 1}}};
const Gate2x2 GATE_X = {{{0, 1}, {1, 0}}};
const Gate2x2 GATE_Z = {{{1, 0}, {0, -1}}};
const Gate2x2 GATE_H = {{{1/SQRT2, 1/SQRT2}, {1/SQRT2, -1/SQRT2}}};
const Gate2x2 GATE_S = {{{1, 0}, {0, 1.0*I}}};

/* 量子态 */
typedef struct {
    Complex a0;  /* |0⟩ 振幅 */
    Complex a1;  /* |1⟩ 振幅 */
} QState;

/**
 * @brief 应用量子门到量子态
 */
void apply_gate(QState *state, const Gate2x2 *gate) {
    Complex new_a0 = gate->m[0][0] * state->a0 + gate->m[0][1] * state->a1;
    Complex new_a1 = gate->m[1][0] * state->a0 + gate->m[1][1] * state->a1;
    state->a0 = new_a0;
    state->a1 = new_a1;
}

/**
 * @brief 打印量子态
 */
void print_state(const QState *s, const char *label) {
    printf("%s:\n", label);
    printf("  |ψ⟩ = (%.4f %+.4fi)|0⟩ + (%.4f %+.4fi)|1⟩\n",
           creal(s->a0), cimag(s->a0), creal(s->a1), cimag(s->a1));
    printf("  |振幅|: |0⟩=%.4f, |1⟩=%.4f\n",
           cabs(s->a0), cabs(s->a1));
}

/**
 * @brief CNOT门（受控非门）- 双量子比特操作
 * @param control 控制量子比特（0或1）
 * @param target 目标量子比特状态
 */
void apply_cnot(int control, QState *target) {
    if (control == 1) {
        /* 当控制位为|1⟩时，目标位翻转 */
        Complex temp = target->a0;
        target->a0 = target->a1;
        target->a1 = temp;
    }
}

/* 量子门操作演示 */
int main() {
    QState q = {1.0+0.0*I, 0.0+0.0*I};  /* |0⟩ */

    printf("=== 量子门操作演示 ===\n\n");

    /* 初始状态 |0⟩ */
    print_state(&q, "初始 |0⟩");

    /* 应用Hadamard门创建叠加态 */
    printf("\n→ 应用 H 门\n");
    apply_gate(&q, &GATE_H);
    print_state(&q, "叠加态 |+⟩");

    /* 应用X门 */
    printf("\n→ 应用 X 门\n");
    apply_gate(&q, &GATE_X);
    print_state(&q, "变换后");

    /* 应用Z门 */
    printf("\n→ 应用 Z 门\n");
    apply_gate(&q, &GATE_Z);
    print_state(&q, "相位翻转后");

    /* 再次应用H门 */
    printf("\n→ 应用 H 门\n");
    apply_gate(&q, &GATE_H);
    print_state(&q, "最终状态");

    return 0;
}
