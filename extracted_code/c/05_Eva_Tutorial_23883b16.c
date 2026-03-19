/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 195
 * Language: c
 * Block ID: 23883b16
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial1_basic.c
#include <stdio.h>

int abs_value(int x) {
    if (x >= 0) {
        return x;
    } else {
        return -x;
    }
}

int main() {
    int input;
    scanf("%d", &input);  // 任意整数值

    int result = abs_value(input);

    printf("Result: %d\n", result);
    return 0;
}
