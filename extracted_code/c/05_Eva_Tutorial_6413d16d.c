/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 616
 * Language: c
 * Block ID: 6413d16d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial7_range.c

// 数值约束传播示例
void clamp_values(int* values, int n, int min_val, int max_val) {
    // Eva推断约束:
    // values[i] 将在 [min_val, max_val] 范围内
    for (int i = 0; i < n; i++) {
        if (values[i] < min_val) {
            values[i] = min_val;
        } else if (values[i] > max_val) {
            values[i] = max_val;
        }
    }
    // 后置条件: 所有值都在范围内
}

// 范围检查函数
int is_in_range(int x, int low, int high) {
    // Eva推断:
    // 如果返回1,则 x ∈ [low, high]
    return (x >= low) && (x <= high);
}
