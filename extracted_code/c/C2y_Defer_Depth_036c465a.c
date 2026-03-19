/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2739
 * Language: c
 * Block ID: 036c465a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ⚠️ 热点代码
void hot_function(void) {
    for (int i = 0; i < 1000000; i++) {
        int *p = malloc(8);
        defer free(p);  // 百万次defer

        // 简单操作
    }
}

// ✅ 优化后
void optimized_hot_function(void) {
    int *pool = malloc(8 * 1000000);
    defer free(pool);

    for (int i = 0; i < 1000000; i++) {
        int *p = &pool[i * 8];
        // 简单操作
    }
}
