/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 487
 * Language: c
 * Block ID: b0342bd7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：尝试用经典条件控制量子操作
if (measure_qubit(&q, &prob) == MEASURE_1) {
    apply_x(&target);  // 这不是量子控制！
}

// 正确：使用受控门实现量子控制
// CNOT实现 |c⟩|t⟩ -> |c⟩|t ⊕ c⟩
// 这是真正的量子控制，不测量控制位
TwoQubitState state = { .amp = {1, 0, 0, 0} };  // |00⟩
// 应用Hadamard到控制位
// 应用CNOT
// 现在处于叠加态
