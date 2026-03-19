/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 2810
 * Language: c
 * Block ID: 8c8cca48
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef RESP_H
#define RESP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* RESP types */
typedef enum {
    RESP_STRING = '+',      /* Simple String */
    RESP_ERROR = '-',       /* Error */
    RESP_INTEGER = ':',     /* Integer */
    RESP_BULK = '$',        /* Bulk String */
    RESP_ARRAY = '*',       /* Array */
    RESP_NULL = '_',        /* Null (RESP3) */
    RESP_BOOL = '#',        /* Boolean (RESP3) */
    RESP_DOUBLE = ',',      /* Double (RESP3) */
} resp_type_t;

typedef struct resp_value {
    resp_type_t type;
    union {
        int64_t integer;
        double dbl;
        bool boolean;
        struct {
            char *data;
            size_t len;
        } str;
        struct {
            struct resp_value **items;
            size_t count;
        } array;
    } data;
} resp_value_t;

/* RESP parsing/serialization */
int resp_parse(resp_value_t *out, const char *data, size_t len, size_t *consumed);
int resp_serialize(const resp_value_t *value, char *buf, size_t buf_size);
resp_value_t* resp_create_string(const char *str, size_t len);
resp_value_t* resp_create_error(const char *str);
resp_value_t* resp_create_integer(int64_t val);
resp_value_t* resp_create_bulk(const char *data, size_t len);
resp_value_t* resp_create_null(void);
resp_value_t* resp_create_array(size_t count);
void resp_free(resp_value_t *val);
resp_value_t* resp_array_append(resp_value_t *arr, resp_value_t *item);

/* Convenience macros for responses */
#define RESP_OK resp_create_string("OK", 2)
#define RESP_PONG resp_create_string("PONG", 4)
#define RESP_NIL resp_create_null()

#endif /* RESP_H */
