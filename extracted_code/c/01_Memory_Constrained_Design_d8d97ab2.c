/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 2042
 * Language: c
 * Block ID: d8d97ab2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 轻量级压缩算法
 * ============================================================================ */

/* 行程编码 (RLE) - 适合重复数据 */
int rle_encode(const uint8_t *in, int in_len, uint8_t *out, int out_max)
{
    int in_pos = 0, out_pos = 0;

    while (in_pos < in_len && out_pos < out_max) {
        uint8_t count = 1;
        uint8_t value = in[in_pos];

        /* 统计重复次数 */
        while (in_pos + count < in_len &&
               count < 255 &&
               in[in_pos + count] == value) {
            count++;
        }

        if (count >= 3) {
            /* 压缩: [0x00] [count] [value] */
            if (out_pos + 3 > out_max) break;
            out[out_pos++] = 0x00;      /* 压缩标记 */
            out[out_pos++] = count;
            out[out_pos++] = value;
        } else {
            /* 不压缩，直接复制 */
            for (int i = 0; i < count && out_pos < out_max; i++) {
                out[out_pos++] = value;
            }
        }

        in_pos += count;
    }

    return out_pos;
}

int rle_decode(const uint8_t *in, int in_len, uint8_t *out, int out_max)
{
    int in_pos = 0, out_pos = 0;

    while (in_pos < in_len && out_pos < out_max) {
        if (in[in_pos] == 0x00) {
            /* 压缩块 */
            if (in_pos + 2 >= in_len) break;
            uint8_t count = in[in_pos + 1];
            uint8_t value = in[in_pos + 2];

            for (int i = 0; i < count && out_pos < out_max; i++) {
                out[out_pos++] = value;
            }
            in_pos += 3;
        } else {
            /* 原始字节 */
            out[out_pos++] = in[in_pos++];
        }
    }

    return out_pos;
}

/* 增量编码 - 传感器数据压缩 */
int delta_encode(const int16_t *in, int count, int8_t *out, int out_max)
{
    if (count == 0 || out_max < count) return 0;

    /* 第一个值完整存储 */
    out[0] = (int8_t)(in[0] >> 8);
    if (out_max > 1) out[1] = (int8_t)(in[0] & 0xFF);

    int out_pos = 2;
    int16_t prev = in[0];

    for (int i = 1; i < count && out_pos < out_max; i++) {
        int16_t delta = in[i] - prev;

        /* 检查是否可以用8位表示 */
        if (delta >= -128 && delta <= 127) {
            out[out_pos++] = (int8_t)delta;
        } else {
            /* 溢出，存储特殊标记和完整值 */
            if (out_pos + 3 > out_max) break;
            out[out_pos++] = 0x80;  /* 溢出标记 */
            out[out_pos++] = (int8_t)(in[i] >> 8);
            out[out_pos++] = (int8_t)(in[i] & 0xFF);
        }

        prev = in[i];
    }

    return out_pos;
}
