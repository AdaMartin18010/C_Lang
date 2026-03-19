/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\01_DNA_Synthesis.md
 * Line: 121
 * Language: c
 * Block ID: a73ad4dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// DNA存储编码基础
// 符合ISO/IEC 21794标准
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// DNA碱基类型
typedef enum {
    BASE_A = 0,  // 腺嘌呤 (Adenine)
    BASE_C = 1,  // 胞嘧啶 (Cytosine)
    BASE_G = 2,  // 鸟嘌呤 (Guanine)
    BASE_T = 3   // 胸腺嘧啶 (Thymine)
} DNABase;

// 碱基字符表示
static const char base_chars[] = "ACGT";

// 碱基互补配对
static const DNABase base_complement[] = {
    [BASE_A] = BASE_T,
    [BASE_C] = BASE_G,
    [BASE_G] = BASE_C,
    [BASE_T] = BASE_A
};

// ============================================================================
// 基础编码方案
// ============================================================================

/*
 * 简单2-bit编码: 每字节编码为4个碱基
 * 00 -> A, 01 -> C, 10 -> G, 11 -> T
 */

// 编码表 (8种映射方案，用于游程限制)
static const uint8_t encoding_tables[8][4] = {
    {BASE_A, BASE_C, BASE_G, BASE_T},  // 0: ACGT
    {BASE_A, BASE_G, BASE_C, BASE_T},  // 1: AGCT
    {BASE_A, BASE_T, BASE_C, BASE_G},  // 2: ATCG
    {BASE_C, BASE_A, BASE_G, BASE_T},  // 3: CAGT
    {BASE_C, BASE_G, BASE_A, BASE_T},  // 4: CGAT
    {BASE_C, BASE_T, BASE_A, BASE_G},  // 5: CTAG
    {BASE_G, BASE_A, BASE_C, BASE_T},  // 6: GACT
    {BASE_G, BASE_C, BASE_A, BASE_T},  // 7: GCAT
};

// 简单编码 (不考虑生化约束)
int encode_simple(const uint8_t *data, size_t len, char *dna, uint8_t table_id) {
    if (table_id >= 8) return -1;

    const uint8_t *table = encoding_tables[table_id];
    size_t dna_pos = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        // 每字节编码为4个碱基
        for (int j = 3; j >= 0; j--) {
            uint8_t bits = (byte >> (j * 2)) & 0x03;
            dna[dna_pos++] = base_chars[table[bits]];
        }
    }

    dna[dna_pos] = '\0';
    return (int)dna_pos;
}

// 简单解码
int decode_simple(const char *dna, size_t dna_len, uint8_t *data, uint8_t table_id) {
    if (table_id >= 8) return -1;
    if (dna_len % 4 != 0) return -1;

    const uint8_t *table = encoding_tables[table_id];
    size_t data_pos = 0;

    for (size_t i = 0; i < dna_len; i += 4) {
        uint8_t byte = 0;

        for (int j = 0; j < 4; j++) {
            char base = dna[i + j];
            uint8_t bits = 0;

            // 查找碱基对应的2-bit值
            for (int k = 0; k < 4; k++) {
                if (base_chars[table[k]] == base) {
                    bits = k;
                    break;
                }
            }

            byte = (byte << 2) | bits;
        }

        data[data_pos++] = byte;
    }

    return (int)data_pos;
}
