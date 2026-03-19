/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\02_C99_Library.md
 * Line: 204
 * Language: c
 * Block ID: c3322525
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ VLA不能用于结构体
struct Bad {
    int n;
    int arr[n];  // 错误！
};

// ✅ VLA只能用于局部数组
void func(int n) {
    int vla[n];  // OK
}

// ❌ VLA生命周期问题（避免过大）
void risky(int n) {
    int huge[n];  // 栈溢出风险！
}
