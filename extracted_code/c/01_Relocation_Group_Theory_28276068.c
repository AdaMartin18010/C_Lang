/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 440
 * Language: c
 * Block ID: 28276068
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：32位重定位超出范围
// 在大型程序中，PC相对跳转可能超出2GB范围

void far_away_function(void);  // 距离超过2GB

void caller(void) {
    far_away_function();  // 可能产生重定位溢出
}

// 解决：使用PLT（过程链接表）
// 或使用 -mcmodel=large 编译选项
