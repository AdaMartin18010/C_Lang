/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_RESP_Protocol.md
 * Line: 464
 * Language: c
 * Block ID: 9e97335b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 缓冲区写入器 */
typedef struct {
    char *buf;
    size_t pos;
    size_t capacity;
} Buffer;

static void buf_append(Buffer *b, const char *data, size_t len) {
    if (b->pos + len > b->capacity) {
        b->capacity = (b->pos + len) * 2;
        b->buf = realloc(b->buf, b->capacity);
    }
    memcpy(b->buf + b->pos, data, len);
    b->pos += len;
}

static void buf_append_crlf(Buffer *b) {
    buf_append(b, "\r\n", 2);
}

/* 序列化整数 */
static void serialize_int(Buffer *b, int64_t val) {
    char tmp[32];
    int len = snprintf(tmp, sizeof(tmp), "%lld", (long long)val);
    buf_append(b, tmp, len);
}

/* 主序列化函数 */
int resp_serialize(Buffer *b, const RESPValue *v) {
    switch (v->type) {
    case RESP_SIMPLE_STRING:
        buf_append(b, "+", 1);
        buf_append(b, v->value.simple.data, v->value.simple.len);
        buf_append_crlf(b);
        break;

    case RESP_ERROR:
        buf_append(b, "-", 1);
        buf_append(b, v->value.simple.data, v->value.simple.len);
        buf_append_crlf(b);
        break;

    case RESP_INTEGER:
        buf_append(b, ":", 1);
        serialize_int(b, v->value.integer);
        buf_append_crlf(b);
        break;

    case RESP_BULK_STRING:
    case RESP_NULL:
        buf_append(b, "$", 1);
        if (v->type == RESP_NULL) {
            buf_append(b, "-1", 2);
        } else {
            serialize_int(b, v->value.bulk.len);
            buf_append_crlf(b);
            buf_append(b, v->value.bulk.data, v->value.bulk.len);
        }
        buf_append_crlf(b);
        break;

    case RESP_ARRAY:
    case RESP_NULL_ARRAY:
        buf_append(b, "*", 1);
        if (v->type == RESP_NULL_ARRAY) {
            buf_append(b, "-1", 2);
            buf_append_crlf(b);
        } else {
            serialize_int(b, v->value.array.count);
            buf_append_crlf(b);
            for (size_t i = 0; i < v->value.array.count; i++) {
                resp_serialize(b, v->value.array.elements[i]);
            }
        }
        break;
    }

    return 0;
}

/* 便捷构造器 */
Buffer resp_format_simple_string(const char *str) {
    Buffer b = {malloc(256), 0, 256};
    RESPValue v = {.type = RESP_SIMPLE_STRING};
    strncpy(v.value.simple.data, str, sizeof(v.value.simple.data) - 1);
    v.value.simple.len = strlen(v.value.simple.data);
    resp_serialize(&b, &v);
    return b;
}

Buffer resp_format_error(const char *err) {
    Buffer b = {malloc(256), 0, 256};
    RESPValue v = {.type = RESP_ERROR};
    strncpy(v.value.simple.data, err, sizeof(v.value.simple.data) - 1);
    v.value.simple.len = strlen(v.value.simple.data);
    resp_serialize(&b, &v);
    return b;
}

Buffer resp_format_integer(int64_t n) {
    Buffer b = {malloc(32), 0, 32};
    RESPValue v = {.type = RESP_INTEGER, .value.integer = n};
    resp_serialize(&b, &v);
    return b;
}
