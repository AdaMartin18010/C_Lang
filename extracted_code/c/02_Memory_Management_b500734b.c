/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1601
 * Language: c
 * Block ID: b500734b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 异常路径导致泄漏
void process_file_bad(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;

    char *buffer = malloc(4096);
    if (!buffer) return;  // ❌ 泄漏 fp！

    char *result = malloc(1024);
    if (!result) return;  // ❌ 泄漏 fp 和 buffer！

    // 处理中出错返回
    if (process_data(buffer, result) < 0) {
        return;  // ❌ 泄漏所有资源！
    }

    free(result);
    free(buffer);
    fclose(fp);
}

// ✅ 正确: 统一清理点（goto模式）
void process_file_good(const char *filename) {
    FILE *fp = NULL;
    char *buffer = NULL;
    char *result = NULL;
    int status = -1;

    fp = fopen(filename, "r");
    if (!fp) goto cleanup;

    buffer = malloc(4096);
    if (!buffer) goto cleanup;

    result = malloc(1024);
    if (!result) goto cleanup;

    if (process_data(buffer, result) < 0) goto cleanup;

    status = 0;  // 成功

cleanup:
    free(result);
    free(buffer);
    if (fp) fclose(fp);
    return status;
}
