/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\01_DNA_Synthesis.md
 * Line: 709
 * Language: c
 * Block ID: 8f8c6f0e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 极端GC含量导致合成失败
char *dna = encode_simple(data, len);
// 可能产生 >70% GC 的序列

// ✅ 正确: 实时监控和调整
int encode_gc_balanced(const uint8_t *data, size_t len, char *dna) {
    int gc = 0, total = 0;
    int pos = 0;

    for (size_t i = 0; i < len; i++) {
        for (int j = 3; j >= 0; j--) {
            uint8_t bits = (data[i] >> (j * 2)) & 0x03;

            // 选择使GC接近目标的碱基
            DNABase candidates[2];
            int num_candidates = 0;

            if (gc * 2 < total) {  // GC偏低，优先选GC
                if (bits == 0 || bits == 2) {  // A或G
                    candidates[num_candidates++] = BASE_G;
                } else {
                    candidates[num_candidates++] = BASE_C;
                }
            } else {  // GC偏高，优先选AT
                if (bits == 0 || bits == 2) {
                    candidates[num_candidates++] = BASE_A;
                } else {
                    candidates[num_candidates++] = BASE_T;
                }
            }

            // 选择不违反游程限制的
            DNABase selected = candidates[0];

            dna[pos++] = base_chars[selected];
            if (selected == BASE_G || selected == BASE_C) gc++;
            total++;
        }
    }

    dna[pos] = '\0';
    return pos;
}
