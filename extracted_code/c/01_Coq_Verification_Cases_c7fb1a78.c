/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_Coq_Examples\01_Coq_Verification_Cases.md
 * Line: 3128
 * Language: c
 * Block ID: c7fb1a78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* File: c_impl/loop_invariants.c */

#include <stdio.h>
#include <assert.h>
#include <limits.h>

/* ============================================================ */
/* 1. 阶乘计算（带不变量注释） */
/* ============================================================ */

/*
 * 前置条件: n >= 0
 * 后置条件: result = n!
 * 不变量: acc * i! = n! /\\ 0 <= i <= n
 * 变体: i
 */
unsigned long factorial(int n) {
    unsigned long acc = 1;
    int i = n;

    while (i > 0) {
        /* 不变量检查点 */
        acc = acc * i;
        i--;
    }

    return acc;
}

/* ============================================================ */
/* 2. 欧几里得GCD */
/* ============================================================ */

/*
 * 前置条件: a >= 0 /\\ b >= 0 /\\ a + b > 0
 * 后置条件: result = gcd(a, b)
 * 不变量: gcd(a, b) = gcd(a0, b0) /\\ a >= 0 /\\ b >= 0
 * 变体: a + b
 */
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/* ============================================================ */
/* 3. 二分查找 */
/* ============================================================ */

/*
 * 前置条件: arr已排序 /\\ n >= 0
 * 后置条件: 如果key在arr中，返回其索引；否则返回-1
 * 不变量: 如果key在arr中，则在arr[low..high]范围内 /\\ 0 <= low <= high+1 <= n
 * 变体: high - low
 */
int binary_search(int* arr, int n, int key) {
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
/* 4. 找数组最大值 */
/* ============================================================ */

/*
 * 前置条件: n > 0
 * 后置条件: result是arr中的最大值
 * 不变量: m = max(arr[0..i-1]) /\\ 1 <= i <= n
 * 变体: n - i
 */
int find_max(int* arr, int n) {
    assert(n > 0);
    int m = arr[0];
    int i = 1;

    while (i < n) {
        if (arr[i] > m) {
            m = arr[i];
        }
        i++;
    }

    return m;
}

/* ============================================================ */
/* 5. 数组逆序 */
/* ============================================================ */

/*
 * 前置条件: n >= 0
 * 后置条件: arr是原数组的逆序
 * 不变量: arr[0..i-1]和arr[n-i..n-1]已经交换 /\\ 0 <= i <= n/2
 * 变体: n/2 - i
 */
void reverse_array(int* arr, int n) {
    int i = 0;
    int j = n - 1;

    while (i < j) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
        i++;
        j--;
    }
}

/* ============================================================ */
/* 6. 测试函数 */
/* ============================================================ */

void test_factorial() {
    assert(factorial(0) == 1);
    assert(factorial(5) == 120);
    assert(factorial(10) == 3628800);
    printf("Factorial tests passed!\n");
}

void test_gcd() {
    assert(gcd(48, 18) == 6);
    assert(gcd(56, 98) == 14);
    assert(gcd(0, 5) == 5);
    assert(gcd(5, 0) == 5);
    printf("GCD tests passed!\n");
}

void test_find_max() {
    int arr1[] = {3, 1, 4, 1, 5, 9, 2, 6};
    assert(find_max(arr1, 8) == 9);

    int arr2[] = {5};
    assert(find_max(arr2, 1) == 5);

    printf("Find max tests passed!\n");
}

void test_reverse_array() {
    int arr[] = {1, 2, 3, 4, 5};
    reverse_array(arr, 5);
    assert(arr[0] == 5);
    assert(arr[4] == 1);
    printf("Reverse array tests passed!\n");
}
