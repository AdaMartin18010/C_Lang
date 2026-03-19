/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\README.md
 * Line: 249
 * Language: c
 * Block ID: beedd045
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 强符号 - 多次定义会报错
int global_var = 10;        // 强符号 (已初始化)
void strong_func() { }      // 强符号 (函数定义)

// 弱符号 - 允许重复定义，强符号优先
extern int weak_var __attribute__((weak));
void weak_func() __attribute__((weak));

// 使用场景: 库函数的默认实现
void __attribute__((weak)) platform_init() {
    // 默认空实现
}

// 应用程序可以覆盖
void platform_init() {
    // 自定义硬件初始化
}
