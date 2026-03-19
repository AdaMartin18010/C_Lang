/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\README.md
 * Line: 357
 * Language: c
 * Block ID: c1e0d968
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DNA 数据块地址和索引
typedef struct {
    uint32_t block_id;       // 块标识
    uint16_t payload_len;    // 有效载荷长度
    uint8_t rs_parity[32];   // RS 校验
    uint32_t crc32;          // CRC 校验
} DNABlockHeader;

#define DNA_BLOCK_SIZE 200  // DNA 寡核苷酸长度限制

int create_dna_block(uint32_t block_id, const uint8_t *payload,
                     size_t payload_len, char *dna_output,
                     RSEncoder *rs) {
    if (payload_len > DNA_BLOCK_SIZE - sizeof(DNABlockHeader) - 32) {
        return -1;
    }

    DNABlockHeader header = {
        .block_id = block_id,
        .payload_len = payload_len
    };

    // 组合头部和数据
    uint8_t block_data[DNA_BLOCK_SIZE];
    memcpy(block_data, &header, sizeof(header));
    memcpy(block_data + sizeof(header), payload, payload_len);

    // 计算 RS 校验
    rs_encode(rs, block_data, sizeof(header) + payload_len,
              header.rs_parity);

    // 计算 CRC
    header.crc32 = crc32(block_data, sizeof(header) + payload_len);

    // 更新块数据
    memcpy(block_data, &header, sizeof(header));

    // 编码为 DNA
    return encode_to_dna(block_data, sizeof(header) + payload_len + 32,
                         dna_output, DNA_BLOCK_SIZE * 4);
}
