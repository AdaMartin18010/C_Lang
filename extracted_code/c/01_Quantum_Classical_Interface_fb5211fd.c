/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\01_Quantum_Classical_Interface.md
 * Line: 808
 * Language: c
 * Block ID: fb5211fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 连续小角度旋转累积误差
for (int i = 0; i < 1000; i++) {
    rotate_z(qubit, M_PI / 1000);  // 累积舍入误差
}

// ✅ 正确: 预计算总角度，一次应用
rotate_z(qubit, M_PI);  // 直接应用π

// ✅ 或使用虚拟Z门跟踪相位
accumulate_virtual_phase(qubit, M_PI / 1000);  // 纯软件跟踪
