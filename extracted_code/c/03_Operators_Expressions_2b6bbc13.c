/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 506
 * Language: c
 * Block ID: 2b6bbc13
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：赋值而非比较
if (x = 5) {  // x被赋值为5，表达式值为5（真）
    // 总是执行
}

// ✅ 正确
if (x == 5) {
    // 相等比较
}

// ✅ 防御性写法（Yoda条件）
if (5 == x) {  // 如果写成 = 会编译错误
    // ...
}
