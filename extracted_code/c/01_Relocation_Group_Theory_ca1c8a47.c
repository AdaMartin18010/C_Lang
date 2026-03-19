/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 328
 * Language: c
 * Block ID: ca1c8a47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 热/冷代码分离

// 热代码（频繁执行）
__attribute__((hot))
void hot_function(void) {
    // 放在靠近.text开始的位置
    // 更好的缓存局部性
}

// 冷代码（罕见执行）
__attribute__((cold))
void cold_function(void) {
    // 放在.text.unlikely段
    // 减少缓存污染
}

// 链接脚本控制布局
/*
SECTIONS {
    .text : {
        *(.text.hot)
        *(.text)
        *(.text.unlikely)
    }
}
*/
