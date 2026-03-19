/*
 * Auto-generated from: Zig\matrices\README.md
 * Line: 166
 * Language: c
 * Block ID: 86075761
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C: 手动错误处理
int parse_number(const char *s, int *out) {
    char *end;
    long val = strtol(s, &end, 10);
    if (end == s || *end != '\0' || val > INT_MAX) {
        return -1;
    }
    *out = (int)val;
    return 0;
}

int n;
if (parse_number("42", &n) != 0) { /* 错误处理 */ }
