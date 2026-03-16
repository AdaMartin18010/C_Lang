# 调试谜题 (Debugging Puzzles)

> **难度**: L3-L5 | **预计时间**: 15-25小时

---

## 如何使用本章节

每个谜题包含一段有问题的代码。你的任务是：

1. 找出问题（内存泄漏、未定义行为、逻辑错误等）
2. 解释为什么出问题
3. 修复代码

建议使用工具辅助：

- `valgrind` 检测内存问题
- `gdb` 调试
- `clang -fsanitize=address,undefined` 动态检测
- `cppcheck` 静态分析

---

## 谜题1: 微妙的内存泄漏

### 问题代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int age;
} Person;

Person* create_person(const char *name, int age) {
    Person *p = malloc(sizeof(Person));
    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->age = age;
    return p;
}

void free_person(Person *p) {
    free(p);  // 问题在这里！
}

int main() {
    Person *people[100];
    for (int i = 0; i < 100; i++) {
        char name[32];
        sprintf(name, "Person%d", i);
        people[i] = create_person(name, i);
    }

    for (int i = 0; i < 100; i++) {
        free_person(people[i]);
    }

    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

`free_person` 只释放了 `Person` 结构体，但没有释放 `name` 字段指向的内存。

**修复**:

```c
void free_person(Person *p) {
    free(p->name);  // 先释放内部指针
    free(p);        // 再释放结构体
}
```

</details>

---

## 谜题2: 悬垂指针

### 问题代码

```c
#include <stdio.h>

int* get_local(void) {
    int x = 42;
    return &x;  // 返回局部变量地址
}

int main() {
    int *p = get_local();
    printf("%d\n", *p);  // 未定义行为
    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

`x` 是局部变量，函数返回后栈帧被销毁，`p` 指向无效内存。

**修复**:

```c
// 方案1: 使用静态变量
int* get_static(void) {
    static int x = 42;
    return &x;
}

// 方案2: 动态分配
int* get_heap(void) {
    int *x = malloc(sizeof(int));
    *x = 42;
    return x;
}

// 方案3: 通过参数传出
void get_value(int *out) {
    *out = 42;
}
```

</details>

---

## 谜题3: 越界访问

### 问题代码

```c
#include <stdio.h>

int sum_array(int arr[], int n) {
    int sum = 0;
    for (int i = 0; i <= n; i++) {  // 注意这里
        sum += arr[i];
    }
    return sum;
}

int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    printf("Sum: %d\n", sum_array(arr, 5));
    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

循环条件是 `i <= n`，应该是 `i < n`。访问了 `arr[5]`，越界。

**修复**:

```c
for (int i = 0; i < n; i++) {  // 使用 < 而不是 <=
    sum += arr[i];
}
```

</details>

---

## 谜题4: 未初始化的指针

### 问题代码

```c
#include <stdio.h>
#include <stdlib.h>

void append(char *buffer, const char *str) {
    strcat(buffer, str);  // buffer未初始化！
}

int main() {
    char *buffer = malloc(100);
    append(buffer, "Hello");
    append(buffer, " World");
    printf("%s\n", buffer);
    free(buffer);
    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

`malloc` 分配的内存未初始化，`strcat` 从第一个 `\0` 开始追加，但内存内容是随机的。

**修复**:

```c
char *buffer = malloc(100);
if (buffer) {
    buffer[0] = '\0';  // 初始化为空字符串
    // 或使用 calloc
    // char *buffer = calloc(100, 1);
}
```

</details>

---

## 谜题5: 双重释放

### 问题代码

```c
#include <stdlib.h>

void process(int *p) {
    // 一些处理...
    free(p);
}

int main() {
    int *data = malloc(sizeof(int) * 10);

    process(data);

    // 稍后...
    free(data);  // 双重释放！

    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

`data` 在 `process` 中已被释放，`main` 中再次释放导致未定义行为。

**修复**:

```c
// 方案1: 明确所有权
void process(const int *p) {  // 不释放，只读取
    // 只读操作
}

// 方案2: 释放后置NULL
void process(int *p) {
    free(p);
}

int main() {
    int *data = malloc(sizeof(int) * 10);
    process(data);
    data = NULL;  // 避免悬挂指针
    // ...
    free(data);  // 现在安全了（free(NULL)是安全的）
}
```

</details>

---

## 谜题6: 整数溢出

### 问题代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* duplicate(const char *str, int count) {
    int len = strlen(str);
    int total = len * count;  // 可能溢出
    char *result = malloc(total + 1);

    for (int i = 0; i < count; i++) {
        strcat(result, str);  // 未初始化！
    }
    return result;
}

int main() {
    char *result = duplicate("A", 1000000);
    printf("%s\n", result);
    free(result);
    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

1. `len * count` 可能溢出（`1000000 * 1000000` 超过 `int` 范围）
2. `result` 未初始化就使用 `strcat`
3. 没有检查 `malloc` 返回值

**修复**:

```c
#include <stdint.h>
#include <limits.h>

char* duplicate(const char *str, int count) {
    if (count < 0 || str == NULL) return NULL;

    size_t len = strlen(str);

    // 检查溢出
    if (len > 0 && (size_t)count > SIZE_MAX / len) {
        return NULL;  // 溢出
    }

    size_t total = len * (size_t)count;
    char *result = malloc(total + 1);
    if (!result) return NULL;

    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        strcat(result, str);
    }
    return result;
}
```

</details>

---

## 谜题7: 类型混淆

### 问题代码

```c
#include <stdio.h>

void print_values(void *arr, int n, char type) {
    for (int i = 0; i < n; i++) {
        if (type == 'i') {
            printf("%d ", ((int*)arr)[i]);
        } else if (type == 'f') {
            printf("%f ", ((float*)arr)[i]);
        } else if (type == 'd') {
            printf("%lf ", ((double*)arr)[i]);
        }
    }
    printf("\n");
}

int main() {
    double values[] = {1.1, 2.2, 3.3};
    print_values(values, 3, 'f');  // 错误类型！
    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

传入 `double` 数组但指定类型为 `'f'`（`float`），导致类型混淆和错误解析。

**修复**:

```c
// 使用宏或泛型（C11）
#define print_array(arr, n, fmt) do { \
    for (int i = 0; i < (n); i++) { \
        printf(fmt " ", (arr)[i]); \
    } \
    printf("\n"); \
} while(0)

// 或C11泛型选择
#define print_val(x) _Generic((x), \
    int: print_int, \
    float: print_float, \
    double: print_double \
)(x)
```

</details>

---

## 谜题8: 竞争条件

### 问题代码

```c
#include <stdio.h>
#include <pthread.h>

static int counter = 0;

void* increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;  // 非原子操作
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter: %d (expected 200000)\n", counter);
    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

`counter++` 不是原子操作（读取-修改-写入），多线程并发导致竞争条件。

**修复**:

```c
// 方案1: 互斥锁
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 方案2: 原子操作（C11）
#include <stdatomic.h>
static _Atomic int counter = 0;
// counter++ 现在是原子的
```

</details>

---

## 谜题9: 死锁

### 问题代码

```c
#include <pthread.h>

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1(void *arg) {
    pthread_mutex_lock(&m1);
    // 做一些工作...
    pthread_mutex_lock(&m2);  // 等待m2
    // ...
    pthread_mutex_unlock(&m2);
    pthread_mutex_unlock(&m1);
    return NULL;
}

void* thread2(void *arg) {
    pthread_mutex_lock(&m2);
    // 做一些工作...
    pthread_mutex_lock(&m1);  // 等待m1 - 死锁！
    // ...
    pthread_mutex_unlock(&m1);
    pthread_mutex_unlock(&m2);
    return NULL;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

两个线程以不同顺序获取锁，形成循环等待，导致死锁。

**修复**:

```c
// 方案1: 统一加锁顺序
void* thread2(void *arg) {
    pthread_mutex_lock(&m1);  // 先m1
    pthread_mutex_lock(&m2);  // 再m2
    // ...
}

// 方案2: 使用trylock
void* thread2(void *arg) {
    pthread_mutex_lock(&m2);
    if (pthread_mutex_trylock(&m1) != 0) {
        pthread_mutex_unlock(&m2);
        // 重试或返回错误
    }
    // ...
}

// 方案3: 使用层级锁或单个锁
```

</details>

---

## 谜题10: 宏的副作用

### 问题代码

```c
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))

int main() {
    int x = 5, y = 3;
    int m = MAX(x++, y++);  // 副作用！
    printf("m=%d, x=%d, y=%d\n", m, x, y);

    int n = SQUARE(x + 1);  // 预期36，实际？
    printf("n=%d\n", n);

    return 0;
}
```

### 问题

<details>
<summary>点击查看答案</summary>

- `MAX(x++, y++)` 展开后 `x++` 或 `y++` 会被执行两次
- `SQUARE(x + 1)` 展开为 `((x + 1) * (x + 1))`，正确
- 但如果参数有副作用就会出错

**修复**:

```c
// 使用内联函数（C99）或泛型选择（C11）
static inline int max_int(int a, int b) {
    return a > b ? a : b;
}

#define MAX(a, b) _Generic((a), \
    int: max_int, \
    long: max_long, \
    float: max_float, \
    double: max_double \
)(a, b)
```

</details>

---

## 更多调试技巧

### 常用工具命令

```bash
# Valgrind内存检查
valgrind --leak-check=full --show-leak-kinds=all ./program
valgrind --tool=helgrind ./program  # 线程错误

# AddressSanitizer
gcc -fsanitize=address -g program.c -o program

# UndefinedBehaviorSanitizer
gcc -fsanitize=undefined -g program.c -o program

# 静态分析
cppcheck --enable=all --inconclusive program.c
clang-tidy program.c --
```

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)
