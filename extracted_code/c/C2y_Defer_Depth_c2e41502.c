/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 921
 * Language: c
 * Block ID: c2e41502
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int process_with_error_handling(const char *input) {
    FILE *fp = fopen(input, "r");
    if (!fp) return -1;
    defer fclose(fp);

    char *buffer = malloc(1024);
    if (!buffer) return -2;
    defer free(buffer);

    // 读取和处理...
    if (fread(buffer, 1, 1024, fp) == 0) {
        if (ferror(fp)) {
            return -3;  // 读取错误，资源自动释放
        }
    }

    // 处理数据...
    int result = process_data(buffer);
    if (result < 0) {
        return result - 10;  // 传播错误，资源自动释放
    }

    return 0;  // 成功，资源自动释放
}
