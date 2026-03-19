/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1026
 * Language: c
 * Block ID: 831c9b0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: basic_example.c
// compile: clang -std=c2y basic_example.c -o basic_example

#include <stdio.h>
#include <stdlib.h>

void basic_resource_management(void) {
    printf("=== Basic Resource Management ===\n");

    // 分配内存
    int *numbers = malloc(10 * sizeof(int));
    if (!numbers) {
        perror("malloc");
        return;
    }
    defer free(numbers);
    printf("Memory allocated\n");

    // 打开文件
    FILE *fp = fopen("test.txt", "w");
    if (!fp) {
        perror("fopen");
        return;  // numbers会自动释放
    }
    defer fclose(fp);
    printf("File opened\n");

    // 使用资源
    for (int i = 0; i < 10; i++) {
        numbers[i] = i * i;
        fprintf(fp, "numbers[%d] = %d\n", i, numbers[i]);
    }

    printf("Resources used successfully\n");
    // 函数退出时：先关闭文件，再释放内存
}

int main(void) {
    basic_resource_management();
    printf("Function completed\n");
    return 0;
}
