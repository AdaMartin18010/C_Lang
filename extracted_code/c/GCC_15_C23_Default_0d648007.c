/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1404
 * Language: c
 * Block ID: 0d648007
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* memccpy 示例 */
#include <stdio.h>
#include <string.h>

int main(void) {
    char src[] = "Hello, World! This is C23.";
    char dest[50];

    /* memccpy: 复制直到遇到字符 c 或复制 n 字节 */
    /* 复制到 ',' 为止 */
    void* result = memccpy(dest, src, ',', sizeof(dest));

    if (result) {
        /* 找到 ','，result 指向复制位置的下一个字节 */
        *(char*)result = '\0';  /* 添加终止符 */
        printf("Copied until ',': '%s'\n", dest);
    } else {
        /* 没找到 ','，完整复制 */
        printf("Copied all: '%s'\n", dest);
    }

    /* 实际应用场景: 解析 CSV */
    const char* csv = "name,age,city";
    char field[32];

    const char* p = csv;
    int field_num = 0;

    while (*p) {
        void* end = memccpy(field, p, ',', sizeof(field) - 1);

        if (end) {
            /* 找到逗号 */
            *(char*)end = '\0';
            p = p + (end - (void*)field);
        } else {
            /* 最后一个字段 */
            strncpy(field, p, sizeof(field) - 1);
            field[sizeof(field) - 1] = '\0';
            p = p + strlen(p);
        }

        printf("Field %d: '%s'\n", ++field_num, field);
    }

    return 0;
}
