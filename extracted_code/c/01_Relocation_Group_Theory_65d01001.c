/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 309
 * Language: c
 * Block ID: 65d01001
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 相同函数的去重（COMDAT）

// 文件1.c
__attribute__((weak)) void common_function(void) {
    // 实现A
}

// 文件2.c
__attribute__((weak)) void common_function(void) {
    // 实现B（相同）
}

// 链接器选择其中一个，丢弃其他
// 节省代码空间
