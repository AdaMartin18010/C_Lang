/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1672
 * Language: c
 * Block ID: 88837b55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: nested_functions.c
// compile: clang -std=c2y nested_functions.c -o nested_functions

#include <stdio.h>
#include <stdlib.h>

void inner_function(int level) {
    printf("%*sEntering inner_function level %d\n", level * 2, "", level);

    int *data = malloc(100);
    defer {
        printf("%*sInner level %d cleanup\n", level * 2, "", level);
        free(data);
    }

    if (level < 3) {
        inner_function(level + 1);
    }

    printf("%*sExiting inner_function level %d\n", level * 2, "", level);
}

void outer_function(void) {
    printf("Entering outer_function\n");

    FILE *fp = fopen("outer.txt", "w");
    if (fp) {
        defer {
            printf("Outer function cleanup\n");
            fclose(fp);
        }

        fprintf(fp, "Outer function log\n");

        inner_function(1);
    }

    printf("Exiting outer_function\n");
}

int main(void) {
    outer_function();
    return 0;
}
