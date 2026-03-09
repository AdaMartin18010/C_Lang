# 数组与指针深度解析

> **层级定位**: 01 Core Knowledge System / 02 Core_Layer
> **对应标准**: ISO C §6.5.2.1, §6.7.6.2, §6.3.2.1
> **难度级别**: L3-L5
> **预估学习时间**: 8-10小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 数组内存布局、指针运算、数组-指针关系、多维数组、数组退化 |
| **前置知识** | 基本类型、内存地址概念 |
| **后续延伸** | 动态内存、字符串、函数指针 |
| **权威来源** | ISO C标准, K&R C §5 |

---

## 📖 1. 数组基础

### 1.1 数组定义与初始化

```c
#include <stdio.h>

int main() {
    // 基本数组定义
    int arr1[5];  // 未初始化，值为未定义

    // 初始化方式
    int arr2[5] = {1, 2, 3, 4, 5};  // 完整初始化
    int arr3[5] = {1, 2};  // 部分初始化，其余为0
    int arr4[] = {1, 2, 3, 4, 5};  // 自动推断大小为5
    int arr5[5] = {0};  // 全部初始化为0

    // 指定初始化器（C99）
    int arr6[10] = {[0] = 1, [5] = 5, [9] = 9};
    int arr7[10] = {[1 ... 3] = 1, [7 ... 9] = 2};  // GCC扩展

    // 字符数组
    char str1[] = "Hello";  // 大小为6（包含'\0'）
    char str2[10] = "Hi";   // 剩余空间填充'\0'

    printf("arr4 size: %zu\n", sizeof(arr4) / sizeof(arr4[0]));

    return 0;
}
```

### 1.2 数组内存布局

```c
#include <stdio.h>

void show_memory_layout(void) {
    int arr[5] = {10, 20, 30, 40, 50};

    printf("Array memory layout:\n");
    printf("Address of arr: %p\n", (void*)arr);
    printf("Size of int: %zu\n", sizeof(int));
    printf("\n");

    for (int i = 0; i < 5; i++) {
        printf("arr[%d] at %p, value: %d\n",
               i, (void*)&arr[i], arr[i]);
    }

    // 内存是连续的
    // arr[0] at 0x7fff0000, value: 10
    // arr[1] at 0x7fff0004, value: 20  (+4字节)
    // arr[2] at 0x7fff0008, value: 30  (+4字节)
    // ...
}
```

---

## 📖 2. 指针与数组的关系

### 2.1 数组名是指针常量

```c
#include <stdio.h>

void array_pointer_equivalence(void) {
    int arr[5] = {10, 20, 30, 40, 50};
    int *ptr = arr;  // arr退化为指向首元素的指针

    // 等价访问方式
    printf("arr[2] = %d\n", arr[2]);
    printf("*(arr+2) = %d\n", *(arr + 2));
    printf("ptr[2] = %d\n", ptr[2]);
    printf("*(ptr+2) = %d\n", *(ptr + 2));

    // 关键区别
    // arr是常量，不能修改
    // arr++;  // 错误！

    // ptr是变量，可以修改
    ptr++;  // OK，现在指向arr[1]
    printf("*ptr = %d\n", *ptr);  // 20

    // sizeof的区别
    printf("sizeof(arr) = %zu\n", sizeof(arr));  // 20 (5 * 4)
    printf("sizeof(ptr) = %zu\n", sizeof(ptr));  // 4或8 (指针大小)
}
```

### 2.2 指针运算

```c
#include <stdio.h>

void pointer_arithmetic(void) {
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;

    // 指针加法：加的是"元素个数"，不是字节数
    printf("p = %p\n", (void*)p);
    printf("p + 1 = %p (+%zu bytes)\n", (void*)(p + 1), sizeof(int));

    // 指针减法：计算元素间距
    int *q = &arr[3];
    printf("q - p = %td\n", q - p);  // 3 (3个int的距离)

    // 下标运算的本质
    // a[i] ≡ *(a + i) ≡ *(i + a) ≡ i[a]  (合法但怪异的写法)
    printf("2[arr] = %d\n", 2[arr]);  // 30，等同于arr[2]
}
```

---

## 📖 3. 数组作为函数参数

### 3.1 数组退化为指针

```c
#include <stdio.h>

// 以下三种声明完全等价！
void func1(int arr[], int n) { /* ... */ }
void func2(int arr[10], int n) { /* 10被忽略 */ }
void func3(int *arr, int n) { /* 最准确的写法 */ }

// 数组大小必须在函数内重新计算或传递
void print_array_wrong(int arr[]) {
    // 错误！sizeof(arr)返回指针大小，不是数组大小
    size_t n = sizeof(arr) / sizeof(arr[0]);  // 永远是1或2
    printf("Wrong size: %zu\n", n);
}

void print_array_correct(int arr[], size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// 传递数组指针（保留大小信息）
void process_fixed_array(int (*arr)[10]) {
    // arr是指向int[10]的指针
    for (int i = 0; i < 10; i++) {
        printf("%d ", (*arr)[i]);
    }
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    print_array_correct(arr, n);

    int fixed[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    process_fixed_array(&fixed);  // 传递整个数组的地址

    return 0;
}
```

---

## 📖 4. 多维数组

### 4.1 二维数组内存布局

```c
#include <stdio.h>

void multi_dim_array(void) {
    // 二维数组定义
    int matrix[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };

    // 内存布局是连续的！
    // 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12

    printf("Memory layout:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            printf("matrix[%d][%d] at %p = %d\n",
                   i, j, (void*)&matrix[i][j], matrix[i][j]);
        }
    }

    // 等价访问方式
    printf("\nEquivalence:\n");
    printf("matrix[1][2] = %d\n", matrix[1][2]);      // 7
    printf("*(*(matrix+1)+2) = %d\n", *(*(matrix + 1) + 2));  // 7

    // 类型分析
    // matrix: int[3][4] → 退化为 int (*)[4]（指向4个int的指针）
    // matrix[i]: int[4] → 退化为 int*（指向int的指针）
}
```

### 4.2 多维数组作为参数

```c
#include <stdio.h>

// 必须指定除第一维外的所有维度
void process_2d(int matrix[][4], int rows) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// 使用指针语法（等价）
void process_2d_alt(int (*matrix)[4], int rows) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// 完全动态：使用指针的指针（需要额外分配）
void process_dynamic(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int mat[3][4] = {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}};
    process_2d(mat, 3);
    return 0;
}
```

---

## 📖 5. 动态数组

```c
#include <stdio.h>
#include <stdlib.h>

// 一维动态数组
int* create_1d_array(size_t n) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;

    for (size_t i = 0; i < n; i++) {
        arr[i] = i;
    }
    return arr;
}

// 二维动态数组（连续内存）
int* create_2d_contiguous(size_t rows, size_t cols) {
    int *arr = malloc(rows * cols * sizeof(int));
    // 访问: arr[i * cols + j]
    return arr;
}

// 二维动态数组（指针数组）
int** create_2d_pointer_array(size_t rows, size_t cols) {
    int **arr = malloc(rows * sizeof(int*));
    for (size_t i = 0; i < rows; i++) {
        arr[i] = malloc(cols * sizeof(int));
    }
    return arr;
}

void free_2d_pointer_array(int **arr, size_t rows) {
    for (size_t i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);
}

int main() {
    // 使用示例
    int *arr1d = create_1d_array(10);
    free(arr1d);

    int **arr2d = create_2d_pointer_array(3, 4);
    arr2d[1][2] = 100;  // 使用
    free_2d_pointer_array(arr2d, 3);

    return 0;
}
```

---

## ⚠️ 常见陷阱

### 陷阱 ARR01: 缓冲区溢出

```c
int arr[10];
for (int i = 0; i <= 10; i++) {  // 错误：i=10越界！
    arr[i] = i;
}
```

### 陷阱 ARR02: 数组大小在函数内丢失

```c
void process(int arr[]) {
    // sizeof(arr) 返回指针大小，不是数组大小
    size_t n = sizeof(arr) / sizeof(arr[0]);  // 错误！
}

// 正确做法：显式传递大小
void process_correct(int arr[], size_t n) {
    // 使用n
}
```

### 陷阱 ARR03: 二维数组与指针的指针混淆

```c
int matrix[3][4];
int **ptr = matrix;  // 错误！类型不匹配

// matrix的类型是 int (*)[4]
// ptr的类型是 int **
// 它们是不同的！
```

---

## ✅ 质量验收清单

- [x] 数组定义与初始化
- [x] 数组内存布局可视化
- [x] 数组名退化为指针
- [x] 指针运算详解
- [x] 数组作为函数参数
- [x] 多维数组内存布局
- [x] 动态数组分配
- [x] 常见陷阱分析

---

> **更新记录**
>
> - 2025-03-09: 创建，补充核心缺失主题
