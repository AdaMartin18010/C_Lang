/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\README.md
 * Line: 359
 * Language: c
 * Block ID: f3c4c100
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Grover 搜索：在未排序数据库中搜索
 * 经典：O(N) 次查询
 * 量子：O(√N) 次查询
 */

#include <math.h>

void grover_diffusion(QuantumState *state, int n)
{
    // 扩散算子：2|ψ⟩⟨ψ| - I

    // 对所有量子比特应用 H
    for (int i = 0; i < n; i++) {
        apply_gate1(state, i, &GATE_H);
    }

    // 条件相位移动
    for (int i = 0; i < (1 << n); i++) {
        if (i != 0) {
            state->amplitudes[i] *= -1;
        }
    }

    // 再次应用 H
    for (int i = 0; i < n; i++) {
        apply_gate1(state, i, &GATE_H);
    }
}

void grover_oracle(QuantumState *state, int n, int target)
{
    // 标记目标态：相位翻转
    state->amplitudes[target] *= -1;
}

int grover_search(int n, int (*oracle)(int))
{
    QuantumState state;
    state.n_qubits = n;

    // 初始化均匀叠加态
    double amp = 1.0 / sqrt(1 << n);
    for (int i = 0; i < (1 << n); i++) {
        state.amplitudes[i] = amp;
    }

    // 找到目标
    int target = -1;
    for (int i = 0; i < (1 << n); i++) {
        if (oracle(i)) {
            target = i;
            break;
        }
    }

    if (target < 0) return -1;  // 目标不存在

    // 迭代次数：π/4 * √N
    int iterations = (int)(M_PI / 4 * sqrt(1 << n));

    for (int iter = 0; iter < iterations; iter++) {
        grover_oracle(&state, n, target);
        grover_diffusion(&state, n);
    }

    // 测量
    double max_prob = 0;
    int result = 0;
    for (int i = 0; i < (1 << n); i++) {
        double prob = cabs(state.amplitudes[i]) *
                      cabs(state.amplitudes[i]);
        if (prob > max_prob) {
            max_prob = prob;
            result = i;
        }
    }

    return result;
}

// 示例：搜索函数 f(x) = (x == 42)
int example_oracle(int x)
{
    return x == 42;
}
