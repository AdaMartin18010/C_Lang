# 指针专项练习 (Pointer Exercises)

> **难度**: L2-L4 | **预计时间**: 6-8小时

---

## 练习1: 指针基础操作

### 描述

理解指针的基本概念和操作。

### 任务

1. 编写程序展示以下概念：
   - 指针声明和初始化
   - 取地址和解引用
   - 指针运算
   - void指针和类型转换
   - 多级指针

2. 预测并验证以下代码的输出：

```c
int a = 10;
int *p = &a;
int **pp = &p;
printf("%d %d %d\n", a, *p, **pp);
printf("%p %p %p\n", (void*)&a, (void*)p, (void*)*pp);
```

### 挑战题

```c
// 不使用临时变量交换两个整数
void swap(int *a, int *b);

// 实现类似memmove的功能（处理重叠区域）
void *my_memmove(void *dest, const void *src, size_t n);
```

---

## 练习2: 复杂声明解读

### 描述

掌握C语言复杂的类型声明。

### 任务

解释以下声明的含义：

```c
int *p[10];
int (*p)[10];
int *(*p[5])(int, int);
int (*(*p[5])())[10];
void (*signal(int sig, void (*func)(int)))(int);
```

### 工具

使用 `cdecl` 工具或编写自己的声明解析器。

### 进阶

编写一个程序，将复杂声明转换为英文描述：

```text
输入: int *(*p[5])(int, int)
输出: p is an array of 5 pointers to functions taking (int, int) and returning pointer to int
```

---

## 练习3: 动态内存管理

### 描述

实现一个安全的动态内存分配包装器。

### 要求

```c
typedef struct {
    void *ptr;
    size_t size;
    const char *file;
    int line;
} AllocInfo;

// 实现以下API:
void* safe_malloc(size_t size, const char *file, int line);
void* safe_calloc(size_t nmemb, size_t size, const char *file, int line);
void* safe_realloc(void *ptr, size_t size, const char *file, int line);
void safe_free(void *ptr);
void print_memory_leaks(void);

// 使用宏简化调用
#define SAFE_MALLOC(size) safe_malloc(size, __FILE__, __LINE__)
#define SAFE_FREE(ptr) safe_free(ptr)
```

### 功能

- 跟踪所有分配
- 检测重复释放
- 检测内存泄漏
- 检测越界写入（使用哨兵值）

### 示例输出

```text
Memory Leak Report:
  [main.c:42] 100 bytes not freed
  [utils.c:15] 256 bytes not freed
Total: 356 bytes leaked
```

---

## 练习4: 函数指针与回调

### 描述

使用函数指针实现泛型算法。

### 任务1: 泛型排序

```c
// 实现支持任意类型的快速排序
void generic_qsort(void *base, size_t nmemb, size_t size,
                   int (*compare)(const void *, const void *));

// 使用示例
int compare_int(const void *a, const void *b);
int compare_string(const void *a, const void *b);
```

### 任务2: 状态机

```c
// 使用函数指针实现状态机
typedef struct StateMachine StateMachine;
typedef void (*StateFunc)(StateMachine *);

struct StateMachine {
    StateFunc current_state;
    int data;
};

void state_idle(StateMachine *sm);
void state_running(StateMachine *sm);
void state_paused(StateMachine *sm);
void state_stopped(StateMachine *sm);
```

### 任务3: 事件驱动编程

```c
// 实现简单的事件系统
typedef void (*EventHandler)(void *event_data);

void register_handler(const char *event_type, EventHandler handler);
void trigger_event(const char *event_type, void *event_data);
```

---

## 练习5: 内存池实现

### 描述

实现一个固定大小的内存池分配器。

### 要求

```c
typedef struct MemoryPool MemoryPool;

// 创建指定块大小和数量的内存池
MemoryPool* pool_create(size_t block_size, size_t block_count);

// 从内存池分配
void* pool_alloc(MemoryPool *pool);

// 释放回内存池
void pool_free(MemoryPool *pool, void *ptr);

// 销毁内存池
void pool_destroy(MemoryPool *pool);

// 获取统计信息
size_t pool_available(MemoryPool *pool);
size_t pool_used(MemoryPool *pool);
```

### 实现思路

1. 使用空闲链表管理可用块
2. 每个空闲块的前8字节存储下一个空闲块的指针
3. 分配时从链表头部取出
4. 释放时插入链表头部

### 性能测试

与malloc/free对比：

```text
$ ./pool_benchmark
malloc/free:  1000000 ops in 150ms
pool_alloc:   1000000 ops in 20ms
Speedup: 7.5x
```

---

## 练习6: 字符串处理库

### 描述

实现一个安全的字符串处理库。

### 要求

```c
typedef struct {
    char *data;
    size_t len;
    size_t capacity;
} String;

// 构造和析构
String* string_new(const char *str);
String* string_new_n(const char *str, size_t n);
void string_free(String *s);

// 修改
void string_append(String *s, const char *str);
void string_append_char(String *s, char c);
void string_insert(String *s, size_t pos, const char *str);
void string_erase(String *s, size_t pos, size_t len);
void string_clear(String *s);

// 查询
const char* string_cstr(const String *s);
size_t string_length(const String *s);
int string_compare(const String *a, const String *b);
String* string_substr(const String *s, size_t pos, size_t len);

// 查找
size_t string_find(const String *s, const char *needle);
size_t string_find_char(const String *s, char c);
```

### 安全要求

- 所有操作自动扩容
- 防止缓冲区溢出
- 正确处理空字符串和NULL

---

## 练习7: 链表高级操作

### 描述

实现链表的高级操作。

### 要求

```c
typedef struct ListNode {
    void *data;
    struct ListNode *next;
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *tail;
    size_t size;
    void (*free_func)(void *);
} List;

// 基本操作
List* list_create(void (*free_func)(void *));
void list_destroy(List *list);
void list_push_front(List *list, void *data);
void list_push_back(List *list, void *data);
void* list_pop_front(List *list);
void* list_pop_back(List *list);
void* list_get(const List *list, size_t index);

// 高级操作
void list_sort(List *list, int (*compare)(const void *, const void *));
List* list_reverse(const List *list);
List* list_merge(List *a, List *b, int (*compare)(const void *, const void *));
void list_remove_if(List *list, int (*predicate)(const void *));
void list_foreach(List *list, void (*func)(void *));
```

### 挑战

实现归并排序，时间复杂度O(n log n)，空间复杂度O(1)。

---

## 练习8: 指针调试技巧

### 描述

学习调试指针相关问题的技巧。

### 常见问题与调试

#### 1. 野指针检测

```c
// 使用宏标记已释放指针
#ifdef DEBUG
    #define FREE(ptr) do { free(ptr); ptr = NULL; } while(0)
#else
    #define FREE(ptr) free(ptr)
#endif
```

#### 2. 内存越界检测

```c
// 使用mprotect检测越界
void* protected_alloc(size_t size) {
    // 分配保护页
    // 任何越界访问将触发SIGSEGV
}
```

#### 3. 使用AddressSanitizer

```bash
gcc -fsanitize=address -g program.c -o program
./program
```

### 调试工具清单

- [ ] GDB断点调试
- [ ] Valgrind内存检查
- [ ] AddressSanitizer
- [ ] MemorySanitizer
- [ ] 静态分析工具(cppcheck, clang-analyzer)

---

## 综合项目: 通用容器库

### 描述

实现类似C++ STL的通用容器库。

### 容器列表

1. **Vector**: 动态数组
2. **List**: 双向链表
3. **Map**: 红黑树实现的键值对
4. **Set**: 基于Map的实现
5. **Stack**: 基于Vector或List
6. **Queue**: 基于List

### 设计原则

```c
// 使用宏实现泛型
#define DECLARE_VECTOR(TYPE) \
    typedef struct { \
        TYPE *data; \
        size_t size; \
        size_t capacity; \
    } Vector_##TYPE; \
    \
    Vector_##TYPE* vec_##TYPE##_create(void); \
    void vec_##TYPE##_push(Vector_##TYPE *v, TYPE val); \
    TYPE vec_##TYPE##_get(const Vector_##TYPE *v, size_t index); \
    // ...

// 实例化
DECLARE_VECTOR(int)
DECLARE_VECTOR(double)
DECLARE_VECTOR(char*)
```

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
