/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 518
 * Language: c
 * Block ID: 45e743f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 量子傅里叶变换实现
// QFT是Shor算法的核心组件

void apply_qft(Qubit *qubits, int n) {
    for (int i = 0; i < n; i++) {
        // Hadamard门
        apply_hadamard(&qubits[i]);

        // 控制旋转门
        for (int j = i + 1; j < n; j++) {
            double angle = M_PI / (1 << (j - i));
            apply_controlled_rotation(&qubits[i], &qubits[j], angle);
        }
    }

    // 反转量子比特顺序
    for (int i = 0; i < n / 2; i++) {
        swap_qubits(&qubits[i], &qubits[n - 1 - i]);
    }
}

// 逆QFT
void apply_inverse_qft(Qubit *qubits, int n) {
    // 先反转
    for (int i = 0; i < n / 2; i++) {
        swap_qubits(&qubits[i], &qubits[n - 1 - i]);
    }

    // 应用共轭转置
    for (int i = n - 1; i >= 0; i--) {
        for (int j = n - 1; j > i; j--) {
            double angle = -M_PI / (1 << (j - i));
            apply_controlled_rotation(&qubits[i], &qubits[j], angle);
        }
        apply_hadamard(&qubits[i]);
    }
}
