/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 1390
 * Language: c
 * Block ID: 3364da6a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C Use-After-Free 示例
void use_after_free_c(void) {
    int* p = malloc(sizeof(int));
    *p = 42;
    free(p);

    // p 现在是悬挂指针，来源已失效
    // int x = *p;  // Use-After-Free！

    // 防御性编程：释放后置空
    p = NULL;
    // 现在 *p 是空指针解引用（更容易检测）
}

// 双重释放
void double_free(void) {
    int* p = malloc(sizeof(int));
    free(p);
    // free(p);  // 双重释放！未定义行为
}

// 使用 ASan 检测
// 编译: clang -fsanitize=address -g test.c
void asan_detection(void) {
    int* p = malloc(40);
    free(p);
    // ASan 会在 *p 访问时报告错误
}
