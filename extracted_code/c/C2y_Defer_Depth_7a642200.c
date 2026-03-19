/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1113
 * Language: c
 * Block ID: 7a642200
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: conditional_return.c
// compile: clang -std=c2y conditional_return.c -o conditional_return

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int conditional_processing(int value) {
    printf("\n=== Processing value: %d ===\n", value);

    int *data = malloc(100 * sizeof(int));
    if (!data) return -1;
    defer {
        printf("Cleanup: freeing data\n");
        free(data);
    }

    // 初始化数据
    for (int i = 0; i < 100; i++) {
        data[i] = value + i;
    }

    if (value < 0) {
        printf("Error: negative value\n");
        return -1;  // defer执行
    }

    if (value == 0) {
        printf("Warning: zero value\n");
        return 0;  // defer执行
    }

    if (value > 1000) {
        printf("Error: value too large\n");
        return -2;  // defer执行
    }

    printf("Processing completed successfully\n");
    return value * 2;  // defer执行
}

int main(void) {
    srand(time(NULL));

    conditional_processing(-5);
    conditional_processing(0);
    conditional_processing(50);
    conditional_processing(2000);

    return 0;
}
