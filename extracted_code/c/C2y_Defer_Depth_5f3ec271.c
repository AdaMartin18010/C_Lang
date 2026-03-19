/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 479
 * Language: c
 * Block ID: 5f3ec271
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void lifo_example(void) {
    FILE *file1 = fopen("file1.txt", "r");
    defer {
        printf("Closing file1\n");
        fclose(file1);
    }

    FILE *file2 = fopen("file2.txt", "r");
    defer {
        printf("Closing file2\n");
        fclose(file2);
    }

    void *resource = acquire_resource();
    defer {
        printf("Releasing resource\n");
        release_resource(resource);
    }

    printf("Function body\n");
}

// 输出顺序：
// Function body
// Releasing resource
// Closing file2
// Closing file1
