/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 1972
 * Language: c
 * Block ID: 5ac9fd96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: DSL 构建非常有限

// 可以通过宏创建小型 DSL
#define HTML(tag, content) "<" #tag ">" content "</" #tag ">"
#define P(text) HTML(p, text)
#define H1(text) HTML(h1, text)

const char* html = H1("Hello") P("World");
// 展开为: "<h1>" "Hello" "</h1>" "<p>" "World" "</p>"

// ❌ 无法在编译时解析字符串
// ❌ 无法验证 DSL 语法
// ❌ 无法根据 DSL 生成类型安全代码
