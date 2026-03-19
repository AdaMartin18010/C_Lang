/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\08_Debugging_Tools\02_Valgrind_Memory.md
 * Line: 85
 * Language: c
 * Block ID: 395b3fdb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void invalid_access() {
    int *arr = malloc(sizeof(int) * 5);
    arr[5] = 42;  // 越界写入！
    free(arr);
}

void use_after_free() {
    int *ptr = malloc(sizeof(int));
    free(ptr);
    *ptr = 42;  // 使用已释放内存！
}

void uninitialized() {
    int x;
    if (x > 0) {  // 使用未初始化变量！
        printf("x is positive\n");
    }
}
