/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\01_DNA_Synthesis.md
 * Line: 757
 * Language: c
 * Block ID: 41ec8624
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 不同数据块编码后相似，测序时难以区分
// 块1: "ATCGATCG..."
// 块2: "ATCGATCG..." (只差几个碱基)

// ✅ 正确: 添加唯一标识和错位
void encode_with_diversity(const uint8_t *data, size_t len,
                           uint32_t seed, char *dna) {
    // 使用seed扰乱编码表选择
    uint8_t table_offset = seed % 8;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i] ^ (seed & 0xFF);  // XOR扰乱

        for (int j = 3; j >= 0; j--) {
            uint8_t bits = (byte >> (j * 2)) & 0x03;
            uint8_t table_id = (table_offset + i + j) % 8;

            dna[i * 4 + (3-j)] = base_chars[encoding_tables[table_id][bits]];
        }
    }
}
