/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 759
 * Language: c
 * Block ID: 398ef3f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

char *duplicate_and_process_fixed(const char *input) {
    size_t len = strlen(input);
    char *buffer = malloc(len * 2 + 1);
    if (!buffer) return NULL;

    // 处理字符串...
    char *p = buffer;
    for (const char *s = input; *s; s++) {
        *p++ = *s;
        *p++ = '_';
    }
    *p = '\0';

    char *result = strdup(buffer);
    free(buffer);  // 显式释放临时缓冲区
    return result;
}
