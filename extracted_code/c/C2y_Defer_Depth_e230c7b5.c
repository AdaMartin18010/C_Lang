/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2962
 * Language: c
 * Block ID: e230c7b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// test_defer.c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *p = malloc(100);
    defer free(p);

    printf("Hello, C2y defer!\n");
    return 0;
}
