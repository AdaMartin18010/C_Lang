/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CERT_C_2024\README.md
 * Line: 246
 * Language: c
 * Block ID: acb6496c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码 - 有符号溢出 (UB)
int multiply(int a, int b) {
    return a * b;  // 未定义行为如果溢出
}

// 合规代码
#include <limits.h>

bool multiply_safe(int a, int b, int* result) {
    if (a > 0) {
        if (b > 0) {
            if (a > (INT_MAX / b)) return false;
        } else {
            if (b < (INT_MIN / a)) return false;
        }
    } else {
        if (b > 0) {
            if (a < (INT_MIN / b)) return false;
        } else {
            if (a != 0 && b < (INT_MAX / a)) return false;
        }
    }
    *result = a * b;
    return true;
}
