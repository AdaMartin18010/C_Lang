/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1721
 * Language: c
 * Block ID: b728a31d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: complex_nesting.c
// compile: clang -std=c2y complex_nesting.c -o complex_nesting

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[32];
    int *data;
    size_t size;
} Resource;

void complex_nested_scenario(void) {
    printf("=== Complex Nested Scenario ===\n");

    // 外层资源
    Resource outer = {"outer", NULL, 0};
    outer.data = malloc(100 * sizeof(int));
    if (outer.data) {
        outer.size = 100;
        defer {
            printf("Cleanup: %s resource (%zu integers)\n", outer.name, outer.size);
            free(outer.data);
        }

        // 初始化数据
        for (size_t i = 0; i < outer.size; i++) {
            outer.data[i] = i;
        }

        // 第一层嵌套
        printf("Entering first nested block\n");
        {
            Resource inner1 = {"inner1", NULL, 0};
            inner1.data = malloc(50 * sizeof(int));
            if (inner1.data) {
                inner1.size = 50;
                defer {
                    printf("  Cleanup: %s resource\n", inner1.name);
                    free(inner1.data);
                }

                printf("  Processing inner1\n");

                // 第二层嵌套
                printf("  Entering second nested block\n");
                {
                    Resource inner2 = {"inner2", NULL, 0};
                    inner2.data = malloc(25 * sizeof(int));
                    if (inner2.data) {
                        inner2.size = 25;
                        defer {
                            printf("    Cleanup: %s resource\n", inner2.name);
                            free(inner2.data);
                        }

                        printf("    Processing inner2\n");
                    }
                }  // inner2 cleanup
                printf("  Exited second nested block\n");

                printf("  Continuing inner1 processing\n");
            }
        }  // inner1 cleanup
        printf("Exited first nested block\n");

        printf("Continuing outer processing\n");
    }
    printf("Function ending\n");
}  // outer cleanup

void loop_with_defer(void) {
    printf("\n=== Loop with defer ===\n");

    for (int i = 0; i < 3; i++) {
        printf("Iteration %d start\n", i);

        int *temp = malloc(10);
        defer {
            printf("  Iteration %d cleanup\n", i);
            free(temp);
        }

        printf("  Processing in iteration %d\n", i);

        // 嵌套条件块
        if (i == 1) {
            printf("  Special handling for iteration 1\n");

            FILE *fp = fopen("temp.txt", "w");
            if (fp) {
                defer {
                    printf("    Closing file in iteration 1\n");
                    fclose(fp);
                }
                fprintf(fp, "Iteration %d\n", i);
            }
        }

        printf("Iteration %d end\n", i);
    }

    printf("After loop\n");
}

int main(void) {
    complex_nested_scenario();
    loop_with_defer();
    return 0;
}
