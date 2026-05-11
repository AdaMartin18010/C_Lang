# 所有权语义与生命周期文档化

> **标准**: ISO/IEC 9899:2024 (C23), MISRA C:2023, CERT C  
> **前置知识**: [Arena分配器](00_Arena_Allocators.md), [抽象状态机](../01_Basic_Layer/00_Abstract_State_Machine.md)  
> **后续延伸**: [MISRA C安全标准](../09_Safety_Standards/MISRA_C_2023/README.md), [Rust互操作](../../03_System_Technology_Domains/32_Rust_Interoperability/README.md)  
> **对标资源**: Rust Ownership, GSL (Guidelines Support Library), MISRA C:2012 Rule 22.x, CERT C EXP00-C  
> **难度**: 4/5 | **预计学习时间**: 3-4 小时  

---

## 目录

1. [为什么C需要所有权概念](#一为什么需要所有权)
2. [核心所有权模型](#二核心模型)
3. [所有权模式](#三所有权模式)
4. [生命周期标注](#四生命周期标注)
5. [Borrow检查器的C模拟](#五borrow检查器)
6. [与MISRA/CERT的映射](#六安全标准映射)
7. [实战模式](#七实战模式)
8. [工具支持](#八工具支持)

---

## 一、为什么需要所有权

### 1.1 C语言内存安全的根本问题

C语言没有内置的内存安全机制，导致：

| 问题类型 | 占比 | 典型原因 |
|:---------|:-----|:---------|
| 缓冲区溢出 | ~70% | 缺失长度检查、越界访问 |
| Use-after-free | ~15% | 释放后继续使用指针 |
| Double-free | ~5% | 同一内存释放两次 |
| 内存泄漏 | ~10% | 分配后未释放 |

> **核心洞察**：这些问题的根本原因是**缺乏对指针生命周期和所有权的形式化约定**。

### 1.2 所有权的定义

**所有权（Ownership）**：确定谁负责分配、谁负责释放、以及资源在何时可以被安全访问的约定。

```
所有权三元组
├── 谁分配（Allocator）
├── 谁释放（Deallocator）
└── 有效期（Lifetime）
```

---

## 二、核心所有权模型

### 2.1 三种基本所有权

| 所有权类型 | 语义 | 责任 |
|:-----------|:-----|:-----|
| **Owned（拥有）** | 独占资源，负责释放 | 分配者必须释放；转移后原指针失效 |
| **Borrowed（借用）** | 临时访问，不拥有 | 不得释放；必须在所有者有效期内使用 |
| **Shared（共享）** | 共同访问，引用计数 | 引用计数管理生命周期；最后使用者释放 |

```c
// Owned指针
char* owned_buffer = malloc(1024);  // 我拥有这块内存
// ... 使用 ...
free(owned_buffer);                  // 我必须释放它
owned_buffer = NULL;                 // 释放后置空（防御性编程）

// Borrowed指针
void print_borrowed(const char* msg) {  // 借用：const表示只读
    printf("%s\n", msg);                 // 不释放、不修改
}                                        // 调用者仍拥有msg

// Shared指针（引用计数）
typedef struct {
    void* data;
    int refcount;
} SharedPtr;

SharedPtr* shared_acquire(SharedPtr* p) {
    if (p) p->refcount++;
    return p;
}

void shared_release(SharedPtr* p) {
    if (p && --p->refcount == 0) {
        free(p->data);
        free(p);
    }
}
```

### 2.2 所有权转移

```c
// 所有权转移：sink函数接收所有权
void buffer_sink(char* buffer) {  // 接收所有权
    // ... 使用buffer ...
    free(buffer);  // 负责释放
}

// 调用者转移所有权
char* buf = malloc(1024);
buffer_sink(buf);   // 所有权转移给buffer_sink
// buf 现在不可使用！
buf = NULL;         // 防御性置空

// 更好的方式：显式转移语义
char* buffer_take(char** buffer) {  // 从原所有者取出
    char* p = *buffer;
    *buffer = NULL;  // 原所有者失去所有权
    return p;
}
```

---

## 三、所有权模式

### 3.1 Copy-on-Write（COW）

```c
typedef struct {
    char* data;
    size_t len;
    int refcount;
    bool owns;  // 是否拥有data
} String;

String* string_clone(const String* src) {
    String* dst = malloc(sizeof(String));
    if (src->refcount > 1 || !src->owns) {
        // 共享引用
        dst->data = src->data;
        dst->refcount = ++((String*)src)->refcount;
        dst->owns = false;
    } else {
        // 独占，直接复制
        dst->data = malloc(src->len + 1);
        memcpy(dst->data, src->data, src->len + 1);
        dst->refcount = 1;
        dst->owns = true;
    }
    dst->len = src->len;
    return dst;
}

void string_mutate(String* s, size_t index, char c) {
    if (!s->owns && s->refcount > 1) {
        // Copy-on-Write: 先复制再修改
        char* new_data = malloc(s->len + 1);
        memcpy(new_data, s->data, s->len + 1);
        s->data = new_data;
        s->owns = true;
        s->refcount = 1;
    }
    s->data[index] = c;
}
```

### 3.2 RAII模式

```c
// 使用GNU cleanup属性实现RAII
#ifdef __GNUC__
#define RAII(type, name, init, cleanup_fn) \
    type name __attribute__((cleanup(cleanup_fn))) = init

static inline void auto_free(char** p) { free(*p); }
static inline void auto_fclose(FILE** fp) { if (*fp) fclose(*fp); }
static inline void auto_close(int* fd) { if (*fd >= 0) close(*fd); }

// 使用
void process_file(const char* path) {
    RAII(FILE*, fp, fopen(path, "r"), auto_fclose);
    if (!fp) return;
    
    RAII(char*, buffer, malloc(1024), auto_free);
    
    // ... 使用fp和buffer ...
    // 函数返回时自动释放
}
#endif
```

### 3.3 唯一所有权（Unique Ownership）

```c
typedef struct {
    void* ptr;
    bool valid;
} UniquePtr;

UniquePtr unique_new(size_t size) {
    return (UniquePtr){malloc(size), true};
}

void* unique_take(UniquePtr* u) {
    assert(u->valid);
    void* p = u->ptr;
    u->ptr = NULL;
    u->valid = false;
    return p;
}

void unique_free(UniquePtr* u) {
    if (u->valid) {
        free(u->ptr);
        u->ptr = NULL;
        u->valid = false;
    }
}
```

---

## 四、生命周期标注

### 4.1 手动生命周期文档化

```c
/**
 * @brief 打开配置文件
 * @param path 配置文件路径（借用，调用者拥有）
 * @param buffer 输出缓冲区（借用，必须在函数调用期间有效）
 * @param buffer_size 缓冲区大小
 * @return 读取的字节数，或错误码
 * @lifetime path: 调用期间借用
 * @lifetime buffer: 调用期间借用，函数返回后仍有效
 * @ownership 不转移任何所有权
 */
int read_config(const char* path, char* buffer, size_t buffer_size);

/**
 * @brief 创建新的资源
 * @param size 资源大小
 * @return 指向新资源的指针（拥有，调用者负责释放）
 * @ownership 返回Owned指针，调用者必须使用resource_free释放
 */
Resource* resource_create(size_t size);

/**
 * @brief 释放资源
 * @param res 要释放的资源（拥有，函数后指针失效）
 * @ownership 接收Owned指针，释放后资源不可再用
 */
void resource_free(Resource* res);
```

### 4.2 GSL风格标注（微软指南支持库）

```c
// GSL-inspired annotations
#define GSL_OWNER __attribute__((annotate("gsl::owner")))
#define GSL_POINTER __attribute__((annotate("gsl::pointer")))
#define GSL_BORROWED __attribute__((annotate("gsl::borrowed")))
#define GSL_NOTNULL __attribute__((annotate("gsl::not_null")))

// 使用标注
GSL_OWNER int* create_array(size_t n);           // 拥有者
GSL_BORROWED int* get_element(Array* a, size_t i); // 借用
void destroy_array(GSL_OWNER int* arr);           // 接收拥有者

size_t strlen(GSL_NOTNULL const char* s);         // 非空指针
```

---

## 五、Borrow检查器的C模拟

### 5.1 编译时检查（使用宏和静态分析）

```c
// 借用检查宏
#define BORROW(ptr) (assert((ptr) != NULL), (ptr))
#define BORROW_MUT(ptr) (assert((ptr) != NULL), (ptr))
#define MOVE(ptr) ({ typeof(ptr) _p = (ptr); (ptr) = NULL; _p; })

// 使用
void example(void) {
    int* owned = malloc(sizeof(int));
    *owned = 42;
    
    int* borrowed = BORROW(owned);  // 借用
    printf("%d\n", *borrowed);
    
    int* moved = MOVE(owned);       // 转移所有权
    // owned 现在为 NULL
    
    free(moved);  // 正确：释放moved
    // free(owned);  // 安全：owned为NULL，free(NULL)是no-op
}
```

### 5.2 运行时空闲列表检测

```c
typedef struct {
    void* ptr;
    bool freed;
    const char* alloc_file;
    int alloc_line;
} TrackedPtr;

static TrackedPtr tracked_pointers[10000];
static int tracked_count = 0;

void* tracked_malloc(size_t size, const char* file, int line) {
    void* p = malloc(size);
    tracked_pointers[tracked_count++] = (TrackedPtr){p, false, file, line};
    return p;
}

void tracked_free(void* p, const char* file, int line) {
    for (int i = 0; i < tracked_count; i++) {
        if (tracked_pointers[i].ptr == p) {
            if (tracked_pointers[i].freed) {
                fprintf(stderr, "Double-free at %s:%d (allocated at %s:%d)\n",
                        file, line, tracked_pointers[i].alloc_file, tracked_pointers[i].alloc_line);
                abort();
            }
            tracked_pointers[i].freed = true;
            free(p);
            return;
        }
    }
    fprintf(stderr, "Free of untracked pointer at %s:%d\n", file, line);
    abort();
}

#define SAFE_MALLOC(size) tracked_malloc(size, __FILE__, __LINE__)
#define SAFE_FREE(ptr) tracked_free(ptr, __FILE__, __LINE__)
```

---

## 六、与MISRA/CERT的映射

### 6.1 MISRA C:2023 相关规则

| MISRA规则 | 主题 | 所有权映射 |
|:----------|:-----|:-----------|
| Rule 22.1 | 分配的内存必须释放 | Owned指针必须对应free |
| Rule 22.2 | 只释放分配的内存 | 不得free Borrowed指针 |
| Rule 22.3 | 不赋值重叠对象 | Borrowed指针的有效性检查 |
| Rule 22.4 | 不使用已释放内存 | 释放后置空（Move语义）|
| Rule 22.5 | 不多次释放 | 唯一所有权检查 |
| Rule 22.6 | 指针使用前要检查 | Borrow指针的非空检查 |

### 6.2 CERT C相关规则

| CERT规则 | 主题 |
|:---------|:-----|
| EXP00-C | 使用正确的表达式求值顺序 |
| EXP34-C | 不要解引用空指针 |
| MEM00-C | 分配和释放内存要在同一抽象层次 |
| MEM01-C | 释放后立即置空指针 |
| MEM30-C | 不要访问已释放内存 |
| MEM31-C | 释放动态分配的内存 |

---

## 七、实战模式

### 7.1 所有权检查清单

```markdown
## 函数设计所有权检查清单

- [ ] 每个指针参数是否有明确的所有权语义？
  - [ ] Owned: 函数是否负责释放？
  - [ ] Borrowed: 调用者是否确保生命周期？
  - [ ] Shared: 引用计数是否正确管理？

- [ ] 返回的指针所有权是否文档化？
  - [ ] Owned: 调用者必须释放
  - [ ] Borrowed: 返回值的生命周期关联到什么对象？

- [ ] 是否有悬垂指针风险？
  - [ ] Borrowed指针指向的对象是否在函数返回后仍有效？

- [ ] 错误路径是否正确处理所有权？
  - [ ] 部分初始化时是否释放已分配资源？
```

### 7.2 典型API设计模式

```c
// 模式1: 工厂模式（返回Owned）
Database* db_open(const char* path);   // 返回Owned
db_close(Database* db);                // 接收Owned

// 模式2: 迭代器模式（返回Borrowed）
const Record* db_next(Database* db);   // 返回Borrowed（db的生命周期内有效）

// 模式3: 输出参数（调用者提供Owned缓冲区）
int db_read(Database* db, char* buffer, size_t size);  // buffer是Borrowed

// 模式4: 转移模式
char* db_take_error(Database* db);     // 转移：db失去错误字符串的所有权
```

---

## 八、工具支持

### 8.1 静态分析工具

| 工具 | 所有权检查能力 |
|:-----|:---------------|
| **Clang Static Analyzer** | 基础内存泄漏检测、Use-after-free检测 |
| **Infer (Facebook)** | 高级的内存错误检测，支持所有权推理 |
| **Frama-C** | 形式化验证，可证明无内存错误 |
| **Coverity** | 商业级所有权违规检测 |
| **CodeSonar** | 深层静态分析，支持自定义所有权规则 |

### 8.2 动态检测

```bash
# AddressSanitizer (检测Use-after-free, Double-free, Buffer overflow)
gcc -fsanitize=address -g source.c -o program
./program

# MemorySanitizer (检测未初始化内存使用)
gcc -fsanitize=memory -g source.c -o program

# UndefinedBehaviorSanitizer
gcc -fsanitize=undefined -g source.c -o program
```

### 8.3 GSL实现

微软的Guidelines Support Library提供了C++实现，但理念可直接应用于C：

```c
// GSL概念在C中的简化实现
typedef struct {
    void* ptr;
    void (*destructor)(void*);
} Owner;

Owner owner_new(size_t size, void (*dtor)(void*)) {
    return (Owner){malloc(size), dtor};
}

void owner_release(Owner* o) {
    if (o->ptr) {
        if (o->destructor) o->destructor(o->ptr);
        else free(o->ptr);
        o->ptr = NULL;
    }
}
```

---

## 参考资源

1. **The Rust Programming Language** - Ownership章节（概念源头）
2. **C++ Core Guidelines** - Bjarne Stroustrup, Herb Sutter
3. **GSL (Guidelines Support Library)** - github.com/microsoft/GSL
4. **MISRA C:2023** - 内存管理相关规则（Rule 22.x）
5. **CERT C Secure Coding Standard** - 内存安全章节
6. **Ownership and Borrowing in C** - 现代C社区最佳实践

---

> 核心总结：C语言虽然没有内置所有权系统，但通过编码约定、文档化、静态标注和工具支持，可以建立有效的所有权语义体系。这是从"能编译的C代码"进化到"工业级安全C代码"的关键一步。所有权思维不仅能防止内存错误，还能使API设计更清晰、更易于维护。

---

*文档状态: 初版完成 | 对齐: Rust Ownership, GSL, MISRA C:2023, CERT C | 最后更新: 2026-05-11*
