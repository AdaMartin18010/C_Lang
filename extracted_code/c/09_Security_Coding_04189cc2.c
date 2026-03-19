/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 102
 * Language: c
 * Block ID: 04189cc2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    VAL_OK = 0,
    VAL_NULL,
    VAL_EMPTY,
    VAL_TOO_LONG,
    VAL_INVALID_CHAR,
    VAL_FORMAT_ERROR,
    VAL_OUT_OF_RANGE
} ValidationResult;

// 整数验证
ValidationResult validate_int(const char *input, int64_t min,
                              int64_t max, int64_t *output);
ValidationResult validate_uint(const char *input, uint64_t min,
                               uint64_t max, uint64_t *output);

// 浮点数验证
ValidationResult validate_double(const char *input, double min,
                                 double max, double *output);

// 字符串验证
typedef struct {
    size_t min_len;
    size_t max_len;
    const char *allowed_chars;  // NULL=全部允许
    const char *forbidden_chars;
} StringValidationRules;

ValidationResult validate_string(const char *input,
                                  const StringValidationRules *rules);

// 路径验证（防止目录遍历）
ValidationResult validate_path(const char *input,
                               const char *allowed_base_dir);

// 正则验证
ValidationResult validate_regex(const char *input, const char *pattern);
