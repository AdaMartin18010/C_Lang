/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\01_Quantum_Computing_Interface.md
 * Line: 125
 * Language: c
 * Block ID: 16b4e4b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 布洛赫球坐标
typedef struct {
    double theta;  // 极角 [0, π]
    double phi;    // 方位角 [0, 2π]
} BlochSphere;

// 量子态到布洛赫球坐标
BlochSphere qubit_to_bloch(const Qubit *q) {
    BlochSphere b;

    // 计算概率
    double p0 = cabs(q->alpha) * cabs(q->alpha);
    double p1 = cabs(q->beta) * cabs(q->beta);

    // θ = 2 * arccos(|α|)
    b.theta = 2.0 * acos(sqrt(p0));

    // φ = arg(β) - arg(α)
    b.phi = carg(q->beta) - carg(q->alpha);
    if (b.phi < 0) b.phi += 2.0 * M_PI;

    return b;
}

// 布洛赫球坐标到量子态
Qubit bloch_to_qubit(const BlochSphere *b) {
    Qubit q;
    q.alpha = cos(b->theta / 2.0);
    q.beta = sin(b->theta / 2.0) * cexp(I * b->phi);
    return q;
}

// 布洛赫球可视化（ASCII）
void bloch_visualize(const Qubit *q) {
    BlochSphere b = qubit_to_bloch(q);
    printf("布洛赫球坐标:\n");
    printf("  θ = %.4f (%.2f°)\n", b.theta, b.theta * 180.0 / M_PI);
    printf("  φ = %.4f (%.2f°)\n", b.phi, b.phi * 180.0 / M_PI);
    printf("  |0⟩概率: %.4f\n", cos(b.theta / 2.0) * cos(b.theta / 2.0));
    printf("  |1⟩概率: %.4f\n", sin(b.theta / 2.0) * sin(b.theta / 2.0));
}
