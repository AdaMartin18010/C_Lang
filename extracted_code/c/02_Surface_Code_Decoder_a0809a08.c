/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\02_Surface_Code_Decoder.md
 * Line: 547
 * Language: c
 * Block ID: a0809a08
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 完整表面码解码器
// ============================================================================

typedef struct {
    SurfaceCode code;
    SyndromeHistory history;
    DecoderGraph x_graph;
    DecoderGraph z_graph;

    // 统计
    uint32_t total_decodings;
    uint32_t successful_corrections;
    uint32_t logical_errors;
} SurfaceCodeDecoder;

// 初始化解码器
void decoder_init(SurfaceCodeDecoder *dec, uint8_t distance) {
    surface_code_init(&dec->code, distance);
    memset(&dec->history, 0, sizeof(SyndromeHistory));
    dec->total_decodings = 0;
    dec->successful_corrections = 0;
    dec->logical_errors = 0;
}

// 执行完整解码周期
void decoder_cycle(SurfaceCodeDecoder *dec) {
    SyndromeMeasurement *current =
        &dec->history.measurements[dec->history.current_round];

    // 1. 提取当前syndrome
    extract_syndrome_simulation(&dec->code, current);

    // 2. 如果是第一轮，只做记录
    if (dec->history.current_round == 0) {
        dec->history.current_round = 1;
        return;
    }

    // 3. 计算差异syndrome
    SyndromeMeasurement diff;
    uint8_t prev_idx = (dec->history.current_round - 1) % 3;
    compute_syndrome_difference(&dec->history.measurements[prev_idx],
                                 current, &diff);

    // 4. 解码X错误 (从Z syndrome)
    union_find_decode(&diff, &dec->code, 'Z');

    // 5. 解码Z错误 (从X syndrome)
    union_find_decode(&diff, &dec->code, 'X');

    // 6. 更新历史
    dec->history.current_round = (dec->history.current_round + 1) % 3;
    dec->total_decodings++;

    // 7. 检查逻辑错误 (模拟)
    check_logical_error(dec);
}

// 检查是否存在逻辑错误
bool check_logical_error(SurfaceCodeDecoder *dec) {
    // 计算逻辑X和逻辑Z算符的奇偶性
    // 如果存在奇数个X错误穿过逻辑Z链，则产生逻辑X错误
    // 反之亦然

    // 简化: 统计边界错误
    int x_logical_error = 0;
    int z_logical_error = 0;

    for (int i = 0; i < TOTAL_QUBITS; i++) {
        Qubit *q = &dec->code.qubits[i];
        if (q->is_logical) {
            if (q->error == ERROR_X || q->error == ERROR_Y) {
                x_logical_error++;
            }
            if (q->error == ERROR_Z || q->error == ERROR_Y) {
                z_logical_error++;
            }
        }
    }

    bool has_logical_error = (x_logical_error % 2) || (z_logical_error % 2);

    if (has_logical_error) {
        dec->logical_errors++;
    } else {
        dec->successful_corrections++;
    }

    return has_logical_error;
}

// 获取解码统计
void get_decoder_stats(const SurfaceCodeDecoder *dec,
                       float *logical_error_rate,
                       float *success_rate) {
    if (dec->total_decodings > 0) {
        *logical_error_rate = (float)dec->logical_errors / dec->total_decodings;
        *success_rate = (float)dec->successful_corrections / dec->total_decodings;
    } else {
        *logical_error_rate = 0.0f;
        *success_rate = 0.0f;
    }
}
