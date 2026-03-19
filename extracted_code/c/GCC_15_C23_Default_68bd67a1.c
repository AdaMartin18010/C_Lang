/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1890
 * Language: c
 * Block ID: 68bd67a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 高性能 CSV 解析器 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CSV_MAX_FIELDS 128
#define CSV_MAX_LINE 8192

typedef struct {
    char* fields[CSV_MAX_FIELDS];
    size_t field_count;
    size_t line_number;
} csv_row_t;

/* 快速行解析 - GCC 15 自动向量化 */
bool parse_csv_line(const char* line, csv_row_t* row) {
    row->field_count = 0;

    char buffer[CSV_MAX_LINE];
    size_t buf_pos = 0;
    bool in_quotes = false;

    for (const char* p = line; *p; p++) {
        char c = *p;

        if (in_quotes) {
            if (c == '"') {
                if (*(p + 1) == '"') {
                    /* 转义引号 */
                    buffer[buf_pos++] = '"';
                    p++;
                } else {
                    in_quotes = false;
                }
            } else {
                buffer[buf_pos++] = c;
            }
        } else {
            if (c == '"') {
                in_quotes = true;
            } else if (c == ',') {
                /* 字段结束 */
                buffer[buf_pos] = '\0';
                row->fields[row->field_count] = strdup(buffer);
                row->field_count++;
                buf_pos = 0;

                if (row->field_count >= CSV_MAX_FIELDS) {
                    return false;
                }
            } else if (c == '\n' || c == '\r') {
                break;
            } else {
                buffer[buf_pos++] = c;
            }
        }
    }

    /* 最后一个字段 */
    if (buf_pos > 0 || row->field_count > 0) {
        buffer[buf_pos] = '\0';
        row->fields[row->field_count] = strdup(buffer);
        row->field_count++;
    }

    return true;
}

/* 快速数字解析 - GCC 15 优化 */
long fast_atol(const char* str) {
    long result = 0;
    bool negative = false;

    /* 跳过空白 */
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    /* 处理符号 */
    if (*str == '-') {
        negative = true;
        str++;
    } else if (*str == '+') {
        str++;
    }

    /* GCC 15 使用 SIMD 优化数字解析 */
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return negative ? -result : result;
}

/* 快速字符串转 double - GCC 15 优化 */
double fast_atod(const char* str) {
    /* 简单实现，实际使用 strtod */
    return strtod(str, NULL);
}
