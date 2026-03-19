/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2279
 * Language: c
 * Block ID: f4346e99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 大数组分配在栈上
void stack_overflow_large_array(void) {
    int big_array[10000000];  // ❌ 40MB栈数组！通常栈只有1-8MB
    big_array[0] = 1;
}

// ❌ 错误: 递归无终止条件
void stack_overflow_infinite_recursion(int n) {
    int local[1000];  // 消耗栈空间
    local[0] = n;
    stack_overflow_infinite_recursion(n + 1);  // ❌ 无限递归
}

// ❌ 错误: 深层递归
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  // ❌ 大n时栈溢出
}

// ❌ 错误: 变长数组过大
void stack_overflow_vla(int n) {
    int arr[n];  // ❌ 如果n很大，栈溢出
    arr[0] = 0;
}

// ✅ 正确: 大数组使用堆分配
void stack_safe_heap(void) {
    int *big_array = malloc(10000000 * sizeof(int));  // 堆分配
    if (big_array) {
        big_array[0] = 1;
        free(big_array);
    }
}

// ✅ 正确: 尾递归优化或迭代
int factorial_safe(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// ✅ 正确: 限制VLA大小
void stack_safe_vla(int n) {
    #define MAX_STACK_ARRAY 10000
    if (n > MAX_STACK_ARRAY) {
        // 太大，使用堆
        int *arr = malloc(n * sizeof(int));
        if (arr) {
            arr[0] = 0;
            free(arr);
        }
    } else {
        int arr[MAX_STACK_ARRAY];  // 固定大小，安全
        arr[0] = 0;
    }
}
