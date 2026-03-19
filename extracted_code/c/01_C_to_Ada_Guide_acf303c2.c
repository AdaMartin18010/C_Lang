/*
 * Auto-generated from: 17_Ada_SPARK\06_Migration_from_C\01_C_to_Ada_Guide.md
 * Line: 302
 * Language: c
 * Block ID: acf303c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int* create_array(size_t n) {
    int* arr = malloc(n * sizeof(int));
    if (arr == NULL) return NULL;
    for (size_t i = 0; i < n; i++) {
        arr[i] = 0;
    }
    return arr;
}

void destroy_array(int* arr) {
    free(arr);
}
