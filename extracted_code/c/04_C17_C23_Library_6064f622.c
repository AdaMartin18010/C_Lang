/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\04_C17_C23_Library.md
 * Line: 243
 * Language: c
 * Block ID: 6064f622
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 传统方法：可能不够全面或依赖编译器扩展
bool old_safe_add(int a, int b, int *result) {
    if (b > 0 && a > INT_MAX - b) return false;
    if (b < 0 && a < INT_MIN - b) return false;
    *result = a + b;
    return true;
}

// ✅ C23标准方法：全面、可移植、高效
bool new_safe_add(int a, int b, int *result) {
    return !ckd_add(result, a, b);  // 返回true表示成功
}
