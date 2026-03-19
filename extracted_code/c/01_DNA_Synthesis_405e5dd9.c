/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\01_DNA_Synthesis.md
 * Line: 230
 * Language: c
 * Block ID: 405e5dd9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 生化约束编码
// 解决DNA合成和测序中的实际问题
// ============================================================================

// 约束参数
#define MAX_HOMOPOLYMER_RUN   3       // 最大连续相同碱基数
#define TARGET_GC_MIN         0.40    // 最小GC含量
#define TARGET_GC_MAX         0.60    // 最大GC含量
#define MAX_REPEAT_LENGTH     8       // 最大重复序列长度

// 编码状态
typedef struct {
    uint8_t current_table;      // 当前使用的编码表
    uint8_t last_base;          // 上一个碱基
    uint8_t run_length;         // 当前游程长度
    double gc_count;            // GC计数
    double total_count;         // 总碱基数
} EncodeState;

// 初始化编码状态
void init_encode_state(EncodeState *state) {
    state->current_table = 0;
    state->last_base = 255;  // 无效值
    state->run_length = 0;
    state->gc_count = 0;
    state->total_count = 0;
}

// 检查GC含量
bool check_gc_content(const EncodeState *state) {
    if (state->total_count < 10) return true;  // 太短不检查

    double gc_ratio = state->gc_count / state->total_count;
    return (gc_ratio >= TARGET_GC_MIN && gc_ratio <= TARGET_GC_MAX);
}

// 选择最优编码表 (解决游程限制)
uint8_t select_best_table(const EncodeState *state, uint8_t bits) {
    uint8_t best_table = state->current_table;
    uint8_t min_run = MAX_HOMOPOLYMER_RUN + 1;

    // 尝试所有编码表
    for (uint8_t t = 0; t < 8; t++) {
        DNABase base = encoding_tables[t][bits];

        // 计算使用此表后的游程长度
        uint8_t new_run = 1;
        if (base == state->last_base) {
            new_run = state->run_length + 1;
        }

        // 检查GC含量影响
        bool is_gc = (base == BASE_G || base == BASE_C);
        double new_gc = state->gc_count + (is_gc ? 1 : 0);
        double new_total = state->total_count + 1;
        double new_gc_ratio = new_gc / new_total;

        // 优先选择游程短的
        if (new_run <= MAX_HOMOPOLYMER_RUN &&
            new_gc_ratio >= TARGET_GC_MIN &&
            new_gc_ratio <= TARGET_GC_MAX) {
            if (new_run < min_run) {
                min_run = new_run;
                best_table = t;
            }
        }
    }

    return best_table;
}

// ============================================================================
// 游程限制编码
// ============================================================================

int encode_rll(const uint8_t *data, size_t len, char *dna) {
    EncodeState state;
    init_encode_state(&state);

    size_t dna_pos = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        // 每字节分4个2-bit组编码
        for (int j = 3; j >= 0; j--) {
            uint8_t bits = (byte >> (j * 2)) & 0x03;

            // 选择最优编码表
            state.current_table = select_best_table(&state, bits);

            DNABase base = encoding_tables[state.current_table][bits];

            // 更新游程计数
            if (base == state.last_base) {
                state.run_length++;
            } else {
                state.run_length = 1;
            }

            // 更新GC计数
            if (base == BASE_G || base == BASE_C) {
                state.gc_count++;
            }
            state.total_count++;

            // 输出碱基
            dna[dna_pos++] = base_chars[base];
            state.last_base = base;
        }
    }

    dna[dna_pos] = '\0';
    return (int)dna_pos;
}

// ============================================================================
// 二级结构避免
// ============================================================================

#define MIN_FREE_ENERGY       -3.0    // 最小自由能阈值 (kcal/mol)

// 简单自由能估计 (简化版)
double estimate_free_energy(const char *dna, int start, int length) {
    // 使用简化的最近邻模型
    // 实际应使用NUPACK等专业工具

    double energy = 0.0;

    // 碱基配对能量表 (简化)
    static const double pair_energy[4][4] = {
        // A     C     G     T
        { 0.0,  0.0, -1.0, -1.0},  // A
        { 0.0,  0.0, -2.0,  0.0},  // C
        {-1.0, -2.0,  0.0,  0.0},  // G
        {-1.0,  0.0,  0.0,  0.0}   // T
    };

    for (int i = start; i < start + length - 1 && dna[i+1]; i++) {
        DNABase b1 = (dna[i] == 'A') ? BASE_A :
                     (dna[i] == 'C') ? BASE_C :
                     (dna[i] == 'G') ? BASE_G : BASE_T;
        DNABase b2 = (dna[i+1] == 'A') ? BASE_A :
                     (dna[i+1] == 'C') ? BASE_C :
                     (dna[i+1] == 'G') ? BASE_G : BASE_T;

        energy += pair_energy[b1][b2];
    }

    return energy;
}

// 检查序列是否可能形成强二级结构
bool has_strong_secondary_structure(const char *dna, int length) {
    // 检查反向互补重复
    for (int window = 4; window <= 12 && window <= length; window++) {
        for (int i = 0; i <= length - window * 2; i++) {
            // 检查i到i+window-1和i+window到i+2*window-1是否互补
            bool complementary = true;

            for (int k = 0; k < window && complementary; k++) {
                char b1 = dna[i + k];
                char b2 = dna[i + 2 * window - 1 - k];

                // 检查是否互补
                if (!((b1 == 'A' && b2 == 'T') || (b1 == 'T' && b2 == 'A') ||
                      (b1 == 'C' && b2 == 'G') || (b1 == 'G' && b2 == 'C'))) {
                    complementary = false;
                }
            }

            if (complementary) {
                return true;
            }
        }
    }

    return false;
}
