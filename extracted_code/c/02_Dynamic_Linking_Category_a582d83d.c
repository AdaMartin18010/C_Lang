/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 542
 * Language: c
 * Block ID: a582d83d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：跨库的全局变量初始化顺序不确定
// liba.so
int *global_a = &global_b;  // 依赖libb.so的global_b

// libb.so
int global_b = 42;

// 解决：延迟初始化
int *get_global_a(void) {
    static int *a = NULL;
    if (!a) a = get_global_b();  // 运行时初始化
    return a;
}
