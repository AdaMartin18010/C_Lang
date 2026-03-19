/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 324
 * Language: c
 * Block ID: a0de763a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void example_with_block(void) {
    FILE *input = fopen("input.txt", "r");
    if (!input) return;

    FILE *output = fopen("output.txt", "w");
    if (!output) {
        fclose(input);
        return;
    }

    defer {
        fclose(input);
        fclose(output);
        log_operation("files_closed");
    }

    // 处理文件...
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), input)) {
        fputs(buffer, output);
    }
}  // defer块在这里执行
