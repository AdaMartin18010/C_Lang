# 全局不变式：跨模块通用约束

> **层级定位**: 06_Thinking_Representation > 05_Concept_Mappings
> **用途**: 提取和应用跨模块的通用不变式
> **更新**: 2026-03-24

---

## 全局不变式全景

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    C语言全局不变式网络                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│   空间维度                    时间维度                    逻辑维度           │
│   ┌──────────┐               ┌──────────┐               ┌──────────┐       │
│   │ 内存布局  │◄─────────────►│ 生命周期  │◄─────────────►│ 类型一致  │       │
│   │ 不变式   │               │ 不变式   │               │ 不变式   │       │
│   └────┬─────┘               └────┬─────┘               └────┬─────┘       │
│        │                          │                          │             │
│        │                          │                          │             │
│        └──────────────────────────┼──────────────────────────┘             │
│                                   │                                        │
│                                   ▼                                        │
│                            ┌─────────────┐                                 │
│                            │  程序正确性  │                                 │
│                            │  全局保证    │                                 │
│                            └─────────────┘                                 │
│                                                                              │
│   核心洞察: 不变式是程序状态的"锚点"，任何违反都意味着错误                   │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 一、内存布局不变式

### 1.1 对齐不变式

**不变式 A1**: 任何类型T的对象地址必须是alignof(T)的倍数。

```c
// 不变式表达
∀ obj : T, (uintptr_t)&obj % alignof(T) == 0

// 应用示例
struct Data {
    char c;      // 偏移 0
    // 3字节填充
    int i;       // 偏移 4 (alignof(int) = 4)
    double d;    // 偏移 8 (alignof(double) = 8)
};  // 总大小 16 (8的倍数)
```

**验证代码**:

```c
#include <stdalign.h>
#include <stdint.h>
#include <assert.h>

#define CHECK_ALIGNMENT(ptr, T) \
    assert((uintptr_t)(ptr) % alignof(T) == 0)

void test_alignment() {
    int x;
    CHECK_ALIGNMENT(&x, int);

    double d;
    CHECK_ALIGNMENT(&d, double);

    struct Data data;
    CHECK_ALIGNMENT(&data.i, int);
    CHECK_ALIGNMENT(&data.d, double);
}
```

### 1.2 数组布局不变式

**不变式 A2**: 数组元素在内存中连续存储，间隔为sizeof(T)。

```c
// 不变式表达
∀ arr : T[n], ∀ i : 0 ≤ i < n :
    &arr[i] == (T*)((char*)&arr[0] + i * sizeof(T))

// 推论
&arr[i] - &arr[j] == i - j  (以元素为单位)
```

### 1.3 结构体布局不变式

**不变式 A3**: 结构体成员的偏移量满足对齐要求。

```c
// offsetof 宏基于的不变式
offsetof(struct S, member) % alignof(typeof(member)) == 0

// 成员地址计算
&obj.member == (typeof(member)*)((char*)&obj + offsetof(struct S, member))
```

---

## 二、生命周期不变式

### 2.1 所有权不变式

**不变式 L1**: 每个分配的内存单元有且只有一个所有者。

```c
// 良好: 明确的所有权
void* owner = malloc(size);
// ... 使用 ...
free(owner);  // 所有权释放

// 违反: 双重释放 (两个所有者试图释放)
void* p = malloc(size);
void* alias = p;
free(p);
free(alias);  // 错误: 所有权已释放
```

### 2.2 借用不变式

**不变式 L2**: 借用者（引用/指针）的生命周期不能超过被借用者。

```c
// 良好
void example() {
    int x = 42;      // 被借用者
    int* ref = &x;   // 借用
    *ref = 10;       // 在x的生命周期内使用
}  // x和ref同时销毁

// 违反
int* dangling() {
    int x = 42;
    return &x;  // 错误: 返回指向栈变量的指针
}  // x被销毁，指针悬空
```

### 2.3 资源获取即初始化(RAII)

**不变式 L3**: 资源获取必须有对应的释放，且释放只发生一次。

```c
// 模式: 获取-使用-释放
FILE* f = fopen("file.txt", "r");  // 获取
if (f) {
    // 使用
    fclose(f);  // 释放 (只一次)
}

// C++风格的RAII模拟
typedef struct { FILE* f; } FileGuard;

FileGuard fileguard_open(const char* path) {
    return (FileGuard){ fopen(path, "r") };
}

void fileguard_close(FileGuard* g) {
    if (g->f) {
        fclose(g->f);
        g->f = NULL;  // 防止重复关闭
    }
}
```

---

## 三、类型一致不变式

### 3.1 严格别名不变式

**不变式 T1**: 通过类型T的指针访问的内存，必须最近一次通过兼容类型写入。

```c
// 良好
int x = 42;
int* p = &x;
*p = 10;  // 读写类型一致

// 违反 (严格别名违反)
int i = 0x3F800000;  // IEEE754 1.0f 的位模式
float f = *(float*)&i;  // 未定义行为 (C标准)

// 正确做法: 使用 union
union IntFloat { int i; float f; };
union IntFloat u = { .i = 0x3F800000 };
float f = u.f;  // 合法
```

### 3.2 类型大小不变式

**不变式 T2**: sizeof(T) 在程序执行期间是常量。

```c
// 依赖此不变式的代码
void process_batch(void* data, size_t count, size_t elem_size) {
    // elem_size 可以在运行时确定
    // 但每个元素的大小是固定的
    for (size_t i = 0; i < count; i++) {
        void* elem = (char*)data + i * elem_size;
        // 处理 elem
    }
}
```

### 3.3 指针类型不变式

**不变式 T3**: 指针类型决定了访问的"步长"和解释方式。

```c
char* cp = buffer;
cp++;  // 地址 + 1

int* ip = (int*)buffer;
ip++;  // 地址 + sizeof(int)

// 不变式: ip 和 cp 指向同一地址时，
// *ip 解释 4 个连续的 *cp 值
```

---

## 四、并发不变式

### 4.1 数据竞争自由不变式

**不变式 C1**: 对共享可变内存的并发访问必须有同步保护。

```c
// 共享数据
_Atomic(int) counter = 0;  // 原子类型自然满足

// 或
int shared;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// 良好: 所有访问都加锁
pthread_mutex_lock(&mtx);
shared++;
pthread_mutex_unlock(&mtx);
```

### 4.2 happens-before 关系

**不变式 C2**: 若操作A happens-before 操作B，则A的效果对B可见。

```c
// 通过互斥锁建立 happens-before
// Thread 1:
pthread_mutex_lock(&mtx);
shared = 42;  // A
pthread_mutex_unlock(&mtx);

// Thread 2:
pthread_mutex_lock(&mtx);  // 建立 happens-before
int x = shared;  // B: 保证看到 42
pthread_mutex_unlock(&mtx);
```

### 4.3 原子性不变式

**不变式 C3**: 对原子变量的操作是原子的（不可分割的）。

```c
_Atomic(int) flag = 0;

// 原子读取
int val = atomic_load(&flag);

// 原子写入
atomic_store(&flag, 1);

// 原子读-修改-写
int old = atomic_fetch_add(&counter, 1);
```

---

## 五、控制流不变式

### 5.1 循环不变式

**不变式 F1**: 每次循环迭代开始时，某些条件必须成立。

```c
// 示例: 二分查找
int binary_search(int* arr, int n, int target) {
    int lo = 0, hi = n - 1;

    // 循环不变式:
    // - arr[0..lo-1] < target
    // - arr[hi+1..n-1] > target

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] < target) {
            lo = mid + 1;  // 保持不变式
        } else if (arr[mid] > target) {
            hi = mid - 1;  // 保持不变式
        } else {
            return mid;
        }
    }

    return -1;  // 不变式保证 target 不在数组中
}
```

### 5.2 函数契约不变式

**不变式 F2**: 函数满足前置条件和后置条件。

```c
/**
 * @pre: ptr != NULL
 * @pre: ptr points to valid memory
 * @post: *ptr is incremented by 1
 * @invariant: ptr remains valid
 */
void increment(int* ptr) {
    assert(ptr != NULL);  // 检查前置条件
    (*ptr)++;
    // 后置条件: *ptr 增加了1
}
```

---

## 六、全局不变式应用框架

### 6.1 不变式检查宏

```c
// 调试模式检查
#ifdef DEBUG
    #define INVARIANT(cond) \
        do { \
            if (!(cond)) { \
                fprintf(stderr, "Invariant violated: %s\n", #cond); \
                abort(); \
            } \
        } while(0)
#else
    #define INVARIANT(cond) ((void)0)
#endif

// 应用示例
void* my_alloc(size_t size) {
    void* p = malloc(size);
    INVARIANT(p != NULL || size == 0);  // L1 变体
    return p;
}

void my_free(void* p) {
    INVARIANT(p != NULL);  // 可选的前置条件
    free(p);
}
```

### 6.2 不变式文档化

```c
/**
 * @invariant alignment: (uintptr_t)buffer % alignof(T) == 0
 * @invariant capacity: used <= capacity
 * @invariant ownership: buffer is owned by this structure
 */
typedef struct {
    T* buffer;
    size_t used;
    size_t capacity;
} Vector;
```

---

## 七、不变式违反检测

### 7.1 静态检测

- **编译器警告**: -Wstrict-aliasing, -Wcast-align
- **静态分析器**: 检查潜在的不变式违反
- **类型检查**: 编译时捕获类型错误

### 7.2 动态检测

```c
// 运行时检查
#define CHECK_INVARIANT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "INVARIANT VIOLATION: %s\n", msg); \
            abort(); \
        } \
    } while(0)

// 使用示例
void vector_push(Vector* v, T elem) {
    CHECK_INVARIANT(v->used <= v->capacity, "size overflow");
    // ...
}
```

### 7.3 调试工具

- **AddressSanitizer**: 检测内存不变式违反
- **ThreadSanitizer**: 检测并发不变式违反
- **UBSan**: 检测未定义行为（不变式违反）

---

## 八、不变式网络总结

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     C语言不变式层次                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│   Level 0: 语言级不变式 (由标准保证)                                      │
│   ─────────────────────────────────                                     │
│   • 类型大小恒定    • 对齐要求            • 数组连续存储                  │
│                                                                          │
│   Level 1: 库级不变式 (由实现保证)                                        │
│   ─────────────────────────────────                                     │
│   • malloc对齐      • 线程安全函数          • 标准库契约                  │
│                                                                          │
│   Level 2: 应用级不变式 (由程序员保证)                                    │
│   ─────────────────────────────────                                     │
│   • 数据结构一致性  • 资源管理              • 业务逻辑                    │
│                                                                          │
│   Level 3: 模块级不变式 (由设计保证)                                      │
│   ─────────────────────────────────                                     │
│   • API契约         • 状态机有效性          • 并发协议                    │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
**质量等级**: L5 (理论深化)
