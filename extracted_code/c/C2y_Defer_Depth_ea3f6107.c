/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 3470
 * Language: c
 * Block ID: ea3f6107
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int example(int *result) {
    int local_result = 0;

    defer {
        if (result) *result = local_result;
        cleanup();
    };

    // 处理...
    local_result = 42;
    return local_result;
}
