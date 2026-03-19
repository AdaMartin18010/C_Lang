/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 86
 * Language: c
 * Block ID: f4c1d14b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 符号类型示例
// 文件: main.c

extern int shared_var;      // 未定义引用（外部符号）
static int local_static;     // 局部符号（内部链接）
int global_var = 42;         // 已定义符号（全局可见）

void external_function(void);  // 函数引用

void local_function(void) {    // 函数定义
    shared_var = 10;           // 产生重定位条目
    external_function();       // 产生重定位条目
}
