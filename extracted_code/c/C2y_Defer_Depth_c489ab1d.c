/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2919
 * Language: c
 * Block ID: c489ab1d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void debug_wrong_order(void) {
    void *a = acquire_a();
    defer release_a(a);

    void *b = acquire_b();
    defer release_b(b);

    // 如果需要先释放b再释放a，确保注册顺序正确
    // defer按LIFO顺序执行
}
