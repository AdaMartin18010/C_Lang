/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\Formal_Verification\README.md
 * Line: 100
 * Language: c
 * Block ID: a46649f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 待验证函数
int safe_add(int a, int b, int* result) {
    if (a > 0 && b > INT_MAX - a) return 0;  // 正溢出检查
    if (a < 0 && b < INT_MIN - a) return 0;  // 负溢出检查
    *result = a + b;
    return 1;
}
