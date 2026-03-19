/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 1347
 * Language: c
 * Block ID: 6ad97d35
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 传统方法：手动溢出检查（容易出错）
bool old_safe_add(int a, int b, int *result) {
    if (b > 0 && a > INT_MAX - b) return false;  // 正溢出
    if (b < 0 && a < INT_MIN - b) return false;  // 负溢出
    *result = a + b;
    return true;
}

// ✅ C23标准方法：简洁、可靠、可能更高效
bool new_safe_add(int a, int b, int *result) {
    return !ckd_add(result, a, b);
}

// ❌ 传统方法：无符号乘法检查（复杂）
bool old_safe_mul_size(size_t a, size_t b, size_t *result) {
    if (a != 0 && b > SIZE_MAX / a) return false;
    *result = a * b;
    return true;
}

// ✅ C23标准方法
bool new_safe_mul_size(size_t a, size_t b, size_t *result) {
    return !ckd_mul(result, a, b);
}

// 性能对比说明：
// - 现代编译器可将ckd_*函数优化为使用CPU溢出标志
// - x86: add/seto 或 add/jo 序列
// - ARM: adds/cset 序列
// - 无需复杂的分支预测
