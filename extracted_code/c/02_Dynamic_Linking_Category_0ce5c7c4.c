/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 560
 * Language: c
 * Block ID: 0ce5c7c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：在一个库中分配，在另一个库中释放
// liba.so
char *create_string(void) {
    return malloc(100);  // 使用liba的malloc
}

// main
void use_string(void) {
    char *s = create_string();
    free(s);  // 使用main的free（可能不同堆！）
}

// 解决：提供配对函数
void destroy_string(char *s) {
    free(s);  // 使用同一库的free
}
