/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\README.md
 * Line: 132
 * Language: c
 * Block ID: 1101968c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RLL(2,∞) 编码 - 限制连续相同碱基数量
#define MAX_HOMOPOLYMER 2

typedef struct {
    char *output;
    size_t out_len;
    size_t capacity;
    char last_base;
    int run_length;
} RLLEncoder;

void rll_encode_init(RLLEncoder *enc, char *buffer, size_t cap) {
    enc->output = buffer;
    enc->capacity = cap;
    enc->out_len = 0;
    enc->last_base = '\0';
    enc->run_length = 0;
}

// 插入干扰碱基打破长游程
char get_alternative_base(char base, int alt_idx) {
    const char alternatives[] = {'A', 'T', 'C', 'G'};
    for (int i = 0; i < 4; i++) {
        if (alternatives[i] != base) {
            if (alt_idx-- == 0) return alternatives[i];
        }
    }
    return alternatives[0];
}

int rll_encode_byte(RLLEncoder *enc, DNABase base) {
    const char base_char = "ATCG"[base];

    if (base_char == enc->last_base) {
        enc->run_length++;
        if (enc->run_length >= MAX_HOMOPOLYMER) {
            // 插入干扰碱基
            char alt = get_alternative_base(base_char, 0);
            if (enc->out_len >= enc->capacity) return -1;
            enc->output[enc->out_len++] = alt;
            enc->run_length = 0;
        }
    } else {
        enc->last_base = base_char;
        enc->run_length = 1;
    }

    if (enc->out_len >= enc->capacity) return -1;
    enc->output[enc->out_len++] = base_char;
    return 0;
}
