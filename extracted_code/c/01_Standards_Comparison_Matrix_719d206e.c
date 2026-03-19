/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 3005
 * Language: c
 * Block ID: 719d206e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ========================================
 * C89 版本
 * ======================================== */
#ifdef USE_C89

#include <stdlib.h>
#include <string.h>

/* 需要预先知道最大分割数或多次遍历 */
int string_split_c89(const char* str, char delimiter,
                     char** result, int max_splits) {
    const char* p = str;
    int count = 0;
    char* temp;

    while (*p && count < max_splits) {
        const char* end = p;
        while (*end && *end != delimiter) end++;

        int len = end - p;
        result[count] = malloc(len + 1);
        if (result[count] == NULL) {
            /* 错误处理：释放已分配的内存 */
            int i;
            for (i = 0; i < count; i++) {
                free(result[i]);
            }
            return -1;
        }

        strncpy(result[count], p, len);
        result[count][len] = '\0';
        count++;

        if (*end) p = end + 1;
        else break;
    }

    return count;
}

/* ========================================
 * C99 版本：复合字面量和VLA
 * ======================================== */
#elif defined(USE_C99)

#include <string.h>

typedef struct {
    char* str;
    int len;
} Token;

int string_split_c99(const char* str, char delimiter, Token* tokens, int max_tokens) {
    const char* p = str;
    int count = 0;

    while (*p && count < max_tokens) {
        const char* end = p;
        while (*end && *end != delimiter) end++;

        int len = end - p;
        /* 使用VLA临时存储 */
        char temp[len + 1];
        memcpy(temp, p, len);
        temp[len] = '\0';

        tokens[count] = (Token){  /* 复合字面量 */
            .str = strdup(temp),
            .len = len
        };
        count++;

        p = (*end) ? end + 1 : end;
    }

    return count;
}

/* ========================================
 * C11 版本：匿名结构体
 * ======================================== */
#elif defined(USE_C11)

typedef struct {
    union {
        struct {
            char* data;
            size_t length;
        };
        struct {
            char* str;
            size_t len;
        };
    };
} StringSlice;

/* ========================================
 * C23 版本：auto和nullptr
 * ======================================== */
#elif defined(USE_C23)

auto string_split_c23(const char* str, char delimiter, auto** result) {
    auto count = 0;
    auto capacity = 10;
    *result = malloc(capacity * sizeof(char*));

    if (*result == nullptr) return 0;

    const char* p = str;
    while (*p) {
        const char* end = p;
        while (*end && *end != delimiter) end++;

        auto len = end - p;
        (*result)[count] = malloc(len + 1);
        memcpy((*result)[count], p, len);
        (*result)[count][len] = '\0';
        count++;

        if (count >= capacity) {
            capacity *= 2;
            *result = realloc(*result, capacity * sizeof(char*));
        }

        p = (*end) ? end + 1 : end;
    }

    return count;
}

#endif
