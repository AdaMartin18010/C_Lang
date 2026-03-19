/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
 * Line: 296
 * Language: c
 * Block ID: 5684f846
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C: 传统错误处理 */
typedef enum {
    ERR_OK = 0,
    ERR_NULL_PTR = -1,
    ERR_OVERFLOW = -2
} error_t;

error_t divide_checked(int a, int b, int *result) {
    if (!result) return ERR_NULL_PTR;
    if (b == 0) return ERR_OVERFLOW;
    *result = a / b;
    return ERR_OK;
}

/* 使用 - 容易忽略错误检查 */
int result;
error_t err = divide_checked(10, 0, &result);
if (err != ERR_OK) { /* 处理错误 */ }
