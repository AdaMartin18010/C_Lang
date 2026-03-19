/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 431
 * Language: c
 * Block ID: 3776f069
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 严重错误：依赖未定义行为
void ub_example(int *ptr) {
    int x = *ptr;  // 如果ptr为NULL或无效，UB
    // 编译器可能假设ptr有效，删除null检查
    if (ptr == NULL) return;  // 可能被优化掉！
    use(x);
}

// 正确：在使用前验证
void safe_example(int *ptr) {
    if (ptr == NULL) return;  // 先检查
    int x = *ptr;  // 现在安全
    use(x);
}
