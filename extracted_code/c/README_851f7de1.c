/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\README.md
 * Line: 215
 * Language: c
 * Block ID: 851f7de1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 量子测量：波函数坍缩
 */

#include <stdlib.h>
#include <time.h>

// 初始化随机数生成
void init_random(void)
{
    srand((unsigned)time(NULL));
}

// 测量单量子比特，返回 0 或 1
int measure_qubit(const QuantumState *state, int target)
{
    double prob0 = 0.0;
    int stride = 1 << target;

    // 计算测量结果为 0 的概率
    for (int i = 0; i < (1 << state->n_qubits); i++) {
        if (!(i & stride)) {
            prob0 += cabs(state->amplitudes[i]) *
                     cabs(state->amplitudes[i]);
        }
    }

    // 随机决定是否坍缩到 0
    double r = (double)rand() / RAND_MAX;
    return (r < prob0) ? 0 : 1;
}

// 测量并坍缩态
int measure_and_collapse(QuantumState *state, int target)
{
    double prob0 = 0.0;
    int stride = 1 << target;
    int n = state->n_qubits;

    // 计算概率
    for (int i = 0; i < (1 << n); i++) {
        if (!(i & stride)) {
            prob0 += cabs(state->amplitudes[i]) *
                     cabs(state->amplitudes[i]);
        }
    }

    double r = (double)rand() / RAND_MAX;
    int result = (r < prob0) ? 0 : 1;

    // 坍缩：将与结果不一致的振幅置零
    for (int i = 0; i < (1 << n); i++) {
        int bit = (i & stride) ? 1 : 0;
        if (bit != result) {
            state->amplitudes[i] = 0;
        }
    }

    // 重新归一化
    double norm = 0.0;
    for (int i = 0; i < (1 << n); i++) {
        norm += cabs(state->amplitudes[i]) * cabs(state->amplitudes[i]);
    }
    norm = sqrt(norm);
    for (int i = 0; i < (1 << n); i++) {
        state->amplitudes[i] /= norm;
    }

    return result;
}

// 期望值测量
double expectation_value(const QuantumState *state,
                         const Amplitude operator_matrix[2][2],
                         int target)
{
    // 计算 ⟨ψ|O|ψ⟩
    QuantumState temp = *state;
    apply_gate1(&temp, target, &(Gate1){{
        {operator_matrix[0][0], operator_matrix[0][1]},
        {operator_matrix[1][0], operator_matrix[1][1]}
    }});

    double expectation = 0.0;
    for (int i = 0; i < (1 << state->n_qubits); i++) {
        expectation += creal(conj(state->amplitudes[i]) *
                            temp.amplitudes[i]);
    }
    return expectation;
}
