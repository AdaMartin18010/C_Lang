/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 1084
 * Language: c
 * Block ID: 77b93544
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 常见HLS错误与解决方案
 */

// 错误1：变量边界不确定
void error_uncertain_bound(int a[], int n) {  // 错误！
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        a[i] = a[i] * 2;
    }
}
// 错误：数组大小未知

// 修正1：使用最大边界
void fix_bound(int a[1000], int n) {
    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        if (i < n) {  // 条件执行
            a[i] = a[i] * 2;
        }
    }
}

// 错误2：递归调用
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  // 错误！
}

// 修正2：迭代实现
int factorial_fixed(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        #pragma HLS PIPELINE
        result *= i;
    }
    return result;
}

// 错误3：动态内存分配
void error_dynamic_alloc(int n) {
    int *array = malloc(n * sizeof(int));  // 错误！
    // ...
    free(array);  // 错误！
}

// 修正3：静态数组
void fix_static_array(int n) {
    int array[1000];  // 最大可能大小
    for (int i = 0; i < n && i < 1000; i++) {
        array[i] = i;
    }
}

// 错误4：指针别名
void error_aliasing(int *a, int *b) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1  // 无法达到
        a[i] = a[i-1] + b[i];  // 可能a和b重叠
    }
}

// 修正4：使用restrict
void fix_aliasing(int *restrict a, int *restrict b) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        a[i] = a[i-1] + b[i];
    }
}
