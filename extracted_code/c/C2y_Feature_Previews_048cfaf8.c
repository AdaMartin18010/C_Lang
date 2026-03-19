/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 256
 * Language: c
 * Block ID: 048cfaf8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Elvis操作符: a ?: b
// 语义: 如果a"真值"(非零/非空)，则返回a，否则返回b

// 基础用法
int result = value ?: default_value;
// 等价于: (value != 0 ? value : default_value)

// 与三元运算符对比
type    ternary     elvis
────────────────────────────────
a ? b : c   vs   a ?: b
// 三元: 如果a为真返回b，否则c
// Elvis: 如果a为真返回a，否则b
