/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1972
 * Language: c
 * Block ID: dd5c6720
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 源代码
void example(void) {
    FILE *fp = fopen("test.txt", "r");
    defer fclose(fp);

    // 业务逻辑...
    process(fp);
}

// 编译器可能的转换结果（概念性）
void example_transformed(void) {
    FILE *fp = fopen("test.txt", "r");

    // 使用goto进行统一的退出路径
    if (setjmp(defer_jump_buffer)) goto defer_section;

    // 业务逻辑...
    if (process(fp) != 0) goto defer_section;

defer_section:
    fclose(fp);  // defer的内容在这里执行
}
