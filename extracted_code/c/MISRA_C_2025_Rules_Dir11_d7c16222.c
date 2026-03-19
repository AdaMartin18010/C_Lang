/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 878
 * Language: c
 * Block ID: d7c16222
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 递归使用限制 */

/* ❌ 违反 [待官方确认] - 无限递归 */
int bad_recursive(int n) {
    return bad_recursive(n);  /* 无限递归 */
}

/* ⚠️ 谨慎 [待官方确认] - 递归深度不确定 */
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  /* 嵌入式中谨慎使用 */
}

/* ✅ 推荐 [待官方确认] - 迭代替代递归 */
int factorial_iterative(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
