/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\01_H264_Decoding.md
 * Line: 287
 * Language: c
 * Block ID: 8f22bc7d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CAVLC (Context-Adaptive Variable Length Coding)
// 用于Baseline Profile

typedef struct {
    uint8_t *data;
    int size;
    int bit_pos;

    // 上下文
    int total_coeffs[3];  // 非零系数计数
    int trailing_ones[3];
} CAVLCContext;

// 系数Token解码
typedef struct {
    int total_coeffs;
    int trailing_ones;
} CoeffToken;

// CoeffToken码表（简化）
static const struct {
    uint8_t codeword;
    int len;
    int total_coeffs;
    int trailing_ones;
} coeff_token_table[] = {
    {0x1, 1, 0, 0},      // 1 -> (0, 0)
    {0x3, 2, 1, 1},      // 11 -> (1, 1)
    {0x2, 2, 1, 0},      // 10 -> (1, 0)
    // ... 更多条目
};

// 解码CoeffToken
int decode_coeff_token(CAVLCContext *ctx, int nC, CoeffToken *token) {
    // 根据nC选择码表
    // nC = (nA + nB + 1) >> 1，其中nA、nB是相邻块的非零系数

    int bits = show_bits(ctx, 16);

    // 查表解码
    for (int i = 0; i < sizeof(coeff_token_table)/sizeof(coeff_token_table[0]); i++) {
        int mask = (1 << coeff_token_table[i].len) - 1;
        if ((bits >> (16 - coeff_token_table[i].len)) == coeff_token_table[i].codeword) {
            skip_bits(ctx, coeff_token_table[i].len);
            token->total_coeffs = coeff_token_table[i].total_coeffs;
            token->trailing_ones = coeff_token_table[i].trailing_ones;
            return 0;
        }
    }
    return -1;
}

// Level解码
typedef struct {
    int suffix_length;
    int level_prefix;
    int level_suffix;
} LevelInfo;

int decode_level(CAVLCContext *ctx, LevelInfo *info, int *level) {
    // 解码level_prefix（前缀零计数）
    int leading_zeros = 0;
    while (read_bit(ctx) == 0) leading_zeros++;
    info->level_prefix = leading_zeros;

    // 解码level_suffix
    if (info->suffix_length > 0) {
        info->level_suffix = read_bits(ctx, info->suffix_length);
    } else {
        info->level_suffix = 0;
    }

    int level_code = (info->level_prefix << info->suffix_length) + info->level_suffix;

    // 调整suffix_length
    if (info->suffix_length == 0) info->suffix_length = 1;
    if (abs(*level) > (3 << (info->suffix_length - 1)) && info->suffix_length < 6) {
        info->suffix_length++;
    }

    // 处理符号
    int level_val = (level_code + 2) >> 1;
    if (level_code & 1) level_val = -level_val;
    *level = level_val;

    return 0;
}
