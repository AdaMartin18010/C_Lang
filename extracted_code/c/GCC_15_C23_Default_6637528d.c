/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1251
 * Language: c
 * Block ID: 6637528d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 安全密码处理 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PASSWORD_MAX_LEN 128

typedef struct {
    char data[PASSWORD_MAX_LEN];
    size_t len;
} secure_string_t;

/* 安全初始化 */
void secure_string_init(secure_string_t* str) {
    memset_explicit(str->data, 0, sizeof(str->data));
    str->len = 0;
}

/* 安全设置 */
void secure_string_set(secure_string_t* str, const char* source) {
    /* 先清除旧数据 */
    secure_string_init(str);

    /* 复制新数据 */
    size_t len = strlen(source);
    if (len >= PASSWORD_MAX_LEN) {
        len = PASSWORD_MAX_LEN - 1;
    }
    memcpy(str->data, source, len);
    str->len = len;
}

/* 安全清除 */
void secure_string_clear(secure_string_t* str) {
    /* C23: 使用 memset_explicit 确保安全清除 */
    memset_explicit(str->data, 0, sizeof(str->data));
    str->len = 0;
}

/* 验证密码 (示例) */
bool secure_string_verify(secure_string_t* input, const char* expected) {
    if (input->len != strlen(expected)) {
        return false;
    }
    return memcmp(input->data, expected, input->len) == 0;
}

/* 使用示例 */
int main(void) {
    secure_string_t password;
    secure_string_t input;

    /* 设置密码 */
    secure_string_set(&password, "MySecret123!");

    /* 获取用户输入 (模拟) */
    printf("Enter password: ");
    char input_buf[PASSWORD_MAX_LEN];
    if (fgets(input_buf, sizeof(input_buf), stdin)) {
        /* 去除换行符 */
        size_t len = strlen(input_buf);
        if (len > 0 && input_buf[len-1] == '\n') {
            input_buf[len-1] = '\0';
        }

        secure_string_set(&input, input_buf);

        /* 清除输入缓冲区 */
        memset_explicit(input_buf, 0, sizeof(input_buf));
    }

    /* 验证 */
    if (secure_string_verify(&input, "MySecret123!")) {
        printf("Access granted\n");
    } else {
        printf("Access denied\n");
    }

    /* 安全清除所有敏感数据 */
    secure_string_clear(&password);
    secure_string_clear(&input);

    return 0;
}
