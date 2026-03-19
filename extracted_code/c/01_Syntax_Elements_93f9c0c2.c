/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\01_Syntax_Elements.md
 * Line: 415
 * Language: c
 * Block ID: 93f9c0c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 未定义行为：修改字符串字面量
char *s = "Hello";  // 指向只读数据
s[0] = 'h';  // 崩溃或不可预测行为

// ✅ 可修改的字符数组
char modifiable[] = "Hello";  // 数组拷贝
modifiable[0] = 'h';  // OK

// ✅ 显式const
const char *read_only = "Hello";
// read_only[0] = 'h';  // 编译错误
