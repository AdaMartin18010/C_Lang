/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 2818
 * Language: c
 * Block ID: 1fb1836c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C语言错误处理模式示例 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* ============================================
   模式1: 返回错误码，输出参数返回结果
   ============================================ */

typedef enum {
    ERR_OK = 0,
    ERR_NULL_PTR = -1,
    ERR_INVALID_ARG = -2,
    ERR_OUT_OF_MEMORY = -3,
    ERR_OVERFLOW = -4,
    ERR_NOT_FOUND = -5
} ErrorCode;

/* 返回错误码，通过指针输出结果 */
ErrorCode safe_divide(int a, int b, int* result) {
    if (result == NULL) {
        return ERR_NULL_PTR;
    }
    if (b == 0) {
        return ERR_INVALID_ARG;
    }
    *result = a / b;
    return ERR_OK;
}

/* ============================================
   模式2: 返回NULL表示错误
   ============================================ */

char* create_buffer(size_t size) {
    char* buffer = malloc(size);
    if (buffer == NULL) {
        /* 设置全局错误码 */
        errno = ENOMEM;
        return NULL;
    }
    return buffer;
}

/* ============================================
   模式3: 返回特殊值表示错误
   ============================================ */

double safe_sqrt(double x) {
    if (x < 0) {
        return -1.0;  /* 特殊值表示错误 */
    }
    /* 实际计算... */
    return x;  /* 简化 */
}

/* ============================================
   模式4: 使用全局错误变量
   ============================================ */

static ErrorCode last_error = ERR_OK;
static char error_message[256] = {0};

void set_error(ErrorCode code, const char* msg) {
    last_error = code;
    if (msg != NULL) {
        strncpy(error_message, msg, sizeof(error_message) - 1);
        error_message[sizeof(error_message) - 1] = '\0';
    }
}

ErrorCode get_last_error(char* msg, size_t msg_size) {
    if (msg != NULL && msg_size > 0) {
        strncpy(msg, error_message, msg_size - 1);
        msg[msg_size - 1] = '\0';
    }
    return last_error;
}

/* ============================================
   模式5: goto错误处理
   ============================================ */

int complex_operation(void) {
    int result = -1;
    char* buffer1 = NULL;
    char* buffer2 = NULL;
    FILE* file = NULL;

    buffer1 = malloc(1024);
    if (buffer1 == NULL) {
        set_error(ERR_OUT_OF_MEMORY, "无法分配buffer1");
        goto cleanup;
    }

    buffer2 = malloc(2048);
    if (buffer2 == NULL) {
        set_error(ERR_OUT_OF_MEMORY, "无法分配buffer2");
        goto cleanup;
    }

    file = fopen("data.txt", "r");
    if (file == NULL) {
        set_error(ERR_NOT_FOUND, "无法打开文件");
        goto cleanup;
    }

    /* 执行操作... */
    result = 0;  /* 成功 */

cleanup:
    if (file != NULL) fclose(file);
    free(buffer2);
    free(buffer1);
    return result;
}

/* ============================================
   模式6: 错误跳转（setjmp/longjmp）
   ============================================ */

#include <setjmp.h>

static jmp_buf error_jump;
static ErrorCode jump_error_code;

void throw_error(ErrorCode code) {
    jump_error_code = code;
    longjmp(error_jump, 1);
}

int risky_operation(void) {
    if (setjmp(error_jump) != 0) {
        /* 错误处理 */
        printf("捕获到错误: %d\n", jump_error_code);
        return -1;
    }

    /* 可能抛出错误的操作 */
    char* ptr = malloc(100);
    if (ptr == NULL) {
        throw_error(ERR_OUT_OF_MEMORY);
    }

    free(ptr);
    return 0;
}
