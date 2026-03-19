/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 260
 * Language: c
 * Block ID: 5174af43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用 goto 进行统一清理
do_work(void) {
    FILE* f = NULL;
    char* buffer = NULL;
    error_t err = ERR_OK;

    f = fopen("file.txt", "r");
    if (!f) {
        err = ERR_FILE_NOT_FOUND;
        goto cleanup;
    }

    buffer = malloc(1024);
    if (!buffer) {
        err = ERR_OUT_OF_MEMORY;
        goto cleanup;
    }

    // 更多操作...

cleanup:
    free(buffer);
    if (f) fclose(f);
    return err;
}
