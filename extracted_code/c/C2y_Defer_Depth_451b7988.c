/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1073
 * Language: c
 * Block ID: 451b7988
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: lifo_order.c
// compile: clang -std=c2y lifo_order.c -o lifo_order

#include <stdio.h>

void demonstrate_lifo_order(void) {
    printf("\n=== LIFO Execution Order ===\n");

    defer printf("1. First defer (executed last)\n");
    defer printf("2. Second defer (executed second)\n");
    defer printf("3. Third defer (executed first)\n");

    printf("4. Function body\n");
}

void nested_blocks(void) {
    printf("\n=== Nested Blocks ===\n");

    defer printf("Outer block - defer 1\n");

    {
        defer printf("Inner block - defer A\n");
        defer printf("Inner block - defer B\n");
        printf("Inner block body\n");
    }  // 执行 defer B, defer A

    printf("Outer block body\n");
    // 执行 defer 1
}

int main(void) {
    demonstrate_lifo_order();
    nested_blocks();
    return 0;
}
