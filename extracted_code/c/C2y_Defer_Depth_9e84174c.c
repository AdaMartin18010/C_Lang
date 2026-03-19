/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 130
 * Language: c
 * Block ID: 9e84174c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用C2y defer的代码
int process_data_c2y(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    defer fclose(fp);  // 获取资源后立即声明释放

    char *buffer = malloc(1024);
    if (!buffer) return -1;  // fp会自动关闭！
    defer free(buffer);

    void *handle = acquire_resource();
    if (!handle) return -1;  // buffer和fp都会自动释放！
    defer release_resource(handle);

    // 处理数据...
    if (some_error_condition()) {
        return -1;  // 所有资源自动释放！
    }

    // 更多处理...
    return 0;  // 所有资源自动释放！
}
