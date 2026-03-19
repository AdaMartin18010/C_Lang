/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\02_Surface_Code_Decoder.md
 * Line: 660
 * Language: c
 * Block ID: 64201818
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 边界稳定子只有2-3个邻居
Stabilizer *stab = &sc->x_stabilizers[i];
for (int j = 0; j < 4; j++) {  // 错误! 可能只有2个邻居
    Qubit *q = &sc->qubits[stab->data_qubits[j]];
}

// ✅ 正确: 使用num_neighbors
for (int j = 0; j < stab->num_neighbors; j++) {
    Qubit *q = &sc->qubits[stab->data_qubits[j]];
}
