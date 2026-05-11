/*
 * MISRA C:2023 内存安全规则对照（指针与数组）
 * 编译: gcc -Wall -Wextra -std=c11 -o memory_safety memory_safety.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * RULE 9.1: 对象值在用于计算前不应被读取
 * ================================================================ */

/* ❌ 违规: 使用未初始化变量 */
int rule9_1_violation(void) {
    int x;
    return x + 1;  /* x 未初始化 */
}

/* ✅ 合规: 使用前初始化 */
int rule9_1_compliant(void) {
    int x = 0;
    return x + 1;
}

/* ================================================================
 * RULE 18.1: 指针运算不应超过数组边界
 * ================================================================ */

/* ❌ 违规: 指针越界 */
int rule18_1_violation(void) {
    int arr[10];
    int *p = &arr[10];  /* arr[10] 越界，仅允许取 &arr[10] 作为哨兵 */
    return *p;          /* 解引用越界指针 */
}

/* ✅ 合规: 严格在边界内操作 */
int rule18_1_compliant(void) {
    int arr[10] = {0};
    int *end = &arr[10];   /* 哨兵，不解引用 */
    int *p = &arr[0];
    while (p < end) {
        *p = 0;
        p++;
    }
    return arr[0];
}

/* ================================================================
 * RULE 18.2: 减法仅应用于相同数组的指针
 * ================================================================ */

/* ❌ 违规: 不同数组的指针相减 */
ptrdiff_t rule18_2_violation(void) {
    int a[10];
    int b[10];
    return &a[5] - &b[2];  /* 未定义行为 */
}

/* ✅ 合规: 相同数组内相减 */
ptrdiff_t rule18_2_compliant(void) {
    int a[10];
    return &a[5] - &a[2];  /* 结果 = 3 */
}

/* ================================================================
 * RULE 18.3: 关系运算仅应用于相同数组的指针
 * ================================================================ */

/* ❌ 违规: 不同数组指针比较 */
int rule18_3_violation(void) {
    int a[10];
    int b[10];
    return &a[0] < &b[0];  /* 未定义行为 */
}

/* ✅ 合规: 仅比较相同数组指针 */
int rule18_3_compliant(int *p, int *q, size_t n) {
    /* p 和 q 指向同一数组 */
    (void)n;
    if (p < q) {
        return 1;
    }
    return 0;
}

/* ================================================================
 * RULE 18.6: 函数不应返回局部对象的地址
 * ================================================================ */

/* ❌ 违规: 返回局部变量地址 */
int *rule18_6_violation(void) {
    int local = 42;
    return &local;  /* local 在函数返回后失效 */
}

/* ✅ 合规: 返回堆内存或静态存储 */
int *rule18_6_compliant(void) {
    static int persistent = 42;  /* 静态存储期 */
    return &persistent;
}

/* 或堆分配 */
int *rule18_6_compliant_heap(void) {
    int *p = malloc(sizeof(int));
    if (p) *p = 42;
    return p;  /* 调用方负责释放 */
}

/* ================================================================
 * RULE 18.7: 通过指向灵活数组成员的指针访问
 * ================================================================ */

/* ❌ 违规: 未分配足够内存就访问灵活数组 */
typedef struct {
    size_t len;
    int data[];  /* 灵活数组成员 */
} FlexArray;

void rule18_7_violation(void) {
    FlexArray *fa = malloc(sizeof(FlexArray));  /* 没有分配 data 空间 */
    fa->data[0] = 1;  /* 越界写入 */
    free(fa);
}

/* ✅ 合规: 分配足够内存 */
void rule18_7_compliant(void) {
    size_t n = 10;
    FlexArray *fa = malloc(sizeof(FlexArray) + n * sizeof(int));
    if (fa) {
        fa->len = n;
        for (size_t i = 0; i < n; i++) {
            fa->data[i] = (int)i;
        }
        free(fa);
    }
}

/* ================================================================
 * RULE 22.1: 分配的资源应显式释放
 * ================================================================ */

/* ❌ 违规: 内存泄漏 */
void rule22_1_violation(void) {
    int *p = malloc(sizeof(int) * 100);
    (void)p;
    /* 忘记 free(p) */
}

/* ✅ 合规: 每个 malloc 对应一个 free */
void rule22_1_compliant(void) {
    int *p = malloc(sizeof(int) * 100);
    if (p) {
        /* 使用 p */
        free(p);
    }
}

/* ================================================================
 * 测试主函数
 * ================================================================ */
int main(void) {
    printf("MISRA Memory Safety Examples compiled successfully\n");
    printf("9.1: %d\n", rule9_1_compliant());
    printf("18.1: %d\n", rule18_1_compliant());
    printf("18.2: %td\n", rule18_2_compliant());
    
    int a[10];
    printf("18.3: %d\n", rule18_3_compliant(&a[2], &a[5], 10));
    
    int *p = rule18_6_compliant();
    printf("18.6: %d\n", *p);
    
    rule18_7_compliant();
    rule22_1_compliant();
    
    return 0;
}
