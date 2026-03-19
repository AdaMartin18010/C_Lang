/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2417
 * Language: c
 * Block ID: f5b99e21
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 危险代码：gets() 可能导致缓冲区溢出 */
void dangerous(void) {
    char buffer[100];
    gets(buffer);  /* 永远不要使用！ */
    printf("You entered: %s\n", buffer);
}

/* 安全替代：使用 fgets() */
#include <stdio.h>
#include <string.h>

void safe_alternative(void) {
    char buffer[100];

    if (fgets(buffer, sizeof(buffer), stdin) != nullptr) {
        /* 移除可能的换行符 */
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        printf("You entered: %s\n", buffer);
    }
}

/* C11 Annex K（可选支持） */
#ifdef __STDC_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

void annex_k_version(void) {
    char buffer[100];
    gets_s(buffer, sizeof(buffer));  /* 比gets安全，但仍不推荐使用 */
}
#endif
