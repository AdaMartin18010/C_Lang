/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2117
 * Language: c
 * Block ID: 52f09db6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化前
void example(void) {
    int *p = malloc(100);
    defer free(p);

    FILE *fp = fopen("test.txt", "r");
    defer fclose(fp);

    return;
}

// 优化后（概念性）
void example_optimized(void) {
    int *p = malloc(100);
    FILE *fp = fopen("test.txt", "r");

    // 内联展开，无额外开销
    if (fp) fclose(fp);
    if (p) free(p);
}
