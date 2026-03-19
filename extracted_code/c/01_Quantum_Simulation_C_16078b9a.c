/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\01_Quantum_Simulation_C.md
 * Line: 261
 * Language: c
 * Block ID: 16078b9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Grover搜索算法
// 在N=2^n个元素中搜索目标

typedef bool (*OracleFn)(int index);

// Oracle：标记目标状态
void apply_oracle(QuantumState *qs, OracleFn oracle) {
    for (int i = 0; i < qs->n_states; i++) {
        if (oracle(i)) {
            // 目标状态相位翻转
            qs->amplitudes[i] = CMPLX_MAKE(-CMPLX_REAL(qs->amplitudes[i]),
                                            -CMPLX_IMAG(qs->amplitudes[i]));
        }
    }
}

// 扩散算子（反射）
void apply_diffusion(QuantumState *qs) {
    int n = qs->n_states;

    // 计算平均振幅
    Complex mean = CMPLX_MAKE(0, 0);
    for (int i = 0; i < n; i++) {
        mean = cmplx_add(mean, qs->amplitudes[i]);
    }
    mean = CMPLX_MAKE(CMPLX_REAL(mean) / n, CMPLX_IMAG(mean) / n);

    // 反射：新振幅 = 2*mean - 原振幅
    for (int i = 0; i < n; i++) {
        double new_real = 2*CMPLX_REAL(mean) - CMPLX_REAL(qs->amplitudes[i]);
        double new_imag = 2*CMPLX_IMAG(mean) - CMPLX_IMAG(qs->amplitudes[i]);
        qs->amplitudes[i] = CMPLX_MAKE(new_real, new_imag);
    }
}

// Grover搜索
int grover_search(int n_qubits, OracleFn oracle) {
    QuantumState *qs = qstate_create(n_qubits);

    // 1. 初始化：所有量子比特应用H门
    for (int i = 0; i < n_qubits; i++) {
        apply_gate_1q(qs, i, &GATE_H);
    }

    // 2. 迭代（约 pi/4 * sqrt(N) 次）
    int iterations = (int)(M_PI / 4 * sqrt(1 << n_qubits));

    for (int iter = 0; iter < iterations; iter++) {
        apply_oracle(qs, oracle);
        apply_diffusion(qs);
    }

    // 3. 测量
    MeasurementResult m = qstate_measure(qs, 0);
    int result = m.outcome;

    // 测量所有量子比特得到索引
    for (int i = 1; i < n_qubits; i++) {
        MeasurementResult m2 = qstate_measure(qs, i);
        result |= (m2.outcome << i);
    }

    qstate_destroy(qs);
    qstate_destroy(m.new_state);

    return result;
}
