/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\05_Safety_Standards.md
 * Line: 182
 * Language: c
 * Block ID: bef39ba2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 违反规则：多个return语句 (Rule 15.5)
int func(int x) {
    if (x < 0) return -1;
    if (x > 100) return 1;
    return 0;
}

// ✅ 合规代码
int func(int x) {
    int result = 0;
    if (x < 0) {
        result = -1;
    } else if (x > 100) {
        result = 1;
    } else {
        result = 0;
    }
    return result;  // 单一出口
}

// ❌ 违反规则：goto的使用 (Rule 15.1)
if (error) goto cleanup;

// ✅ 合规代码：使用结构化控制流
if (!error) {
    // 正常处理
}
// 清理代码

// ❌ 违反规则：递归 (Rule 17.2)
int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// ✅ 合规代码：迭代实现
int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
