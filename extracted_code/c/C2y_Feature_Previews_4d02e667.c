/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 1149
 * Language: c
 * Block ID: 4d02e667
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// N3631: 宏系统改进 (早期)

// 提案1: 更强大的字符串化
#define STR(x) #x
// 改进: 保留宏展开后的内容

// 提案2: 可变参数改进
#define LOG(fmt, ...) printf(fmt, __VA_ARGS__)
// 问题: LOG("hello") 失败 (没有可变参数)

// C2y可能方案: __VA_OPT__
#define LOG(fmt, ...) printf(fmt __VA_OPT__(,) __VA_ARGS__)

// 提案3: 宏递归扩展控制
// 当前: 宏展开不递归
// 提案: 允许受控递归
