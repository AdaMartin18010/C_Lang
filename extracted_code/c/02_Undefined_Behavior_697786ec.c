/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\02_Undefined_Behavior.md
 * Line: 65
 * Language: c
 * Block ID: 697786ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 有符号整数溢出是UB
int max = INT_MAX;
int overflow = max + 1;  // UB!

// ✅ 使用无符号类型（回绕是明确定义的）
unsigned int umax = UINT_MAX;
unsigned int wrap = umax + 1;  // 回绕到0，明确定义

// ✅ 检查溢出
bool safe_add(int a, int b, int *result) {
    if (b > 0 && a > INT_MAX - b) return false;
    if (b < 0 && a < INT_MIN - b) return false;
    *result = a + b;
    return true;
}
