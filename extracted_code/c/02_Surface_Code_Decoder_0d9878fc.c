/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\02_Surface_Code_Decoder.md
 * Line: 266
 * Language: c
 * Block ID: 0d9878fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// Syndrome提取电路
// 测量所有稳定子算符
// ============================================================================

// syndrome测量结果
typedef struct {
    uint8_t x_syndromes[NUM_X_STABILIZERS];
    uint8_t z_syndromes[NUM_Z_STABILIZERS];
    uint32_t measurement_round;     // 测量轮次
} SyndromeMeasurement;

// 综合征历史 (用于时间解码)
typedef struct {
    SyndromeMeasurement measurements[3];  // 保存3轮测量
    uint8_t current_round;
} SyndromeHistory;

// ============================================================================
// 模拟综合征提取 (用于测试)
// ============================================================================

void extract_syndrome_simulation(SurfaceCode *sc, SyndromeMeasurement *synd) {
    // X稳定子: 测量X⊗X⊗X⊗X
    // syndrome = 1 如果数据量子比特上有奇数个X错误

    for (int i = 0; i < NUM_X_STABILIZERS; i++) {
        Stabilizer *stab = &sc->x_stabilizers[i];
        int x_count = 0;

        for (int j = 0; j < stab->num_neighbors; j++) {
            Qubit *q = &sc->qubits[stab->data_qubits[j]];
            if (q->error == ERROR_X || q->error == ERROR_Y) {
                x_count++;
            }
        }

        stab->syndrome_value = x_count % 2;
        synd->x_syndromes[i] = stab->syndrome_value;
    }

    // Z稳定子: 测量Z⊗Z⊗Z⊗Z
    // syndrome = 1 如果数据量子比特上有奇数个Z错误

    for (int i = 0; i < NUM_Z_STABILIZERS; i++) {
        Stabilizer *stab = &sc->z_stabilizers[i];
        int z_count = 0;

        for (int j = 0; j < stab->num_neighbors; j++) {
            Qubit *q = &sc->qubits[stab->data_qubits[j]];
            if (q->error == ERROR_Z || q->error == ERROR_Y) {
                z_count++;
            }
        }

        stab->syndrome_value = z_count % 2;
        synd->z_syndromes[i] = stab->syndrome_value;
    }
}

// ============================================================================
// 差异Syndrome (检测变化)
// ============================================================================

void compute_syndrome_difference(const SyndromeMeasurement *prev,
                                  const SyndromeMeasurement *curr,
                                  SyndromeMeasurement *diff) {
    // 计算两轮测量间的差异
    // 这对应于新增的错误

    for (int i = 0; i < NUM_X_STABILIZERS; i++) {
        diff->x_syndromes[i] = prev->x_syndromes[i] ^ curr->x_syndromes[i];
    }

    for (int i = 0; i < NUM_Z_STABILIZERS; i++) {
        diff->z_syndromes[i] = prev->z_syndromes[i] ^ curr->z_syndromes[i];
    }
}
