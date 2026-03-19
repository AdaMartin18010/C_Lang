/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 388
 * Language: c
 * Block ID: 15612810
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int goto_example(int condition) {
    void *resource = acquire_resource();
    defer release_resource(resource);

    if (condition == 1) {
        goto cleanup;  // defer先执行，然后跳转
    }

    // 处理...

cleanup:
    // 其他清理...
    return 0;
}
