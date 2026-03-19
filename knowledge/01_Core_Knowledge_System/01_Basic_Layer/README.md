# C 语言基础层


---

## 📑 目录

- [C 语言基础层](#c-语言基础层)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [C 语言概述](#c-语言概述)
    - [语言特性与设计哲学](#语言特性与设计哲学)
    - [程序结构](#程序结构)
  - [数据类型基础](#数据类型基础)
    - [基本类型系统](#基本类型系统)
    - [类型修饰与限定](#类型修饰与限定)
    - [枚举与类型推导](#枚举与类型推导)
  - [运算符系统](#运算符系统)
    - [算术与位运算](#算术与位运算)
    - [关系与逻辑运算符](#关系与逻辑运算符)
    - [赋值与复合运算符](#赋值与复合运算符)
  - [控制流结构](#控制流结构)
    - [条件语句](#条件语句)
    - [循环结构](#循环结构)
  - [函数基础](#函数基础)
    - [函数定义与声明](#函数定义与声明)
    - [递归与函数指针](#递归与函数指针)
    - [可变参数函数](#可变参数函数)
  - [作用域与存储期](#作用域与存储期)
  - [最佳实践](#最佳实践)
    - [编码规范要点](#编码规范要点)
  - [延伸阅读](#延伸阅读)


---

## 概述

C 语言基础层是掌握这门强大系统编程语言的起点。
本层涵盖 C 语言的核心语法元素、基础数据类型、运算符系统、控制流结构和函数机制。
深入理解这些基础概念是构建复杂系统软件的基石。

---

## C 语言概述

### 语言特性与设计哲学

C 语言由 Dennis Ritchie 于 1972 年在贝尔实验室开发，具有以下核心特征：

```c
/*
 * C 语言设计哲学示例
 * - 简洁性：语言特性精简
 * - 高效性：接近底层硬件
 * - 可移植性：抽象机器模型
 * - 信任程序员：最小运行时检查
 */

#include <stdio.h>
#include <stdint.h>

// 精确控制数据表示（可移植性）
struct PacketHeader {
    uint16_t magic;      // 2 字节，无歧义
    uint32_t length;     // 4 字节，跨平台一致
    uint8_t  flags;      // 1 字节，精确位宽
} __attribute__((packed)); // 无填充，精确内存布局

// 直接硬件访问能力（高效性）
volatile uint32_t *const TIMER_REG = (uint32_t *)0x40000000;

void reset_timer(void) {
    *TIMER_REG = 0;  // 直接寄存器操作
}
```

### 程序结构

```c
// 预处理指令：编译器配置
#include <stdio.h>     // 标准 I/O 头文件
#include <stdlib.h>    // 标准库头文件

// 宏定义：编译时常量
#define MAX_BUFFER_SIZE 1024
#define SQUARE(x) ((x) * (x))

// 类型定义：抽象数据表示
typedef unsigned char byte;
typedef struct Node Node;

// 全局声明：外部可见符号
extern int global_counter;
static int internal_state;  // 文件作用域私有

// 函数声明：接口契约
int process_data(const char *input, size_t len);
static void helper_function(void);

// 主函数：程序入口点
int main(int argc, char *argv[])
{
    // 命令行参数处理
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 程序主体逻辑
    const char *filename = argv[1];
    int result = process_data(filename, 0);

    return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// 函数定义：实现细节
int process_data(const char *input, size_t len)
{
    // 实现逻辑
    printf("Processing: %s\n", input);
    return 0;
}
```

---

## 数据类型基础

### 基本类型系统

```c
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <complex.h>  // C99

void demonstrate_types(void)
{
    /* ===== 整数类型 ===== */

    // 有符号整数
    signed char sc = -128;           // 保证范围：-128 到 127
    short sh = -32768;               // 至少 16 位
    int i = 2147483647;              // 自然字长，通常 32 位
    long lo = 2147483647L;           // 至少 32 位
    long long ll = 9223372036854775807LL;  // 至少 64 位 (C99)

    // 无符号整数
    unsigned char uc = 255;
    unsigned short us = 65535U;
    unsigned int ui = 4294967295U;
    unsigned long ul = 4294967295UL;
    unsigned long long ull = 18446744073709551615ULL;

    // 定宽整数类型 (C99 stdint.h)
    int8_t   i8  = -128;             // 精确 8 位
    int16_t  i16 = -32768;           // 精确 16 位
    int32_t  i32 = -2147483648;      // 精确 32 位
    int64_t  i64 = -9223372036854775807LL;  // 精确 64 位

    uint8_t  u8  = 255;              // 无符号 8 位
    uint16_t u16 = 65535;
    uint32_t u32 = 4294967295U;
    uint64_t u64 = 18446744073709551615ULL;

    // 指针尺寸整数
    intptr_t ptr_val = 0;            // 可存储指针的整数
    uintptr_t uptr = 0;              // 无符号版本

    /* ===== 浮点类型 ===== */
    // IEEE 754 标准详解见: IEEE_754_Floating_Point/

    float f = 3.14159f;              // 单精度(binary32)，IEEE 754
    double d = 3.141592653589793;    // 双精度(binary64)，IEEE 754
    long double ld = 3.14159265358979323846L;  // 扩展精度

    // 特殊值
    float inf = INFINITY;            // 正无穷
    float nan = NAN;                 // 非数字
    float eps = FLT_EPSILON;         // 机器精度(~1.19e-07)

    // 复数类型 (C99)
    float complex z = 1.0f + 2.0f*I;
    double complex w = 1.0 + 2.0*I;

    /* ===== 布尔类型 (C99) ===== */
    bool flag = true;  // 或 false

    /* ===== void 类型 ===== */
    void *generic_ptr = NULL;  // 通用指针

    printf("sizeof(char)      = %zu\n", sizeof(char));
    printf("sizeof(short)     = %zu\n", sizeof(short));
    printf("sizeof(int)       = %zu\n", sizeof(int));
    printf("sizeof(long)      = %zu\n", sizeof(long));
    printf("sizeof(long long) = %zu\n", sizeof(long long));
    printf("sizeof(void*)     = %zu\n", sizeof(void*));
    printf("sizeof(float)     = %zu\n", sizeof(float));
    printf("sizeof(double)    = %zu\n", sizeof(double));
}
```

### 类型修饰与限定

```c
// const 限定：只读语义
const int MAX_VALUE = 100;         // 常量值
const char *str = "hello";         // 指向常量的指针
char * const ptr = buffer;         // 常量指针
const char * const p = "fixed";    // 常量指针指向常量

// volatile 限定：易变语义
volatile int sensor_value;         // 可能被硬件改变
volatile uint32_t *hardware_reg;   // 内存映射寄存器

// restrict 限定 (C99)：指针别名优化提示
void vector_add(int n,
                double *restrict a,
                const double *restrict b,
                const double *restrict c)
{
    // 编译器可假设 a, b, c 指向不重叠内存
    for (int i = 0; i < n; i++) {
        a[i] = b[i] + c[i];
    }
}

// _Atomic 限定 (C11)：原子操作
#include <stdatomic.h>
_Atomic int counter = 0;

void increment(void) {
    atomic_fetch_add(&counter, 1);  // 线程安全自增
}

// 存储类说明符
static int internal_count;         // 内部链接
extern int shared_variable;        // 外部声明
thread_local int thread_data;      // 线程本地存储 (C11)
```

### 枚举与类型推导

```c
// 枚举类型定义
enum Color {
    RED,        // 0
    GREEN = 2,  // 2
    BLUE        // 3
};

enum Status {
    OK = 0,
    ERROR_INVALID = -1,
    ERROR_NOMEM = -2,
    ERROR_IO = -3
};

// 匿名枚举作为命名常量
enum {
    BUFFER_SIZE = 4096,
    MAX_FILENAME = 256,
    MAX_PATH = 4096
};

// typedef 创建类型别名
typedef enum Color Color;
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

// 现代类型推导 (C23 _Generic 模拟)
#define TYPE_OF(x) _Generic((x), \
    int: "int", \
    double: "double", \
    char*: "string", \
    default: "other")

// auto 类型推导 (C23)
// auto x = 42;  // x 推导为 int
```

---

## 运算符系统

### 算术与位运算

```c
#include <stdio.h>

void demonstrate_operators(void)
{
    int a = 17, b = 5;

    /* ===== 算术运算符 ===== */
    printf("a + b = %d\n", a + b);    // 22 - 加法
    printf("a - b = %d\n", a - b);    // 12 - 减法
    printf("a * b = %d\n", a * b);    // 85 - 乘法
    printf("a / b = %d\n", a / b);    // 3  - 整数除法（向零截断）
    printf("a %% b = %d\n", a % b);   // 2  - 取模运算

    // 浮点除法
    double fdiv = (double)a / b;     // 3.4

    /* ===== 位运算符 ===== */
    unsigned int x = 0b1100;  // 12
    unsigned int y = 0b1010;  // 10

    printf("x & y  = %u\n", x & y);   // 0b1000 = 8  (按位与)
    printf("x | y  = %u\n", x | y);   // 0b1110 = 14 (按位或)
    printf("x ^ y  = %u\n", x ^ y);   // 0b0110 = 6  (按位异或)
    printf("~x     = %u\n", ~x);      // 按位取反
    printf("x << 2 = %u\n", x << 2);  // 0b110000 = 48 (左移)
    printf("x >> 2 = %u\n", x >> 2);  // 0b11 = 3     (右移)

    /* ===== 实用位操作模式 ===== */

    // 设置第 n 位
    #define SET_BIT(val, n) ((val) |= (1U << (n)))

    // 清除第 n 位
    #define CLEAR_BIT(val, n) ((val) &= ~(1U << (n)))

    // 切换第 n 位
    #define TOGGLE_BIT(val, n) ((val) ^= (1U << (n)))

    // 检查第 n 位
    #define CHECK_BIT(val, n) (((val) >> (n)) & 1U)

    // 掩码操作
    uint32_t flags = 0;
    #define FLAG_READ   0x01
    #define FLAG_WRITE  0x02
    #define FLAG_EXEC   0x04

    flags |= FLAG_READ | FLAG_WRITE;   // 设置权限
    if (flags & FLAG_READ) {           // 检查权限
        printf("Read permission granted\n");
    }
    flags &= ~FLAG_WRITE;              // 移除写权限
}
```

### 关系与逻辑运算符

```c
void demonstrate_logical_ops(void)
{
    int a = 5, b = 10, c = 5;

    /* ===== 关系运算符 ===== */
    printf("a == b: %d\n", a == b);  // 0 (假)
    printf("a != b: %d\n", a != b);  // 1 (真)
    printf("a < b:  %d\n", a < b);   // 1
    printf("a > b:  %d\n", a > b);   // 0
    printf("a <= c: %d\n", a <= c);  // 1
    printf("a >= c: %d\n", a >= c);  // 1

    /* ===== 逻辑运算符 ===== */
    int p = 1, q = 0;

    printf("p && q: %d\n", p && q);  // 0 (逻辑与)
    printf("p || q: %d\n", p || q);  // 1 (逻辑或)
    printf("!p:     %d\n", !p);      // 0 (逻辑非)

    /* ===== 短路求值 ===== */
    int x = 0;

    // 短路行为：若 p 为假，不评估右侧
    if (p && (x = 10)) {
        // x 被赋值为 10
    }

    // 短路行为：若 q 为真，不评估右侧
    if (q || (x = 20)) {
        // x 被赋值为 20
    }

    /* ===== 三目运算符 ===== */
    int max = (a > b) ? a : b;  // 取较大值

    // 嵌套使用（谨慎）
    const char *sign = (a > 0) ? "positive" :
                       (a < 0) ? "negative" : "zero";
}
```

### 赋值与复合运算符

```c
void demonstrate_assignment(void)
{
    int a = 10;

    /* ===== 复合赋值运算符 ===== */
    a += 5;   // a = a + 5;   -> 15
    a -= 3;   // a = a - 3;   -> 12
    a *= 2;   // a = a * 2;   -> 24
    a /= 4;   // a = a / 4;   -> 6
    a %= 4;   // a = a % 4;   -> 2

    a &= 0xFF;
    a |= 0x10;
    a ^= 0x01;
    a <<= 2;
    a >>= 1;

    /* ===== 自增自减运算符 ===== */
    int x = 5;
    int y;

    y = ++x;  // 前缀：先增后用，y = 6, x = 6
    x = 5;
    y = x++;  // 后缀：先用后增，y = 5, x = 6

    // 在表达式中谨慎使用
    int arr[] = {1, 2, 3, 4, 5};
    int i = 0;
    // 避免：未定义行为
    // arr[i++] = arr[i++];  // 错误！

    /* ===== 逗号运算符 ===== */
    int m, n;
    m = (n = 3, n + 2);  // n=3，然后 m=5

    for (int i = 0, j = 10; i < j; i++, j--) {
        // 逗号运算符在 for 循环中常见
    }

    /* ===== sizeof 运算符 ===== */
    printf("sizeof(int) = %zu\n", sizeof(int));
    printf("sizeof a    = %zu\n", sizeof a);    // 可省略括号
    printf("sizeof(1+2) = %zu\n", sizeof(1+2)); // 表达式不求值
}
```

---

## 控制流结构

### 条件语句

```c
#include <stdio.h>
#include <stdlib.h>

void demonstrate_conditional(void)
{
    int score = 85;

    /* ===== if-else 语句 ===== */
    if (score >= 90) {
        printf("Grade: A\n");
    } else if (score >= 80) {
        printf("Grade: B\n");
    } else if (score >= 70) {
        printf("Grade: C\n");
    } else if (score >= 60) {
        printf("Grade: D\n");
    } else {
        printf("Grade: F\n");
    }

    /* ===== switch 语句 ===== */
    enum { RED, YELLOW, GREEN } light = GREEN;

    switch (light) {
        case RED:
            printf("Stop!\n");
            break;
        case YELLOW:
            printf("Caution!\n");
            break;
        case GREEN:
            printf("Go!\n");
            break;
        default:
            printf("Invalid light\n");
            break;
    }

    /* ===== 范围 case (C23 扩展) ===== */
    char grade = 'B';
    switch (grade) {
        case 'A' ... 'D':  // GCC/Clang 扩展，C23 部分支持
            printf("Passed\n");
            break;
        case 'F':
            printf("Failed\n");
            break;
    }

    /* ===== 带标签的 break (C23) ===== */
    // loop1:
    // while (...) {
    //     switch (...) {
    //         case X: break loop1;  // 跳出外层循环
    //     }
    // }

    /* ===== goto 用于错误处理 ===== */
    FILE *fp = fopen("data.txt", "r");
    if (!fp) goto error_open;

    char *buffer = malloc(1024);
    if (!buffer) goto error_malloc;

    // 处理文件...

    free(buffer);
    fclose(fp);
    return;

error_malloc:
    fclose(fp);
error_open:
    perror("Operation failed");
}
```

### 循环结构

```c
void demonstrate_loops(void)
{
    /* ===== while 循环 ===== */
    int i = 0;
    while (i < 10) {
        printf("%d ", i++);
    }
    printf("\n");

    /* ===== do-while 循环 ===== */
    int input;
    do {
        printf("Enter a number (0 to exit): ");
        scanf("%d", &input);
        process_input(input);
    } while (input != 0);

    /* ===== for 循环 ===== */
    // 标准形式
    for (int j = 0; j < 10; j++) {
        printf("%d ", j);
    }

    // 多变量循环
    for (int m = 0, n = 10; m < n; m++, n--) {
        printf("m=%d, n=%d\n", m, n);
    }

    // 省略部分（需谨慎）
    int k = 0;
    for (;;) {  // 无限循环
        if (k++ >= 10) break;
    }

    /* ===== 循环控制 ===== */
    for (int n = 0; n < 100; n++) {
        if (n % 2 == 0) continue;  // 跳过偶数
        if (n > 50) break;          // 提前退出
        printf("%d ", n);
    }

    /* ===== 嵌套循环与标签 ===== */
outer_loop:
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (row == 1 && col == 1) {
                break;  // 仅跳出内层循环
            }
            if (row == 2) {
                goto outer_done;  // 跳出多层
            }
        }
    }
outer_done:
    ;
}

void process_input(int value) {
    (void)value;  // 抑制未使用警告
}
```

---

## 函数基础

### 函数定义与声明

```c
#include <stdio.h>
#include <stdbool.h>

/* ===== 函数声明（原型）===== */
// 编译器需要知道函数的接口才能进行类型检查
int add(int a, int b);
double compute_average(const double *values, size_t count);
void swap(int *a, int *b);
bool is_prime(int n);

/* ===== 函数定义 ===== */

// 值传递：参数副本
int add(int a, int b)
{
    return a + b;
}

// 指针传递：修改调用者数据
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 数组参数：退化为指针
// void process_array(int arr[10]);  // 10 被忽略
// void process_array(int *arr);     // 等价形式
double compute_average(const double *values, size_t count)
{
    if (count == 0) return 0.0;

    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        sum += values[i];
    }
    return sum / count;
}

// 变长数组参数 (C99)
void print_matrix(int rows, int cols, int matrix[rows][cols])
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%4d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// 内联函数建议 (C99 inline)
static inline int max(int a, int b)
{
    return (a > b) ? a : b;
}

// _Noreturn 函数 (C11)
#include <stdlib.h>
_Noreturn void fatal_error(const char *msg)
{
    fprintf(stderr, "Fatal: %s\n", msg);
    exit(EXIT_FAILURE);
}
```

### 递归与函数指针

```c
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
```

### 可变参数函数

```c
#include <stdarg.h>
#include <stdio.h>

// 自定义 printf 风格函数
void debug_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("[DEBUG] ");
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

// 计算平均值（可变参数）
double average(int count, ...)
{
    va_list args;
    va_start(args, count);

    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += va_arg(args, double);
    }

    va_end(args);
    return sum / count;
}

// 使用示例
void demonstrate_varargs(void)
{
    debug_log("Processing item %d with value %f", 42, 3.14);

    double avg = average(5, 1.0, 2.0, 3.0, 4.0, 5.0);
    printf("Average: %f\n", avg);
}
```

---

## 作用域与存储期

```c
#include <stdio.h>

// 文件作用域（外部链接）
int global_variable = 0;

// 文件作用域（内部链接）
static int file_static = 0;

void scope_demo(void)
{
    // 块作用域（自动存储期）
    int local_var = 10;

    // 静态局部变量（静态存储期）
    static int call_count = 0;
    call_count++;

    printf("Function called %d times\n", call_count);

    // 嵌套块作用域
    {
        int inner = 20;
        printf("inner = %d\n", inner);
        // local_var 在此可见
    }
    // inner 在此处不可见
}

// 线程局部存储 (C11)
_Thread_local int thread_data = 0;

void thread_func(void)
{
    thread_data++;  // 每个线程独立副本
}
```

---

## 最佳实践

### 编码规范要点

```c
/*
 * 1. 使用有意义的命名
 * 2. 保持函数简短（<50 行）
 * 3. 避免魔数，使用具名常量
 * 4. 显式处理所有错误情况
 * 5. 优先使用 size_t 进行尺寸计算
 */

// 好的实践示例
#define MAX_BUFFER_SIZE 4096
#define HASH_TABLE_SIZE 1024

enum ErrorCode {
    ERR_OK = 0,
    ERR_INVALID_PARAM = -1,
    ERR_OUT_OF_MEMORY = -2,
    ERR_IO_ERROR = -3
};

size_t safe_strncpy(char *dest, const char *src, size_t n)
{
    if (!dest || !src || n == 0) {
        return 0;
    }

    size_t i;
    for (i = 0; i < n - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';

    return i;
}
```

---

## 延伸阅读

- [核心层 - 指针与内存](../02_Core_Layer/README.md)
- [构造层 - 结构体与联合体](../03_Construction_Layer/README.md)
- [标准库层 - I/O 与字符串](../04_Standard_Library_Layer/README.md)
- [现代 C 编程](../07_Modern_C/README.md)
