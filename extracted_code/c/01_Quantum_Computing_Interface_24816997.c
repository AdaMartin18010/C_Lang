/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 471
 * Language: c
 * Block ID: 24816997
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：测量后状态已坍缩
Qubit q = { 1.0/sqrt(2), 1.0/sqrt(2) };
double prob;
MeasurementResult r = measure_qubit(&q, &prob);
// q现在是|0⟩或|1⟩，不再是叠加态
Qubit result = apply_single_gate(&HADAMARD, &q);  // 不是预期的结果！

// 正确：测量后重新初始化或复制
Qubit q_copy = q;
MeasurementResult r = measure_qubit(&q_copy, &prob);
Qubit result = apply_single_gate(&HADAMARD, &q);  // 使用原始态
