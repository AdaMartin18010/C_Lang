/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\README.md
 * Line: 686
 * Language: c
 * Block ID: 748ca13e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 文件操作示例 */
#include <stdio.h>

void file_operations(void) {
    // 打开文件
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL) {
        perror("Failed to open file");
        return;
    }

    // 读取操作
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    // 写入操作
    FILE *out = fopen("output.txt", "w");
    fprintf(out, "Hello, %s!\n", "World");
    fwrite(buffer, 1, strlen(buffer), out);

    // 关闭文件
    fclose(fp);
    fclose(out);
}
