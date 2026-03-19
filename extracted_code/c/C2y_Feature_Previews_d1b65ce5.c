/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 671
 * Language: c
 * Block ID: d1b65ce5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// _Countof: 获取数组元素个数

_Countof(array);

// 语义:
// - 仅适用于数组类型(不是指针!)
// - 编译期计算
// - 类型: size_t
// - 错误: 如果参数不是数组，编译错误

// 与sizeof对比
sizeof(array)     // 总字节数
_Countof(array)   // 元素个数
