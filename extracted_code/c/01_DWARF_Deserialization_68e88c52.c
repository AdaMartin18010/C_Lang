/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\01_DWARF_Deserialization.md
 * Line: 129
 * Language: c
 * Block ID: 68e88c52
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ULEB128: 无符号LEB128
uint64_t decode_uleb128(const uint8_t *data, size_t *bytes_read) {
    uint64_t result = 0;
    unsigned shift = 0;
    size_t count = 0;
    uint8_t byte;

    do {
        byte = data[count++];
        result |= ((uint64_t)(byte & 0x7F)) << shift;
        shift += 7;
    } while (byte & 0x80);

    *bytes_read = count;
    return result;
}

// SLEB128: 有符号LEB128
int64_t decode_sleb128(const uint8_t *data, size_t *bytes_read) {
    int64_t result = 0;
    unsigned shift = 0;
    size_t count = 0;
    uint8_t byte;

    do {
        byte = data[count++];
        result |= ((int64_t)(byte & 0x7F)) << shift;
        shift += 7;
    } while (byte & 0x80);

    // 符号扩展
    if ((byte & 0x40) && (shift < 64)) {
        result |= (~0ULL) << shift;
    }

    *bytes_read = count;
    return result;
}

// 编码ULEB128
size_t encode_uleb128(uint64_t value, uint8_t *out) {
    size_t count = 0;
    do {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if (value != 0) byte |= 0x80;
        out[count++] = byte;
    } while (value != 0);
    return count;
}

// 测试LEB128编码
void test_leb128(void) {
    // ULEB128测试向量
    struct { uint64_t value; uint8_t encoded[10]; size_t len; } tests[] = {
        { 0, {0x00}, 1 },
        { 1, {0x01}, 1 },
        { 127, {0x7F}, 1 },
        { 128, {0x80, 0x01}, 2 },
        { 255, {0xFF, 0x01}, 2 },
        { 0x2000, {0x80, 0x40}, 2 },
        { 0x4000, {0x80, 0x80, 0x01}, 3 },
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        uint8_t encoded[10];
        size_t len = encode_uleb128(tests[i].value, encoded);
        assert(len == tests[i].len);
        assert(memcmp(encoded, tests[i].encoded, len) == 0);

        size_t decoded_len;
        uint64_t decoded = decode_uleb128(encoded, &decoded_len);
        assert(decoded == tests[i].value);
        assert(decoded_len == len);
    }
}
