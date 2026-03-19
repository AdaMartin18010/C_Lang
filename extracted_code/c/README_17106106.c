/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\README.md
 * Line: 56
 * Language: c
 * Block ID: 17106106
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 二进制到 DNA 序列的基本编码
typedef enum {
    BASE_A = 0,
    BASE_T = 1,
    BASE_C = 2,
    BASE_G = 3
} DNABase;

// 2 bits 转 DNA 碱基
DNABase bits_to_base(uint8_t bits) {
    switch (bits & 0x03) {
        case 0: return BASE_A;
        case 1: return BASE_T;
        case 2: return BASE_C;
        case 3: return BASE_G;
    }
    return BASE_A;
}

// 将二进制数据编码为 DNA 序列
int encode_to_dna(const uint8_t *binary_data, size_t len,
                  char *dna_sequence, size_t dna_buf_size) {
    if (dna_buf_size < len * 4 + 1) {
        return -1;  // 缓冲区不足
    }

    const char base_chars[] = {'A', 'T', 'C', 'G'};
    size_t dna_idx = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = binary_data[i];
        // 每个字节编码为 4 个碱基
        for (int j = 3; j >= 0; j--) {
            uint8_t bits = (byte >> (j * 2)) & 0x03;
            dna_sequence[dna_idx++] = base_chars[bits];
        }
    }
    dna_sequence[dna_idx] = '\0';
    return dna_idx;
}

// DNA 序列解码为二进制
int decode_from_dna(const char *dna_sequence, size_t dna_len,
                    uint8_t *binary_data, size_t bin_buf_size) {
    if (dna_len % 4 != 0 || bin_buf_size < dna_len / 4) {
        return -1;
    }

    size_t bin_idx = 0;
    for (size_t i = 0; i < dna_len; i += 4) {
        uint8_t byte = 0;
        for (int j = 0; j < 4; j++) {
            char base = dna_sequence[i + j];
            uint8_t bits;
            switch (base) {
                case 'A': bits = 0; break;
                case 'T': bits = 1; break;
                case 'C': bits = 2; break;
                case 'G': bits = 3; break;
                default: return -1;  // 无效碱基
            }
            byte = (byte << 2) | bits;
        }
        binary_data[bin_idx++] = byte;
    }
    return bin_idx;
}
