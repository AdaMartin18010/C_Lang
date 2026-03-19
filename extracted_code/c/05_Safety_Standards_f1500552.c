/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\05_Safety_Standards.md
 * Line: 234
 * Language: c
 * Block ID: f1500552
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 违反规则：空指针解引用 (Rule 9.1, 17.8)
int *p = NULL;
*p = 42;  // 崩溃！

// ✅ 合规代码
int *p = malloc(sizeof(int));
if (p != NULL) {  // 检查 (Rule 11.8)
    *p = 42;
    free(p);
    p = NULL;  // 释放后置空 (良好实践)
}

// ❌ 违反规则：不安全的指针算术 (Rule 18.4)
int *p = arr;
p += 10;  // 可能越界

// ✅ 合规代码：确保在有效范围内
if ((p >= arr) && (p < arr + arr_size)) {
    // 安全使用
}
