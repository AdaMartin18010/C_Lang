/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 47
 * Language: c
 * Block ID: 70e22f04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统C代码 - 没有defer的情况
int process_data(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;

    char *buffer = malloc(1024);
    if (!buffer) {
        fclose(fp);  // 需要手动释放
        return -1;
    }

    void *handle = acquire_resource();
    if (!handle) {
        free(buffer);   // 需要手动释放
        fclose(fp);     // 需要手动释放
        return -1;
    }

    // 处理数据...
    if (some_error_condition()) {
        release_resource(handle);  // 又需要手动释放
        free(buffer);
        fclose(fp);
        return -1;
    }

    // 更多处理...

    // 正常退出也需要释放
    release_resource(handle);
    free(buffer);
    fclose(fp);
    return 0;
}
