/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 373
 * Language: c
 * Block ID: 12d05fb0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 向前跳转（资源清理）*/
int process_file(const char *path)
{
    FILE *fp = NULL;
    char *buffer = NULL;
    int result = -1;

    fp = fopen(path, "r");
    if (fp == NULL) {
        goto cleanup;  /* 向前跳转 */
    }

    buffer = malloc(BUF_SIZE);
    if (buffer == NULL) {
        goto cleanup;
    }

    /* 处理文件 */

    result = 0;

cleanup:
    free(buffer);
    if (fp != NULL) {
        fclose(fp);
    }
    return result;
}

/* ❌ 严重违反 - 向后跳转 */
retry:
    if (try_operation() != OK) {
        goto retry;  /* 向后跳转 - 像goto一样！ */
    }
