/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\19_MISRA_C_2023_Rules_19.md
 * Line: 98
 * Language: c
 * Block ID: 0998d16e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 完全括号化 */
#define SQUARE(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))

/* ✅ 合规 - 多语句宏用do-while(0) */
#define SWAP(type, a, b) do { \
    type temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)

/* 使用 */
if (condition) {
    SWAP(int, x, y);  /* 正确 */
}

/* ✅ 合规 - 避免副作用的参数 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* 安全使用 */
int x_val = x++;
int y_val = y++;
int m = MAX(x_val, y_val);

/* ✅ 合规 - 常量定义 */
#define BUFFER_SIZE 1024
#define MAX_RETRIES 3
#define VERSION_STRING "1.0.0"

/* ✅ 合规 - 条件编译宏 */
#define FEATURE_ENABLED 1
#if FEATURE_ENABLED
    /* ... */
#endif

/* ✅ 合规 - 头文件保护 */
#ifndef MODULE_H
#define MODULE_H
/* 内容 */
#endif /* MODULE_H */
