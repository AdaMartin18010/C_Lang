/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_RESP_Protocol.md
 * Line: 235
 * Language: c
 * Block ID: 43a9d91e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 查找CRLF，返回位置或-1 */
static inline int find_crlf(const char *buf, size_t start, size_t len) {
    for (size_t i = start; i + 1 < len; i++) {
        if (buf[i] == '\r' && buf[i + 1] == '\n') {
            return i;
        }
    }
    return -1;
}

/* 解析整数 */
static int64_t parse_integer(const char *buf, size_t start, size_t end) {
    int64_t val = 0;
    bool negative = false;
    size_t i = start;

    if (buf[i] == '-') {
        negative = true;
        i++;
    }

    for (; i < end; i++) {
        val = val * 10 + (buf[i] - '0');
    }

    return negative ? -val : val;
}

/* 主解析函数 - 增量式 */
int resp_parse(RESPParser *p) {
    while (p->pos < p->len) {
        switch (p->state) {
        case PARSE_STATE_INIT:
            p->state = PARSE_STATE_TYPE;
            /* fallthrough */

        case PARSE_STATE_TYPE: {
            char type_char = p->buf[p->pos++];

            switch (type_char) {
            case '+':
                p->current = resp_value_new(RESP_SIMPLE_STRING);
                p->state = PARSE_STATE_INTEGER;  /* 复用：读到CRLF */
                break;
            case '-':
                p->current = resp_value_new(RESP_ERROR);
                p->state = PARSE_STATE_INTEGER;
                break;
            case ':':
                p->current = resp_value_new(RESP_INTEGER);
                p->state = PARSE_STATE_INTEGER;
                break;
            case '$':
                p->current = resp_value_new(RESP_BULK_STRING);
                p->state = PARSE_STATE_BULK_LEN;
                break;
            case '*':
                p->current = resp_value_new(RESP_ARRAY);
                p->state = PARSE_STATE_ARRAY_LEN;
                break;
            default:
                p->state = PARSE_STATE_ERROR;
                return -1;
            }
            break;
        }

        case PARSE_STATE_INTEGER: {
            int crlf_pos = find_crlf(p->buf, p->pos, p->len);
            if (crlf_pos < 0) return 1;  /* 需要更多数据 */

            int64_t val = parse_integer(p->buf, p->pos, crlf_pos);

            if (p->current->type == RESP_INTEGER) {
                p->current->value.integer = val;
            } else {
                /* Simple String / Error */
                size_t len = crlf_pos - p->pos;
                if (len >= sizeof(p->current->value.simple.data)) {
                    len = sizeof(p->current->value.simple.data) - 1;
                }
                memcpy(p->current->value.simple.data, p->buf + p->pos, len);
                p->current->value.simple.data[len] = '\0';
                p->current->value.simple.len = len;
            }

            p->pos = crlf_pos + 2;  /* 跳过\r\n */
            p->state = PARSE_STATE_COMPLETE;
            break;
        }

        case PARSE_STATE_BULK_LEN: {
            int crlf_pos = find_crlf(p->buf, p->pos, p->len);
            if (crlf_pos < 0) return 1;

            int64_t bulk_len = parse_integer(p->buf, p->pos, crlf_pos);
            p->pos = crlf_pos + 2;

            if (bulk_len < 0) {
                /* Null Bulk String */
                p->current->type = RESP_NULL;
                p->state = PARSE_STATE_COMPLETE;
            } else {
                p->pending_bulk_len = bulk_len;
                p->state = PARSE_STATE_BULK_DATA;
            }
            break;
        }

        case PARSE_STATE_BULK_DATA: {
            size_t needed = p->pending_bulk_len + 2;  /* + \r\n */

            if (p->len - p->pos < needed) {
                return 1;  /* 需要更多数据 */
            }

            /* 零拷贝：直接指向缓冲区 */
            p->current->value.bulk.data = (char *)(p->buf + p->pos);
            p->current->value.bulk.len = p->pending_bulk_len;
            p->current->value.bulk.owned = false;

            p->pos += needed;
            p->state = PARSE_STATE_COMPLETE;
            break;
        }

        case PARSE_STATE_ARRAY_LEN: {
            int crlf_pos = find_crlf(p->buf, p->pos, p->len);
            if (crlf_pos < 0) return 1;

            int64_t array_len = parse_integer(p->buf, p->pos, crlf_pos);
            p->pos = crlf_pos + 2;

            if (array_len < 0) {
                p->current->type = RESP_NULL_ARRAY;
                p->state = PARSE_STATE_COMPLETE;
            } else {
                p->current->value.array.count = array_len;
                p->current->value.array.elements = calloc(
                    array_len, sizeof(RESPValue *));
                p->array_remaining = array_len;

                if (array_len == 0) {
                    p->state = PARSE_STATE_COMPLETE;
                } else {
                    /* 压栈，递归解析元素 */
                    if (p->stack_depth >= p->stack_capacity) {
                        p->stack_capacity *= 2;
                        p->stack = realloc(p->stack,
                            p->stack_capacity * sizeof(RESPValue *));
                    }
                    p->stack[p->stack_depth++] = p->current;
                    p->state = PARSE_STATE_TYPE;
                }
            }
            break;
        }

        case PARSE_STATE_COMPLETE: {
            /* 检查是否在数组内 */
            if (p->stack_depth > 0) {
                RESPValue *parent = p->stack[p->stack_depth - 1];
                size_t idx = parent->value.array.count - p->array_remaining;
                parent->value.array.elements[idx] = p->current;
                p->array_remaining--;

                if (p->array_remaining == 0) {
                    /* 数组完成 */
                    p->current = p->stack[--p->stack_depth];
                    p->state = PARSE_STATE_COMPLETE;
                } else {
                    /* 继续解析下一个元素 */
                    p->state = PARSE_STATE_TYPE;
                }
            } else {
                /* 顶层对象完成 */
                return 0;
            }
            break;
        }

        case PARSE_STATE_ERROR:
            return -1;
        }
    }

    return 1;  /* 需要更多数据 */
}
