/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 578
 * Language: c
 * Block ID: e89fede4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 试图通过一级指针返回分配的内存
void alloc_array_wrong(int *arr, size_t n) {
    // 意图: 让外部使用分配的数组
    arr = malloc(n * sizeof(int));  // ❌ 局部赋值，外部不可见
    if (arr) {
        memset(arr, 0, n * sizeof(int));
    }
    // 内存泄漏！arr是局部变量，退出后丢失
}

// ❌ 错误: 返回局部变量的地址
int *alloc_array_very_wrong(size_t n) {
    int local[100];  // 栈上的局部数组
    return local;    // ❌ 返回后local被销毁，悬挂指针！
}

// ✅ 正确1: 返回指针
int *alloc_array_return(size_t n) {
    int *arr = malloc(n * sizeof(int));
    if (arr)
        memset(arr, 0, n * sizeof(int));
    return arr;  // ✅ 返回堆内存地址
}

// ✅ 正确2: 使用二级指针
int alloc_array_pp(size_t n, int **out_arr) {
    if (!out_arr)
        return -1;

    *out_arr = malloc(n * sizeof(int));  // ✅ 修改外部指针
    if (!*out_arr)
        return -ENOMEM;

    memset(*out_arr, 0, n * sizeof(int));
    return 0;
}

// 使用
void example(void) {
    int *arr = NULL;

    // 错误用法
    alloc_array_wrong(arr, 100);  // arr仍然是NULL

    // 正确用法1
    arr = alloc_array_return(100);
    free(arr);

    // 正确用法2
    int ret = alloc_array_pp(100, &arr);
    if (ret == 0) {
        // 使用arr
        free(arr);
    }
}
