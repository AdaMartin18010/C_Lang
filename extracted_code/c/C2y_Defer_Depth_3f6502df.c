/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2900
 * Language: c
 * Block ID: 3f6502df
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void debug_not_executing(void) {
    FILE *fp = fopen("test.txt", "r");
    if (!fp) return;

    // ❌ 错误：goto跳过了defer
    if (some_condition()) {
        goto skip;  // defer不会注册！
    }

    defer fclose(fp);

skip:
    printf("Skipped\n");
}
