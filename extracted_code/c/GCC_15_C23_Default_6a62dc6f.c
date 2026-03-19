/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1360
 * Language: c
 * Block ID: 6a62dc6f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* strdup 和 strndup 示例 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    const char* original = "Hello, C23 World!";

    /* strdup: 完整复制字符串 */
    char* copy1 = strdup(original);
    if (copy1) {
        printf("strdup: %s\n", copy1);
        free(copy1);
    }

    /* strndup: 最多复制 n 个字符 */
    char* copy2 = strndup(original, 5);
    if (copy2) {
        printf("strndup(5): %s\n", copy2);  /* "Hello" */
        free(copy2);
    }

    /* strndup 处理源字符串短于 n 的情况 */
    char* copy3 = strndup("Hi", 100);
    if (copy3) {
        printf("strndup(100) of 'Hi': %s (len=%zu)\n",
               copy3, strlen(copy3));  /* "Hi", len=2 */
        free(copy3);
    }

    return 0;
}
