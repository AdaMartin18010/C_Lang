/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\01_Quantum_Simulation_C.md
 * Line: 407
 * Language: c
 * Block ID: cf708022
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// n个量子比特需要2^n个复数
// n=30需要8GB内存！
QuantumState *qs = qstate_create(30);  // 危险！

// ✅ 检查限制
#define MAX_QUBITS 20
QuantumState *safe_create(int n) {
    if (n > MAX_QUBITS) return NULL;
    return qstate_create(n);
}
