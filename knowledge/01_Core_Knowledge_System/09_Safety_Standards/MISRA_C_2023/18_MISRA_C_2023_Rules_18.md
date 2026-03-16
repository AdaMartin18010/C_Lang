# MISRA C:2023 Rules 18.1-18.6 - 指针和数组

> 数组访问、指针运算、字符串处理

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 18.1 | 数组边界 | Required | 防止数组越界 |
| Rule 18.2 | 指针减法 | Required | 指针减法安全 |
| Rule 18.3 | 关系运算 | Required | 指针关系运算限制 |
| Rule 18.4 | 字符串 | Required | 字符串操作安全 |
| Rule 18.5 | 数组声明 | Required | 数组大小明确 |
| Rule 18.6 | 可变修改类型 | Required | 避免VLA和VM类型 |

---

## Rule 18.1 - 数组边界

### 规则原文

> **Rule 18.1 (Required)**: Array access shall be within the bounds of the array.
>
> **中文**: 数组访问应在数组边界内。

### 违反示例

```c
/* ❌ 违反 - 越界访问 */
int arr[10];
int x = arr[10];  /* 越界！有效索引0-9 */

/* ❌ 违反 - 负数索引 */
int arr[10];
int x = arr[-1];  /* 未定义行为 */

/* ❌ 违反 - 用户输入未验证 */
void process(int index)
{
    int arr[10];
    arr[index] = 42;  /* 可能越界 */
}

/* ❌ 违反 - 循环边界错误 */
for (int i = 0; i <= 10; i++) {  /* 应该是 < 10 */
    arr[i] = 0;  /* i=10时越界 */
}

/* ❌ 违反 - 字符串越界 */
char str[5] = "hello";  /* 需要6字节（含'\0'）*/
```

### 合规示例

```c
/* ✅ 合规 - 边界检查 */
#define ARRAY_SIZE 10
int arr[ARRAY_SIZE];

void safe_access(size_t index)
{
    if (index >= ARRAY_SIZE) {
        /* 错误处理 */
        return;
    }
    arr[index] = 42;
}

/* ✅ 合规 - 使用安全函数 */
void safe_copy(char *dest, size_t dest_size, const char *src)
{
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

/* ✅ 合规 - 循环边界正确 */
for (int i = 0; i < ARRAY_SIZE; i++) {  /* < 不是 <= */
    arr[i] = 0;
}

/* ✅ 合规 - 使用sizeof计算大小 */
int arr[10];
size_t count = sizeof(arr) / sizeof(arr[0]);
for (size_t i = 0; i < count; i++) {
    arr[i] = 0;
}
```

---

## Rule 18.2 - 指针减法

### 规则原文

> **Rule 18.2 (Required)**: Subtraction between pointers shall only be applied to pointers that address elements of the same array.
>
> **中文**: 指针减法只能应用于指向同一数组元素的指针。

### 违反示例

```c
/* ❌ 违反 - 不同数组 */
int arr1[10], arr2[10];
int diff = &arr1[5] - &arr2[5];  /* 未定义行为 */

/* ❌ 违反 - 对象和其后位置 */
int x, y;
size_t diff = &y - &x;  /* 未定义行为 */
```

### 合规示例

```c
/* ✅ 合规 - 同一数组内的指针减法 */
int arr[100];
int *p1 = &arr[10];
int *p2 = &arr[50];
ptrdiff_t diff = p2 - p1;  /* 40 */

/* ✅ 合规 - 计算数组元素个数 */
size_t count_elements(const int *start, const int *end)
{
    return (size_t)(end - start);  /* 同一数组 */
}

/* ✅ 合规 - 字符串长度计算 */
size_t my_strlen(const char *str)
{
    const char *end = str;
    while (*end != '\0') {
        end++;
    }
    return (size_t)(end - str);  /* 同一字符串 */
}
```

---

## Rule 18.3 - 指针关系运算

### 规则原文

> **Rule 18.3 (Required)**: The relational operators shall not be used with pointers to different arrays.
>
> **中文**: 关系运算符不得用于指向不同数组的指针。

### 违反示例

```c
/* ❌ 违反 - 不同数组比较 */
int arr1[10], arr2[10];
if (&arr1[0] < &arr2[0]) {  /* 未定义行为 */ }

/* ❌ 违反 - 对象地址比较 */
int x, y;
if (&x < &y) {  /* 未定义行为 */ }
```

### 合规示例

```c
/* ✅ 合规 - 同一数组内比较 */
int arr[100];
int *p = find_element(arr, 100, target);
if (p != NULL && p >= &arr[0] && p < &arr[100]) {
    /* p在有效范围内 */
}

/* ✅ 合规 - NULL检查 */
int *p = get_pointer();
if (p != NULL) {
    use(p);
}

/* ✅ 合规 - 使用索引而非指针比较 */
size_t index1 = 10;
size_t index2 = 20;
if (index1 < index2) {  /* 安全 */ }
```

---

## Rule 18.4 - 字符串安全

### 规则原文

> **Rule 18.4 (Required)**: String operations shall be performed safely.
>
> **中文**: 字符串操作应安全执行。

### 安全字符串模式

```c
/* ✅ 安全字符串复制 */
#include <string.h>

void safe_strcpy(char *dest, size_t dest_size, const char *src)
{
    if (dest == NULL || src == NULL || dest_size == 0) {
        return;
    }

    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';  /* 确保终止 */
}

/* ✅ 安全字符串连接 */
void safe_strcat(char *dest, size_t dest_size, const char *src)
{
    if (dest == NULL || src == NULL || dest_size == 0) {
        return;
    }

    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size - 1) {
        return;  /* 已满 */
    }

    strncat(dest, src, dest_size - dest_len - 1);
}

/* ✅ 格式化字符串 */
void safe_format(char *buf, size_t buf_size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, buf_size, fmt, args);
    va_end(args);

    buf[buf_size - 1] = '\0';  /* 确保终止 */
}

/* ✅ 使用安全函数 */
#include <stdio.h>

void read_line_safe(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) != NULL) {
        /* 移除可能的换行符 */
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}
```

---

## Rule 18.5 - 数组声明

### 规则原文

> **Rule 18.5 (Required)**: The size of an array shall be declared explicitly.
>
> **中文**: 数组大小应显式声明。

### 合规示例

```c
/* ✅ 合规 - 显式大小 */
int arr1[10];
char str[256];
uint8_t buffer[1024];

/* ✅ 合规 - 从初始化推导大小 */
int arr2[] = {1, 2, 3, 4, 5};  /* 大小为5 */
char msg[] = "hello";  /* 大小为6（含'\0'）*/

/* ✅ 合规 - 使用宏 */
#define MAX_ITEMS 100
struct Item items[MAX_ITEMS];

/* ✅ 合规 - 枚举作为大小 */
enum { NUM_SENSORS = 8 };
float sensor_data[NUM_SENSORS];

/* ✅ 合规 - 外部数组声明 */
extern int global_array[];  /* 外部定义确定大小 */
```

---

## Rule 18.6 - 可变修改类型

### 规则原文

> **Rule 18.6 (Required)**: Variable Length Arrays (VLA) shall not be used.
>
> **中文**: 不得使用变长数组（VLA）。

### VLA问题

```c
/* ❌ 违反 - VLA在栈上 */
void process(int n)
{
    int arr[n];  /* VLA - 栈溢出风险 */
    /* ... */
}

/* ❌ 违反 - VLA作为参数 */
void func(int n, int arr[n]);  /* VLA参数 */

/* ❌ 违反 - 二维VLA */
void matrix(int rows, int cols, int m[rows][cols]);
```

### 替代方案

```c
/* ✅ 合规 - 固定大小 */
#define MAX_SIZE 1000
void process_fixed(void)
{
    int arr[MAX_SIZE];  /* 固定大小 */
}

/* ✅ 合规 - 动态分配 */
void process_dynamic(int n)
{
    int *arr = malloc(n * sizeof(int));
    if (arr == NULL) {
        /* 错误处理 */
        return;
    }
    /* 使用arr */
    free(arr);
}

/* ✅ 合规 - 内存池 */
void *pool_alloc(size_t size);
void pool_free(void *ptr);

void process_pool(int n)
{
    int *arr = pool_alloc(n * sizeof(int));
    if (arr == NULL) {
        return;
    }
    /* 使用arr */
    pool_free(arr);
}

/* ✅ 合规 - 柔性数组 */
struct Buffer {
    size_t size;
    uint8_t data[];
};

struct Buffer *create_buffer(size_t size)
{
    struct Buffer *b = malloc(sizeof(struct Buffer) + size);
    if (b) {
        b->size = size;
    }
    return b;
}
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 18
