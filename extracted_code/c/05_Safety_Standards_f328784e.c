/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\05_Safety_Standards.md
 * Line: 126
 * Language: c
 * Block ID: f328784e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 违反规则：整数溢出 (Rule 17.8)
int x = INT_MAX;
x = x + 1;  // 未定义行为！

// ✅ 合规代码
if (x <= INT_MAX - 1) {
    x = x + 1;
} else {
    // 处理溢出
}

// ❌ 违反规则：数组越界 (Rule 18.1)
int arr[10];
int x = arr[10];  // 越界访问！

// ✅ 合规代码
for (size_t i = 0; i < 10; i++) {  // 注意边界
    // 安全访问 arr[i]
}
