/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\03_Axiomatic_Semantics_Hoare.md
 * Line: 205
 * Language: c
 * Block ID: b69d380f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

[n >= 0]
while (n > 0) {
    // 变式: n
    // n > 0 保证循环条件满足
    // n-- 使 n 递减
    f = f * n;
    n = n - 1;
}
[n == 0]
