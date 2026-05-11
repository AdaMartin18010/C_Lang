/*
 * 自定义错误码系统示例
 * 编译: gcc -Wall -Wextra -o error_code_system error_code_system.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ERR_OK = 0,
    ERR_NULL_PTR = -1,
    ERR_INVALID_ARG = -2,
    ERR_OOM = -3,
    ERR_IO = -4,
    ERR_NOT_FOUND = -5,
    ERR_OVERFLOW = -6,
} ErrorCode;

const char *error_string(ErrorCode code) {
    switch (code) {
        case ERR_OK:           return "success";
        case ERR_NULL_PTR:     return "null pointer";
        case ERR_INVALID_ARG:  return "invalid argument";
        case ERR_OOM:          return "out of memory";
        case ERR_IO:           return "I/O error";
        case ERR_NOT_FOUND:    return "not found";
        case ERR_OVERFLOW:     return "overflow";
        default:               return "unknown error";
    }
}

/* 安全的整数加法 */
ErrorCode safe_add(int a, int b, int *result) {
    if (!result) return ERR_NULL_PTR;
    if (b > 0 && a > INT_MAX - b) return ERR_OVERFLOW;
    if (b < 0 && a < INT_MIN - b) return ERR_OVERFLOW;
    *result = a + b;
    return ERR_OK;
}

/* 安全的数组访问 */
ErrorCode safe_get(const int *arr, size_t len, size_t idx, int *out) {
    if (!arr || !out) return ERR_NULL_PTR;
    if (idx >= len) return ERR_INVALID_ARG;
    *out = arr[idx];
    return ERR_OK;
}

int main(void) {
    int result;
    ErrorCode err;

    /* 测试安全加法 */
    err = safe_add(100, 200, &result);
    if (err != ERR_OK) {
        fprintf(stderr, "add failed: %s\n", error_string(err));
        return 1;
    }
    printf("100 + 200 = %d\n", result);

    /* 测试溢出检测 */
    err = safe_add(INT_MAX, 1, &result);
    printf("INT_MAX + 1: %s\n", error_string(err));

    /* 测试数组访问 */
    int nums[] = {10, 20, 30};
    int val;
    err = safe_get(nums, 3, 5, &val);
    printf("get index 5 from 3-element array: %s\n", error_string(err));

    return 0;
}
