/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\02_Segfault_Troubleshooting.md
 * Line: 198
 * Language: c
 * Block ID: b4fb6337
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// segfault_demo.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void buggy_function(const char *input) {
    char *buffer = NULL;

    if (input != NULL) {
        buffer = malloc(strlen(input) + 1);
        strcpy(buffer, input);
    }

    // 潜在错误: 如果input为NULL，buffer未初始化
    printf("Buffer content: %s\n", buffer);
    free(buffer);
}

void process_data(int index) {
    int data[5] = {1, 2, 3, 4, 5};
    // 潜在错误: 未检查index范围
    printf("Data[%d] = %d\n", index, data[index]);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        buggy_function(argv[1]);
        process_data(atoi(argv[2]));
    } else {
        buggy_function(NULL);  // 触发segfault
    }
    return 0;
}
