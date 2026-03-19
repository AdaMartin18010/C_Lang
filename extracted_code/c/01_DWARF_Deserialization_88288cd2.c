/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 1061
 * Language: c
 * Block ID: 88288cd2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：忽略符号扩展
int64_t wrong_sleb128_decode(const uint8_t *data) {
    int64_t result = 0;
    int shift = 0;
    while (true) {
        uint8_t byte = *data++;
        result |= (byte & 0x7F) << shift;
        shift += 7;
        if ((byte & 0x80) == 0) break;
    }
    return result;  // ❌ 缺少符号扩展！
}

// 正确：SLEB128需要符号扩展
int64_t correct_sleb128_decode(const uint8_t *data, size_t *len) {
    int64_t result = 0;
    int shift = 0;
    size_t count = 0;
    uint8_t byte;

    do {
        byte = data[count++];
        result |= ((int64_t)(byte & 0x7F)) << shift;
        shift += 7;
    } while (byte & 0x80);

    // 符号扩展（关键！）
    if ((byte & 0x40) && shift < 64) {
        result |= (~0ULL) << shift;
    }

    *len = count;
    return result;
}
