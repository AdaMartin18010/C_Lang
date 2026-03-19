/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2625
 * Language: c
 * Block ID: ec9c1477
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ⚠️ 性能敏感代码需要评估
void tight_loop(void) {
    for (int i = 0; i < 1000000; i++) {
        int *p = malloc(100);
        defer free(p);  // 每次迭代都有defer开销

        // 处理...
    }
}

// ✅ 更好的做法
void optimized_loop(void) {
    int *p = malloc(100);
    defer free(p);

    for (int i = 0; i < 1000000; i++) {
        // 重用p，无重复分配
    }
}
