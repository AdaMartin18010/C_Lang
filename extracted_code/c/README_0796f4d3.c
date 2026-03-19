/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\README.md
 * Line: 164
 * Language: c
 * Block ID: 0796f4d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分支友好的代码布局
void branch_friendly(int n) {
    // 良好: 预测循环继续 (高概率)
    for (int i = 0; i < n; i++) {
        // 工作代码
    }

    // 良好: 预测进入 (错误处理罕见)
    if (ptr == NULL) {
        handle_error();  // 罕见路径
        return;
    }
    use_ptr(ptr);        // 常见路径连续执行
}

// 使用条件移动避免分支
int branchless_max(int a, int b) {
    // 有分支版本 (可能预测失败)
    // return (a > b) ? a : b;

    // 无分支版本
    int diff = a - b;
    int mask = diff >> 31;  // 负数时全1
    return b + (diff & ~mask);
}
