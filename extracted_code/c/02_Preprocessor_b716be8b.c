/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\02_Preprocessor.md
 * Line: 243
 * Language: c
 * Block ID: b716be8b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define FOO BAR
#define BAR 42
// FOO -> BAR -> 42

// 间接展开问题
define CAT(a, b) a ## b
define XCAT(a, b) CAT(a, b)
// XCAT(FOO, 1) -> CAT(FOO, 1) -> FOO1
// 需要两次扫描才能完全展开

// 使用 _Generic 替代复杂宏（C11）
#define ABS(x) _Generic((x), \
    int: abs, \
    long: labs, \
    long long: llabs, \
    double: fabs \
)(x)
