/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 340
 * Language: c
 * Block ID: 28f04594
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：乘法溢出
void *alloc_buffer(int width, int height) {
    // 如果 width * height 溢出，分配的内存会小于预期
    return malloc(width * height * sizeof(int));
}

// ❌ 错误示例：有符号整数溢出
int calculate_offset(int base, int increment) {
    return base + increment;  // 可能溢出为负数
}

// ✅ 正确示例：溢出检查
#include <stdint.h>
#include <limits.h>

void *alloc_buffer_safe(int width, int height) {
    if (width <= 0 || height <= 0) return NULL;

    // 检查乘法溢出
    if (width > INT_MAX / height / sizeof(int)) {
        return NULL;  // 会溢出
    }

    size_t total = (size_t)width * height * sizeof(int);
    return malloc(total);
}

// ✅ 正确示例：使用安全整数运算辅助函数
static inline int safe_add(int a, int b, int *result) {
    if (a > 0 && b > INT_MAX - a) return -1;  // 正溢出
    if (a < 0 && b < INT_MIN - a) return -1;  // 负溢出
    *result = a + b;
    return 0;
}

static inline int safe_multiply(int a, int b, int *result) {
    if (a > 0) {
        if (b > 0) {
            if (a > INT_MAX / b) return -1;
        } else {
            if (b < INT_MIN / a) return -1;
        }
    } else {
        if (b > 0) {
            if (a < INT_MIN / b) return -1;
        } else {
            if (a != 0 && b < INT_MAX / a) return -1;
        }
    }
    *result = a * b;
    return 0;
}
