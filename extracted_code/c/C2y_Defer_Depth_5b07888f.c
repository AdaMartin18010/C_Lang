/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 355
 * Language: c
 * Block ID: 5b07888f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int normal_return_example(void) {
    int *data = malloc(100);
    defer free(data);

    // 业务逻辑...
    if (some_condition()) {
        return -1;  // defer在这里执行
    }

    // 更多处理...
    return 0;  // defer在这里执行
}
