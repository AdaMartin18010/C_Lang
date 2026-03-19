/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_Coq_Examples\01_Coq_Verification_Cases.md
 * Line: 2754
 * Language: c
 * Block ID: 8909733a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* File: c_impl/hoare_examples.c */

#include <stdio.h>
#include <assert.h>

/* ============================================================ */
/* 1. 交换两个变量 */
/* ============================================================ */

/*
 * 前置条件: x = x0 /\\ y = y0
 * 后置条件: x = y0 /\\ y = x0
 */
void swap(int* x, int* y) {
    int tmp = *x;
    *x = *y;
    *y = tmp;
}

/* ============================================================ */
/* 2. 使用加法计算乘法 */
/* ============================================================ */

/*
 * 前置条件: a >= 0 /\\ b >= 0
 * 后置条件: result = a * b
 * 循环不变量: result = i * b /\\ 0 <= i <= a
 */
int multiply(int a, int b) {
    int result = 0;
    int i = 0;

    while (i < a) {
        /* 不变量: result = i * b */
        result = result + b;
        i = i + 1;
    }

    return result;
}

/* ============================================================ */
/* 3. 数组求和 */
/* ============================================================ */

/*
 * 前置条件: n >= 0
 * 后置条件: sum = \\sum_{i=0}^{n-1} arr[i]
 * 循环不变量: sum = \\sum_{j=0}^{i-1} arr[j] /\\ 0 <= i <= n
 */
int array_sum(const int* arr, int n) {
    int sum = 0;
    int i = 0;

    while (i < n) {
        /* 不变量: sum = \\sum_{j=0}^{i-1} arr[j] */
        sum = sum + arr[i];
        i = i + 1;
    }

    return sum;
}

/* ============================================================ */
/* 4. 二分查找 */
/* ============================================================ */

/*
 * 前置条件: arr已排序 /\\ n >= 0
 * 后置条件: 如果返回i，则arr[i] = key；如果返回-1，则key不在arr中
 * 循环不变量:
 *   如果key在arr中，则key在arr[low..high]范围内
 *   0 <= low <= high+1 <= n
 */
int binary_search(const int* arr, int n, int key) {
    int low = 0;
    int high = n - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (arr[mid] == key) {
            return mid;
        } else if (arr[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

/* ============================================================ */
/* 5. 测试函数 */
/* ============================================================ */

void test_swap() {
    int x = 5, y = 10;
    swap(&x, &y);
    assert(x == 10 && y == 5);
    printf("Swap test passed!\n");
}

void test_multiply() {
    assert(multiply(3, 4) == 12);
    assert(multiply(0, 100) == 0);
    assert(multiply(5, 0) == 0);
    printf("Multiply test passed!\n");
}

void test_array_sum() {
    int arr[] = {1, 2, 3, 4, 5};
    assert(array_sum(arr, 5) == 15);
    printf("Array sum test passed!\n");
}

void test_binary_search() {
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int n = 7;

    assert(binary_search(arr, n, 5) == 2);
    assert(binary_search(arr, n, 1) == 0);
    assert(binary_search(arr, n, 13) == 6);
    assert(binary_search(arr, n, 4) == -1);
    printf("Binary search test passed!\n");
}
