/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 455
 * Language: c
 * Block ID: 56353fc0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：未归一化的量子态
Qubit bad_state = { 1.0, 1.0 };  // 概率和 = 2，无效！

// 正确：归一化
Qubit good_state = { 1.0/sqrt(2), 1.0/sqrt(2) };  // 概率和 = 1

// 或使用辅助函数
Qubit create_superposition(double p0) {
    Qubit q = { sqrt(p0), sqrt(1 - p0) };
    return q;
}
