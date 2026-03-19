/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 281
 * Language: c
 * Block ID: 415f8726
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 测量结果
typedef enum {
    MEASURE_0 = 0,
    MEASURE_1 = 1
} MeasurementResult;

// 单量子比特测量
MeasurementResult measure_qubit(Qubit *q, double *probability) {
    double p0 = cabs(q->alpha) * cabs(q->alpha);
    double r = (double)rand() / RAND_MAX;

    MeasurementResult result;
    if (r < p0) {
        result = MEASURE_0;
        *probability = p0;
        // 坍缩到 |0⟩
        q->alpha = 1.0;
        q->beta = 0.0;
    } else {
        result = MEASURE_1;
        *probability = 1.0 - p0;
        // 坍缩到 |1⟩
        q->alpha = 0.0;
        q->beta = 1.0;
    }

    return result;
}

// 投影测量
typedef struct {
    Complex matrix[2][2];
} Projector;

// 计算期望值
double expectation_value(const Qubit *q, const SingleQubitGate *observable) {
    // ⟨ψ|O|ψ⟩
    Qubit Opsi = apply_single_gate(observable, q);
    Complex expectation = conj(q->alpha) * Opsi.alpha +
                          conj(q->beta) * Opsi.beta;
    return creal(expectation);
}
