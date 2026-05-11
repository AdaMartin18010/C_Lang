/*
 * 二进制协议序列化引擎（TLV + VarInt）
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -o test_serializer serializer.c test_serializer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/* ===== VarInt 编码（变长整数，类似 Protocol Buffers） ===== */

/* 编码无符号64位整数，返回写入字节数 */
int encode_varint(uint64_t value, uint8_t *buf, size_t buf_size) {
    int i = 0;
    while (value >= 0x80 && (size_t)i < buf_size) {
        buf[i++] = (uint8_t)(value | 0x80);
        value >>= 7;
    }
    if ((size_t)i < buf_size) {
        buf[i++] = (uint8_t)value;
        return i;
    }
    return -1; /* 缓冲区不足 */
}

/* 解码无符号64位整数，返回读取字节数 */
int decode_varint(const uint8_t *buf, size_t buf_size, uint64_t *out) {
    uint64_t result = 0;
    int shift = 0;
    int i = 0;
    while ((size_t)i < buf_size) {
        uint8_t byte = buf[i++];
        result |= (uint64_t)(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) {
            *out = result;
            return i;
        }
        shift += 7;
        if (shift >= 64) return -1; /* 溢出 */
    }
    return -1; /* 数据不完整 */
}

/* ===== TLV 编码（Tag-Length-Value） ===== */

typedef enum {
    TYPE_INT32  = 0x01,
    TYPE_INT64  = 0x02,
    TYPE_STRING = 0x03,
    TYPE_BYTES  = 0x04,
    TYPE_BOOL   = 0x05,
    TYPE_MSG    = 0x06, /* 嵌套消息 */
} FieldType;

/* 编码字段头（Tag + Length） */
int encode_field_header(uint8_t *buf, size_t buf_size, uint8_t tag, FieldType type, uint32_t length) {
    if (buf_size < 6) return -1;
    buf[0] = tag;
    buf[1] = (uint8_t)type;
    buf[2] = (length >> 24) & 0xFF;
    buf[3] = (length >> 16) & 0xFF;
    buf[4] = (length >> 8) & 0xFF;
    buf[5] = length & 0xFF;
    return 6;
}

/* 解码字段头 */
int decode_field_header(const uint8_t *buf, size_t buf_size, uint8_t *tag, FieldType *type, uint32_t *length) {
    if (buf_size < 6) return -1;
    *tag = buf[0];
    *type = (FieldType)buf[1];
    *length = ((uint32_t)buf[2] << 24) | ((uint32_t)buf[3] << 16) |
              ((uint32_t)buf[4] << 8) | (uint32_t)buf[5];
    return 6;
}

/* ===== 消息构建器 ===== */

typedef struct {
    uint8_t *data;
    size_t len;
    size_t cap;
} Buffer;

static void buf_init(Buffer *b) {
    b->cap = 256;
    b->data = malloc(b->cap);
    b->len = 0;
}

static void buf_free(Buffer *b) {
    free(b->data);
    b->data = NULL;
    b->len = b->cap = 0;
}

static int buf_ensure(Buffer *b, size_t need) {
    if (b->len + need > b->cap) {
        size_t new_cap = b->cap * 2;
        while (new_cap < b->len + need) new_cap *= 2;
        uint8_t *new_data = realloc(b->data, new_cap);
        if (!new_data) return -1;
        b->data = new_data;
        b->cap = new_cap;
    }
    return 0;
}

static void buf_append(Buffer *b, const uint8_t *src, size_t n) {
    if (buf_ensure(b, n) < 0) return;
    memcpy(b->data + b->len, src, n);
    b->len += n;
}

/* 写入字段 */
void msg_write_int32(Buffer *b, uint8_t tag, int32_t value) {
    uint8_t hdr[6];
    int hlen = encode_field_header(hdr, sizeof(hdr), tag, TYPE_INT32, 4);
    buf_append(b, hdr, (size_t)hlen);
    uint8_t v[4] = {
        (value >> 24) & 0xFF, (value >> 16) & 0xFF,
        (value >> 8) & 0xFF, value & 0xFF
    };
    buf_append(b, v, 4);
}

void msg_write_int64(Buffer *b, uint8_t tag, int64_t value) {
    uint8_t hdr[6];
    int hlen = encode_field_header(hdr, sizeof(hdr), tag, TYPE_INT64, 8);
    buf_append(b, hdr, (size_t)hlen);
    uint8_t v[8];
    for (int i = 7; i >= 0; i--) { v[i] = value & 0xFF; value >>= 8; }
    buf_append(b, v, 8);
}

void msg_write_string(Buffer *b, uint8_t tag, const char *str) {
    size_t slen = strlen(str);
    uint8_t hdr[6];
    int hlen = encode_field_header(hdr, sizeof(hdr), tag, TYPE_STRING, (uint32_t)slen);
    buf_append(b, hdr, (size_t)hlen);
    buf_append(b, (const uint8_t *)str, slen);
}

void msg_write_bool(Buffer *b, uint8_t tag, int value) {
    uint8_t hdr[6];
    int hlen = encode_field_header(hdr, sizeof(hdr), tag, TYPE_BOOL, 1);
    buf_append(b, hdr, (size_t)hlen);
    uint8_t v = value ? 1 : 0;
    buf_append(b, &v, 1);
}

/* ===== 消息解析器 ===== */

typedef struct {
    const uint8_t *data;
    size_t len;
    size_t pos;
} MsgReader;

void reader_init(MsgReader *r, const uint8_t *data, size_t len) {
    r->data = data;
    r->len = len;
    r->pos = 0;
}

int reader_next(MsgReader *r, uint8_t *tag, FieldType *type, const uint8_t **value, uint32_t *length) {
    if (r->pos + 6 > r->len) return 0; /* 无更多字段 */
    int hlen = decode_field_header(r->data + r->pos, r->len - r->pos, tag, type, length);
    if (hlen < 0) return -1;
    r->pos += (size_t)hlen;
    if (r->pos + *length > r->len) return -1;
    *value = r->data + r->pos;
    r->pos += *length;
    return 1;
}

/* ===== 测试主函数 ===== */

int main(void) {
    printf("=== VarInt Test ===\n");
    uint8_t buf[16];
    uint64_t decoded;
    int n;

    n = encode_varint(1, buf, sizeof(buf));
    assert(n == 1 && buf[0] == 0x01);
    n = decode_varint(buf, sizeof(buf), &decoded);
    assert(n == 1 && decoded == 1);
    printf("  1 -> OK\n");

    n = encode_varint(150, buf, sizeof(buf));
    assert(n == 2 && buf[0] == 0x96 && buf[1] == 0x01);
    n = decode_varint(buf, sizeof(buf), &decoded);
    assert(n == 2 && decoded == 150);
    printf("  150 -> OK\n");

    n = encode_varint(0xFFFFFFFFFFFFFFFFULL, buf, sizeof(buf));
    assert(n == 10);
    n = decode_varint(buf, sizeof(buf), &decoded);
    assert(n == 10 && decoded == 0xFFFFFFFFFFFFFFFFULL);
    printf("  max -> OK\n");

    printf("=== TLV Message Test ===\n");
    Buffer msg;
    buf_init(&msg);
    msg_write_int32(&msg, 1, 42);
    msg_write_string(&msg, 2, "hello");
    msg_write_bool(&msg, 3, 1);
    msg_write_int64(&msg, 4, 1234567890123LL);
    printf("  Encoded %zu bytes\n", msg.len);

    MsgReader reader;
    reader_init(&reader, msg.data, msg.len);
    uint8_t tag;
    FieldType type;
    const uint8_t *value;
    uint32_t length;
    int fields = 0;
    while ((n = reader_next(&reader, &tag, &type, &value, &length)) > 0) {
        fields++;
        printf("  Field tag=%d type=%d len=%u\n", tag, type, length);
        if (type == TYPE_STRING) {
            char tmp[64];
            size_t cp = length < 63 ? length : 63;
            memcpy(tmp, value, cp);
            tmp[cp] = '\0';
            printf("    value: %s\n", tmp);
        }
    }
    assert(fields == 4);
    assert(n == 0);

    buf_free(&msg);
    printf("All serializer tests passed!\n");
    return 0;
}
