/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\README.md
 * Line: 654
 * Language: c
 * Block ID: 2be08d64
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// math_lib.c
int add(int a, int b) {
    return a + b;
}

// main.c
#include <stdio.h>

int add(int a, int b);  // 声明

int main() {
    int result = add(3, 4);  // 可以被内联
    printf("%d\n", result);
    return 0;
}
