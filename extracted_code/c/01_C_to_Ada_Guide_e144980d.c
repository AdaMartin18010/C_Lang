/*
 * Auto-generated from: 17_Ada_SPARK\06_Migration_from_C\01_C_to_Ada_Guide.md
 * Line: 394
 * Language: c
 * Block ID: e144980d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef int (*Comparator)(const void*, const void*);

int compare_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

void sort(void* arr, size_t n, size_t size, Comparator cmp);
