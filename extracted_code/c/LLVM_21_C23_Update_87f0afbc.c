/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_21_C23_Update.md
 * Line: 43
 * Language: c
 * Block ID: 87f0afbc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化器现在可以更好地区分:
void foo(int *p) {
    // 只读取地址(不捕获来源) - 允许更多优化
    printf("%p", (void*)p);

    // 实际解引用 - 捕获来源
    *p = 42;
}
