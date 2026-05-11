/*
 * MISRA C:2023 函数安全规则对照
 * 编译: gcc -Wall -Wextra -std=c11 -o function_safety function_safety.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 * RULE 8.2: 函数类型应显式声明
 * ================================================================ */

/* ✅ 合规: 原型声明 */
int rule8_2_compliant(int x);

int rule8_2_compliant(int x) {
    return x * 2;
}

/* ================================================================
 * RULE 17.1: 函数参数数量不应过多（建议 <= 5）
 * ================================================================ */

/* ❌ 违规: 过多参数 */
void rule17_1_violation(int a, int b, int c, int d, int e, int f, int g) {
    (void)(a + b + c + d + e + f + g);
}

/* ✅ 合规: 使用结构体聚合参数 */
typedef struct {
    int a, b, c, d, e;
} Config;

void rule17_1_compliant(const Config *cfg) {
    if (!cfg) return;
    (void)(cfg->a + cfg->b + cfg->c + cfg->d + cfg->e);
}

/* ================================================================
 * RULE 17.5: 函数不应调用自身（直接/间接递归）
 * ================================================================ */

/* ❌ 违规: 间接递归 */
void rule17_5_violation_a(int x);
void rule17_5_violation_b(int x);

void rule17_5_violation_a(int x) {
    if (x > 0) rule17_5_violation_b(x - 1);
}
void rule17_5_violation_b(int x) {
    if (x > 0) rule17_5_violation_a(x - 1);
}

/* ✅ 合规: 消除递归，使用迭代或状态机 */
void rule17_5_compliant(int x) {
    while (x > 0) {
        x--;
    }
}

/* ================================================================
 * RULE 17.7: 函数的返回值应被使用（除非显式 void）
 * ================================================================ */

/* ❌ 违规: 忽略返回值 */
void rule17_7_violation(void) {
    malloc(100);  /* 返回值被忽略，可能分配失败 */
}

/* ✅ 合规: 检查返回值 */
void rule17_7_compliant(void) {
    void *p = malloc(100);
    if (p) {
        /* 使用 p */
        free(p);
    }
}

/* 或显式转换 void 表示故意忽略 */
void rule17_7_compliant_explicit(void) {
    (void)printf("output\n");  /* 显式忽略返回值 */
}

/* ================================================================
 * RULE 21.3: 不应使用动态内存分配
 * (MISRA C 部分配置文件禁止，嵌入式常用)
 * ================================================================ */

/* ❌ 违规: 使用 malloc/free */
void rule21_3_violation(void) {
    int *p = malloc(sizeof(int) * 10);
    if (p) {
        p[0] = 1;
        free(p);
    }
}

/* ✅ 合规: 静态分配或使用预分配池 */
#define MAX_ITEMS 10
static int g_pool[MAX_ITEMS];
static int g_pool_used = 0;

int *rule21_3_compliant(void) {
    if (g_pool_used >= MAX_ITEMS) {
        return NULL;  /* 池耗尽 */
    }
    return &g_pool[g_pool_used++];
}

/* 或栈分配（如果大小确定且安全） */
void rule21_3_compliant_stack(void) {
    int local[10];
    local[0] = 1;
    (void)local[0];
}

/* ================================================================
 * RULE 21.6: 不应使用标准库输入/输出函数
 * (某些安全关键配置)
 * ================================================================ */

/* ❌ 违规: 使用 printf/scanf */
void rule21_6_violation(void) {
    printf("hello\n");  /* 非确定性，格式化漏洞风险 */
}

/* ✅ 合规: 使用安全的写入函数 */
void rule21_6_compliant(const char *msg) {
    /* 使用已知长度的写入，无格式化 */
    size_t len = strlen(msg);
    /* 在实际系统中使用 UART_putbytes 等 */
    (void)len;
}

/* ================================================================
 * RULE 22.5: 指针使用前应检查非空
 * ================================================================ */

/* ❌ 违规: 未检查空指针 */
void rule22_5_violation(int *p) {
    *p = 42;  /* p 可能为 NULL */
}

/* ✅ 合规: 使用前检查 */
void rule22_5_compliant(int *p) {
    if (p != NULL) {
        *p = 42;
    }
}

/* ================================================================
 * 测试主函数
 * ================================================================ */
int main(void) {
    printf("MISRA Function Safety Examples compiled successfully\n");
    printf("8.2: %d\n", rule8_2_compliant(21));
    
    Config cfg = {1, 2, 3, 4, 5};
    rule17_1_compliant(&cfg);
    
    rule17_5_compliant(5);
    rule17_7_compliant();
    
    int *p = rule21_3_compliant();
    if (p) *p = 42;
    
    rule21_3_compliant_stack();
    rule22_5_compliant(p);
    
    return 0;
}
