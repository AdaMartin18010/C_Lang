/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\01_H264_Decoding.md
 * Line: 70
 * Language: c
 * Block ID: e9daf36c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NAL单元头部
typedef struct {
    uint8_t forbidden_zero_bit : 1;      // 必须为0
    uint8_t nal_ref_idc : 2;             // 参考帧优先级
    uint8_t nal_unit_type : 5;           // NAL类型
} NALHeader;

// NAL类型定义
#define NAL_SLICE_NONIDR    1   // 非IDR片
#define NAL_SLICE_PARTITION_A 2  // 数据分区A
#define NAL_SLICE_PARTITION_B 3  // 数据分区B
#define NAL_SLICE_PARTITION_C 4  // 数据分区C
#define NAL_SLICE_IDR       5   // IDR片
#define NAL_SEI             6   // 补充增强信息
#define NAL_SPS             7   // 序列参数集
#define NAL_PPS             8   // 图像参数集
#define NAL_AUD             9   // 访问单元分隔符

// NAL单元解析
int parse_nal_unit(uint8_t *data, int size, NALHeader *header, uint8_t **payload) {
    if (size < 1) return -1;

    header->forbidden_zero_bit = (data[0] >> 7) & 0x01;
    header->nal_ref_idc = (data[0] >> 5) & 0x03;
    header->nal_unit_type = data[0] & 0x1F;

    *payload = data + 1;
    return size - 1;
}

// 起始码检测 (0x000001 或 0x00000001)
int find_start_code(uint8_t *data, int size, int *start_offset) {
    for (int i = 0; i < size - 3; i++) {
        if (data[i] == 0x00 && data[i+1] == 0x00 &&
            (data[i+2] == 0x01 || (data[i+2] == 0x00 && data[i+3] == 0x01))) {
            *start_offset = i;
            return (data[i+2] == 0x01) ? 3 : 4;
        }
    }
    return -1;
}
