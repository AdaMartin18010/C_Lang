/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 3447
 * Language: c
 * Block ID: 088b8ee6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void conditional_cleanup(void) {
    void *resource = acquire();
    bool should_release = true;

    defer {
        if (should_release) {
            release(resource);
        }
    };

    // 处理...

    if (transfer_ownership(resource)) {
        should_release = false;  // 跳过释放
    }
}
