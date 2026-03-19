/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Proposal_Tracker.md
 * Line: 169
 * Language: c
 * Block ID: daf04567
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 提案语法 (N3742)
outer: for (int i = 0; i < N; i++) {
    inner: for (int j = 0; j < M; j++) {
        if (condition)
            break outer;  // 跳出外层循环
    }
}

// 编译器支持 (2026年3月)
// Clang 22+: -fexperimental-named-loops
// GCC: 预计16+支持

// 争议点解决进展:
// 1. ✅ 标签作用域规则 - 已确定: 循环标签独立于goto标签命名空间
// 2. ✅ 与goto标签的命名空间冲突 - 已解决: 使用不同命名空间
// 3. 🔄 语法风格选择 - 倾向于冒号前置语法

// 2026年3月会议决定:
// - 推进至正式提案阶段
// - 预计2026年9月投票
