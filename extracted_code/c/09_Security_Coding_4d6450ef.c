/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 15
 * Language: c
 * Block ID: 4d6450ef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    char *data;
    size_t len;      // 当前长度
    size_t capacity; // 总容量
} SafeString;

SafeString* safestr_create(size_t initial_capacity);
void safestr_destroy(SafeString *s);

// 安全的字符串操作
bool safestr_append(SafeString *s, const char *src);
bool safestr_append_n(SafeString *s, const char *src, size_t n);
bool safestr_printf(SafeString *s, const char *fmt, ...);

// 安全的字符串输入
bool safestr_readline(SafeString *s, FILE *stream);

// 防止格式字符串漏洞的包装
bool safestr_format(SafeString *s, const char *fmt, ...);
