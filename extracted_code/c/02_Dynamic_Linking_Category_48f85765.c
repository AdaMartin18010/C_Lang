/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 525
 * Language: c
 * Block ID: 48f85765
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：默认导出所有符号
// 导致：符号冲突、加载变慢
void internal_helper(void) { }  // 应该隐藏

// 正确：显式控制可见性
__attribute__((visibility("hidden")))
void internal_helper(void) { }  // 仅库内可见

__attribute__((visibility("default")))
void public_api(void) { }  // 明确导出

// 或使用版本脚本控制
