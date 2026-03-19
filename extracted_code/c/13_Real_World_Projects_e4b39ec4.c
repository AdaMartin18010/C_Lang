/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 2955
 * Language: c
 * Block ID: e4b39ec4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RESP Protocol Implementation
 * Redis Serialization Protocol
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "resp.h"

/* Find CRLF in buffer, return pointer or NULL */
static const char* find_crlf(const char *data, size_t len) {
    for (size_t i = 0; i + 1 < len; i++) {
        if (data[i] == '\r' && data[i+1] == '\n') {
            return &data[i];
        }
    }
    return NULL;
}

/* Parse integer from RESP */
static int64_t parse_integer(const char *data, size_t len) {
    int64_t val = 0;
    int sign = 1;
    size_t i = 0;

    if (i < len && (data[i] == '+' || data[i] == '-')) {
        sign = (data[i] == '-') ? -1 : 1;
        i++;
    }

    for (; i < len && isdigit((unsigned char)data[i]); i++) {
        val = val * 10 + (data[i] - '0');
    }

    return val * sign;
}

int resp_parse(resp_value_t *out, const char *data, size_t len, size_t *consumed) {
    if (len < 3) return -1;  /* Need at least type + \r\n */

    out->type = (resp_type_t)data[0];
    const char *line_end = find_crlf(data + 1, len - 1);
    if (!line_end) return -1;

    size_t line_len = (size_t)(line_end - (data + 1));

    switch (out->type) {
        case RESP_STRING:
        case RESP_ERROR:
            out->data.str.data = malloc(line_len + 1);
            if (!out->data.str.data) return -1;
            memcpy(out->data.str.data, data + 1, line_len);
            out->data.str.data[line_len] = '\0';
            out->data.str.len = line_len;
            *consumed = line_len + 3;  /* type + line + \r\n */
            break;

        case RESP_INTEGER:
            out->data.integer = parse_integer(data + 1, line_len);
            *consumed = line_len + 3;
            break;

        case RESP_BULK: {
            int64_t bulk_len = parse_integer(data + 1, line_len);
            if (bulk_len < 0) {
                /* Null bulk string */
                out->type = RESP_NULL;
                out->data.str.data = NULL;
                out->data.str.len = 0;
                *consumed = line_len + 3;
                break;
            }

            size_t total_needed = (size_t)(line_end - data) + 2 + (size_t)bulk_len + 2;
            if (len < total_needed) return -1;

            out->data.str.data = malloc((size_t)bulk_len + 1);
            if (!out->data.str.data) return -1;

            memcpy(out->data.str.data, line_end + 2, (size_t)bulk_len);
            out->data.str.data[bulk_len] = '\0';
            out->data.str.len = (size_t)bulk_len;
            *consumed = total_needed;
            break;
        }

        case RESP_ARRAY: {
            int64_t arr_len = parse_integer(data + 1, line_len);
            if (arr_len < 0) {
                out->type = RESP_NULL;
                out->data.array.items = NULL;
                out->data.array.count = 0;
                *consumed = line_len + 3;
                break;
            }

            out->data.array.count = (size_t)arr_len;
            out->data.array.items = calloc((size_t)arr_len, sizeof(resp_value_t*));
            if (!out->data.array.items) return -1;

            size_t pos = (size_t)(line_end - data) + 2;

            for (size_t i = 0; i < (size_t)arr_len; i++) {
                out->data.array.items[i] = malloc(sizeof(resp_value_t));
                if (!out->data.array.items[i]) return -1;

                size_t item_consumed = 0;
                int ret = resp_parse(out->data.array.items[i], data + pos,
                                    len - pos, &item_consumed);
                if (ret != 0) return -1;

                pos += item_consumed;
            }

            *consumed = pos;
            break;
        }

        default:
            return -1;  /* Unknown type */
    }

    return 0;
}

int resp_serialize(const resp_value_t *value, char *buf, size_t buf_size) {
    int pos = 0;

    switch (value->type) {
        case RESP_STRING:
            pos += snprintf(buf + pos, buf_size - pos, "+%s\r\n",
                           value->data.str.data);
            break;

        case RESP_ERROR:
            pos += snprintf(buf + pos, buf_size - pos, "-%s\r\n",
                           value->data.str.data);
            break;

        case RESP_INTEGER:
            pos += snprintf(buf + pos, buf_size - pos, ":%ld\r\n", value->data.integer);
            break;

        case RESP_BULK:
            pos += snprintf(buf + pos, buf_size - pos, "$%zu\r\n",
                           value->data.str.len);
            if (value->data.str.len > 0) {
                memcpy(buf + pos, value->data.str.data, value->data.str.len);
                pos += (int)value->data.str.len;
            }
            pos += snprintf(buf + pos, buf_size - pos, "\r\n");
            break;

        case RESP_NULL:
            pos += snprintf(buf + pos, buf_size - pos, "$-1\r\n");
            break;

        case RESP_ARRAY:
            pos += snprintf(buf + pos, buf_size - pos, "*%zu\r\n",
                           value->data.array.count);
            for (size_t i = 0; i < value->data.array.count; i++) {
                int item_len = resp_serialize(value->data.array.items[i],
                                             buf + pos, buf_size - pos);
                pos += item_len;
            }
            break;

        default:
            return -1;
    }

    return pos;
}

resp_value_t* resp_create_string(const char *str, size_t len) {
    resp_value_t *val = calloc(1, sizeof(resp_value_t));
    if (!val) return NULL;

    val->type = RESP_STRING;
    val->data.str.data = malloc(len + 1);
    if (!val->data.str.data) {
        free(val);
        return NULL;
    }
    memcpy(val->data.str.data, str, len);
    val->data.str.data[len] = '\0';
    val->data.str.len = len;

    return val;
}

resp_value_t* resp_create_error(const char *str) {
    resp_value_t *val = resp_create_string(str, strlen(str));
    if (val) val->type = RESP_ERROR;
    return val;
}

resp_value_t* resp_create_integer(int64_t val) {
    resp_value_t *v = calloc(1, sizeof(resp_value_t));
    if (!v) return NULL;

    v->type = RESP_INTEGER;
    v->data.integer = val;
    return v;
}

resp_value_t* resp_create_bulk(const char *data, size_t len) {
    resp_value_t *val = calloc(1, sizeof(resp_value_t));
    if (!val) return NULL;

    val->type = RESP_BULK;
    val->data.str.data = malloc(len + 1);
    if (!val->data.str.data) {
        free(val);
        return NULL;
    }
    memcpy(val->data.str.data, data, len);
    val->data.str.data[len] = '\0';
    val->data.str.len = len;

    return val;
}

resp_value_t* resp_create_null(void) {
    resp_value_t *val = calloc(1, sizeof(resp_value_t));
    if (!val) return NULL;
    val->type = RESP_NULL;
    return val;
}

resp_value_t* resp_create_array(size_t count) {
    resp_value_t *val = calloc(1, sizeof(resp_value_t));
    if (!val) return NULL;

    val->type = RESP_ARRAY;
    val->data.array.count = count;
    val->data.array.items = calloc(count, sizeof(resp_value_t*));
    if (!val->data.array.items) {
        free(val);
        return NULL;
    }

    return val;
}

void resp_free(resp_value_t *val) {
    if (!val) return;

    switch (val->type) {
        case RESP_STRING:
        case RESP_ERROR:
        case RESP_BULK:
            free(val->data.str.data);
            break;

        case RESP_ARRAY:
            for (size_t i = 0; i < val->data.array.count; i++) {
                resp_free(val->data.array.items[i]);
            }
            free(val->data.array.items);
            break;

        default:
            break;
    }

    free(val);
}

resp_value_t* resp_array_append(resp_value_t *arr, resp_value_t *item) {
    if (!arr || arr->type != RESP_ARRAY) return NULL;

    size_t new_count = arr->data.array.count + 1;
    resp_value_t **new_items = realloc(arr->data.array.items,
                                       new_count * sizeof(resp_value_t*));
    if (!new_items) return NULL;

    arr->data.array.items = new_items;
    arr->data.array.items[arr->data.array.count] = item;
    arr->data.array.count = new_count;

    return arr;
}
