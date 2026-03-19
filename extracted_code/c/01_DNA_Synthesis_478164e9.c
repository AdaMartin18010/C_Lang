/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\01_DNA_Synthesis.md
 * Line: 604
 * Language: c
 * Block ID: 478164e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// DNA序列质量评估
// ============================================================================

// 质量分数
typedef struct {
    double gc_score;            // GC含量分数
    double homopolymer_score;   // 均聚物分数
    double secondary_score;     // 二级结构分数
    double repeat_score;        // 重复序列分数
    double overall_score;       // 总体质量
} SequenceQuality;

// 评估GC含量
double score_gc_content(const char *dna) {
    int len = strlen(dna);
    if (len == 0) return 0.0;

    int gc = 0;
    for (int i = 0; i < len; i++) {
        if (dna[i] == 'G' || dna[i] == 'C') gc++;
    }

    double ratio = (double)gc / len;
    double target = 0.5;

    // 越接近50%分数越高
    return 1.0 - fabs(ratio - target) * 2;
}

// 评估均聚物
double score_homopolymers(const char *dna) {
    int len = strlen(dna);
    if (len == 0) return 1.0;

    int max_run = 0;
    int current_run = 1;
    char last = dna[0];

    for (int i = 1; i < len; i++) {
        if (dna[i] == last) {
            current_run++;
            if (current_run > max_run) {
                max_run = current_run;
            }
        } else {
            current_run = 1;
            last = dna[i];
        }
    }

    // max_run <= 3 为满分
    if (max_run <= 3) return 1.0;
    if (max_run >= 10) return 0.0;

    return 1.0 - (max_run - 3) / 7.0;
}

// 评估重复序列
double score_repeats(const char *dna) {
    int len = strlen(dna);
    int repeat_count = 0;

    // 检查短重复
    for (int pattern_len = 2; pattern_len <= 4; pattern_len++) {
        for (int i = 0; i <= len - pattern_len * 3; i++) {
            // 检查是否有3个连续重复
            bool is_repeat = true;
            for (int k = 0; k < pattern_len && is_repeat; k++) {
                if (dna[i + k] != dna[i + pattern_len + k] ||
                    dna[i + k] != dna[i + 2 * pattern_len + k]) {
                    is_repeat = false;
                }
            }
            if (is_repeat) repeat_count++;
        }
    }

    // 重复越多分数越低
    return fmax(0.0, 1.0 - repeat_count / 10.0);
}

// 综合质量评估
void evaluate_sequence_quality(const char *dna, SequenceQuality *quality) {
    quality->gc_score = score_gc_content(dna);
    quality->homopolymer_score = score_homopolymers(dna);
    quality->secondary_score = has_strong_secondary_structure(dna, strlen(dna)) ? 0.0 : 1.0;
    quality->repeat_score = score_repeats(dna);

    // 加权平均
    quality->overall_score =
        quality->gc_score * 0.25 +
        quality->homopolymer_score * 0.35 +
        quality->secondary_score * 0.25 +
        quality->repeat_score * 0.15;
}
