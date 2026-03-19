/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_New_Proposals_2026.md
 * Line: 220
 * Language: c
 * Block ID: 154f3144
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 问题：闭包生命周期管理
int* create_multiplier(int factor) {
    auto fn = `[factor](int x) { return x * factor; };`
    return fn;  // 错误：factor在栈上，函数返回后失效
}

// 可能的解决方案：堆分配闭包
int* create_multiplier_heap(int factor) {
    auto fn = heap_closure `[factor](int x) { return x * factor; };`
    return fn;  // 需要手动释放
}
