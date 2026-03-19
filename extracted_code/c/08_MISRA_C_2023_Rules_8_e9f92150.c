/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 607
 * Language: c
 * Block ID: e9f92150
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 完整文件操作检查 */
int safe_file_copy(const char *src, const char *dst)
{
    FILE *src_fp = NULL;
    FILE *dst_fp = NULL;
    char buffer[4096];
    size_t bytes_read, bytes_written;
    int result = -1;

    /* 打开源文件 */
    src_fp = fopen(src, "rb");
    if (src_fp == NULL) {
        return -1;
    }

    /* 打开目标文件 */
    dst_fp = fopen(dst, "wb");
    if (dst_fp == NULL) {
        goto cleanup;
    }

    /* 复制内容 */
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_fp)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_read, dst_fp);
        if (bytes_written != bytes_read) {
            goto cleanup;
        }
    }

    /* 检查读取错误 */
    if (ferror(src_fp)) {
        goto cleanup;
    }

    result = 0;

cleanup:
    if (src_fp != NULL) {
        fclose(src_fp);
    }
    if (dst_fp != NULL) {
        fclose(dst_fp);
    }
    return result;
}
