/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\02_Surface_Code_Decoder.md
 * Line: 675
 * Language: c
 * Block ID: a0006b80
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: syndrome历史累积
void decoder_cycle_bad(SurfaceCodeDecoder *dec) {
    SyndromeMeasurement synd;
    extract_syndrome(&dec->code, &synd);

    // 直接累积到历史，没有清除旧的
    for (int i = 0; i < NUM_STABILIZERS; i++) {
        dec->history.x_syndromes[i] |= synd.x_syndromes[i];
    }
}

// ✅ 正确: 使用滑动窗口
void decoder_cycle_good(SurfaceCodeDecoder *dec) {
    SyndromeMeasurement *curr = &dec->history.measurements[round % 3];
    SyndromeMeasurement *prev = &dec->history.measurements[(round - 1) % 3];

    extract_syndrome(&dec->code, curr);

    // 计算差异
    SyndromeMeasurement diff;
    for (int i = 0; i < NUM_STABILIZERS; i++) {
        diff.x_syndromes[i] = curr->x_syndromes[i] ^ prev->x_syndromes[i];
    }
}
