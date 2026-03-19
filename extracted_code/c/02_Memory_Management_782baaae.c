/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1929
 * Language: c
 * Block ID: 782baaae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 使用malloc后未初始化
void uninitialized_malloc(void) {
    int *arr = malloc(10 * sizeof(int));
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += arr[i];  // ❌ UB: 读取未初始化值
    }
    free(arr);
}

// ❌ 错误: 栈变量未初始化
void uninitialized_stack(void) {
    int x;
    if (x > 0) {  // ❌ UB: 使用未初始化变量
        printf("positive\n");
    }
}

// ❌ 错误: 结构体部分初始化
struct Point {
    int x, y, z;
};

void uninitialized_struct(void) {
    struct Point p;
    p.x = 1;
    p.y = 2;
    // p.z 未初始化
    printf("%d\n", p.z);  // ❌ UB
}

// ❌ 错误: realloc扩展部分未初始化
void uninitialized_realloc(void) {
    int *arr = calloc(10, sizeof(int));  // 全部初始化为0
    arr = realloc(arr, 20 * sizeof(int));
    // arr[10..19] 未初始化！
    int sum = 0;
    for (int i = 0; i < 20; i++) {
        sum += arr[i];  // ❌ 读取未初始化
    }
    free(arr);
}

// ✅ 正确: 始终初始化内存
void initialized_safe(void) {
    // 方式1: 使用calloc
    int *arr1 = calloc(10, sizeof(int));  // 全部为零

    // 方式2: malloc + memset
    int *arr2 = malloc(10 * sizeof(int));
    if (arr2) memset(arr2, 0, 10 * sizeof(int));

    // 方式3: 显式初始化
    int *arr3 = malloc(10 * sizeof(int));
    if (arr3) {
        for (int i = 0; i < 10; i++) {
            arr3[i] = 0;
        }
    }

    // 方式4: 栈变量声明时初始化
    int x = 0;
    struct Point p = {1, 2, 3};  // 完全初始化

    free(arr1);
    free(arr2);
    free(arr3);
}
