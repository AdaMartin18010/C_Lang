/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\01_H264_Decoding.md
 * Line: 116
 * Language: c
 * Block ID: 029cf463
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// SPS关键参数
typedef struct {
    uint8_t profile_idc;
    uint8_t level_idc;
    uint8_t seq_parameter_set_id;

    // 图像尺寸
    uint32_t pic_width_in_mbs;
    uint32_t pic_height_in_map_units;
    uint8_t frame_mbs_only_flag;

    // 裁剪参数
    uint8_t frame_cropping_flag;
    uint32_t frame_crop_left_offset;
    uint32_t frame_crop_right_offset;
    uint32_t frame_crop_top_offset;
    uint32_t frame_crop_bottom_offset;

    // 计算出的实际尺寸
    uint32_t width;
    uint32_t height;
} SPS;

// 指数哥伦布解码
int exp_golomb_decode(uint8_t *data, int *bit_pos, int *value) {
    int leading_zeros = 0;

    // 计算前导零个数
    while (read_bit(data, (*bit_pos)++) == 0) {
        leading_zeros++;
    }

    // 读取后续比特
    int code_num = (1 << leading_zeros) - 1;
    for (int i = leading_zeros - 1; i >= 0; i--) {
        code_num += read_bit(data, (*bit_pos)++) << i;
    }

    *value = code_num;
    return 0;
}

// 解析SPS
int parse_sps(uint8_t *data, int size, SPS *sps) {
    int bit_pos = 0;

    sps->profile_idc = read_bits(data, &bit_pos, 8);
    read_bits(data, &bit_pos, 1);  // constraint_set0_flag
    read_bits(data, &bit_pos, 1);  // constraint_set1_flag
    read_bits(data, &bit_pos, 1);  // constraint_set2_flag
    read_bits(data, &bit_pos, 5);  // reserved_zero_5bits
    sps->level_idc = read_bits(data, &bit_pos, 8);

    exp_golomb_decode(data, &bit_pos, &sps->seq_parameter_set_id);

    if (sps->profile_idc == 100 || sps->profile_idc == 110 ||
        sps->profile_idc == 122 || sps->profile_idc == 244 ||
        sps->profile_idc == 44 || sps->profile_idc == 83 ||
        sps->profile_idc == 86 || sps->profile_idc == 118) {
        // 解析chroma_format_idc等...
    }

    exp_golomb_decode(data, &bit_pos, (int*)&sps->pic_width_in_mbs);
    sps->pic_width_in_mbs++;

    exp_golomb_decode(data, &bit_pos, (int*)&sps->pic_height_in_map_units);
    sps->pic_height_in_map_units++;

    sps->frame_mbs_only_flag = read_bits(data, &bit_pos, 1);

    // 计算实际图像尺寸
    sps->width = sps->pic_width_in_mbs * 16;
    sps->height = sps->pic_height_in_map_units * 16;

    if (sps->frame_cropping_flag) {
        // 应用裁剪...
    }

    return 0;
}
