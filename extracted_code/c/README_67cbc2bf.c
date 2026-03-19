/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\README.md
 * Line: 666
 * Language: c
 * Block ID: 67cbc2bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ===== 递归函数 ===== */

// 阶乘计算
unsigned long long factorial(unsigned int n)
{
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 尾递归优化形式
unsigned long long factorial_tail(unsigned int n,
                                   unsigned long long acc)
{
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}

// 斐波那契（带备忘录）
unsigned long long fibonacci(int n, unsigned long long *memo)
{
    if (n <= 1) return n;
    if (memo[n] != 0) return memo[n];

    memo[n] = fibonacci(n - 1, memo) + fibonacci(n - 2, memo);
    return memo[n];
}

/* ===== 函数指针 ===== */

// 函数类型定义
typedef int (*compare_func)(const void *, const void *);
typedef void (*operation_func)(int);

// 比较函数
int int_compare_asc(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

int int_compare_desc(const void *a, const void *b)
{
    return -int_compare_asc(a, b);
}

// 回调模式
void execute_operation(int value, operation_func op)
{
    op(value);
}

void print_value(int v) { printf("Value: %d\n", v); }
void print_square(int v) { printf("Square: %d\n", v * v); }

// 函数指针数组
void demonstrate_func_array(void)
{
    operation_func operations[] = {
        print_value,
        print_square,
    };

    for (size_t i = 0; i < 2; i++) {
        `operations[i](5);`
    }
}
