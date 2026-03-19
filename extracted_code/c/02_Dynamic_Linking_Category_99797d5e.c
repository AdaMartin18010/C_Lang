/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 423
 * Language: c
 * Block ID: 99797d5e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// GNU符号版本控制

// 版本脚本（map文件）
/* libexample.map
EXAMPLE_1.0 {
    global:
        function_v1;
    local:
        *;
};

EXAMPLE_2.0 {
    global:
        function_v2;
} EXAMPLE_1.0;
*/

// C代码中使用默认版本
__asm__(".symver function_v1_default,function_v1@@EXAMPLE_1.0");
__asm__(".symver function_v2_default,function_v2@@EXAMPLE_2.0");

// 旧版本实现
void function_v1_default(void) {
    // v1实现
}

// 新版本实现
void function_v2_default(void) {
    // v2实现
}

// 显式使用特定版本
extern void function_v1(void) __attribute__((weak));
__asm__(".symver function_v1, function@EXAMPLE_1.0");
