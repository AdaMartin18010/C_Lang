/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\02_Code_Quality.md
 * Line: 172
 * Language: c
 * Block ID: fdfc8f28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误码定义
typedef enum {
    ERR_OK = 0,
    ERR_NULL_PTR,
    ERR_INVALID_ARG,
    ERR_OUT_OF_MEMORY,
    ERR_IO,
    ERR_OVERFLOW,
    // ...
} ErrorCode;

// 结果类型（类似Rust的Result）
typedef struct {
    ErrorCode code;
    const char *file;
    int line;
    const char *func;
} Result;

#define OK() ((Result){ERR_OK, __FILE__, __LINE__, __func__})
#define ERROR(c) ((Result){(c), __FILE__, __LINE__, __func__})

// 宏链式检查
define TRY(expr) do { \
    Result _r = (expr); \
    if (_r.code != ERR_OK) return _r; \
} while(0)

// 使用
Result read_config(const char *path, Config *out) {
    TRY(open_file(path));      // 失败自动返回
    TRY(parse_header(out));    // 失败自动返回
    TRY(validate_config(out)); // 失败自动返回
    return OK();
}
