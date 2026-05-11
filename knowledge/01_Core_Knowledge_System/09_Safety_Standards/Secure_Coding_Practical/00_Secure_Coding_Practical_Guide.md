# C 语言安全编码实战指南

> **难度**: L3-L5
> **威胁模型**: 外部输入不可信、内存损坏、并发竞争
> **对应标准**: CERT C, CWE Top 25, OWASP

---

## 1. 缓冲区溢出防护

### 1.1 栈溢出

```c
// ❌ 漏洞：固定大小缓冲区，无边界检查
void process_name(const char *input) {
    char buf[64];
    strcpy(buf, input);   // 如果 input > 63 字节 → 栈溢出
}

// ✅ 安全：使用带长度限制的函数
void process_name_safe(const char *input) {
    char buf[64];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';  // 确保终止
}

// ✅ 更安全：显式检查长度
void process_name_better(const char *input) {
    if (strlen(input) >= 64) {
        log_error("Input too long");
        return;
    }
    char buf[64];
    memcpy(buf, input, strlen(input) + 1);
}
```

### 1.2 堆溢出

```c
// ❌ 漏洞：大小计算错误
char *dup_string_bad(const char *src) {
    size_t len = strlen(src);
    char *dst = malloc(len);      // 少了 +1 ！
    strcpy(dst, src);              // 溢出1字节
    return dst;
}

// ✅ 安全
char *dup_string_safe(const char *src) {
    if (!src) return NULL;
    size_t len = strlen(src);
    if (len == SIZE_MAX) return NULL;  // 防止溢出
    char *dst = malloc(len + 1);
    if (!dst) return NULL;
    memcpy(dst, src, len + 1);
    return dst;
}
```

### 1.3 格式化字符串漏洞

```c
// ❌ 极度危险：用户输入作为格式字符串
void log_user_input(const char *user_input) {
    printf(user_input);   // 攻击者输入 "%s%s%s" → 崩溃或信息泄露
}

// ✅ 安全：始终使用常量格式字符串
void log_user_input_safe(const char *user_input) {
    printf("%s", user_input);   // 输入被当作数据，不是格式
}

// ❌ 同样危险
char buf[256];
sprintf(buf, user_input);   // 可能写入任意地址

// ✅ 安全
snprintf(buf, sizeof(buf), "%s", user_input);
```

---

## 2. 整数安全

### 2.1 整数溢出

```c
// ❌ 漏洞：乘法溢出 → 分配过小内存
void *alloc_array_bad(size_t n, size_t size) {
    return malloc(n * size);   // n * size 可能溢出！
}

// ✅ 安全：溢出检查（C23 使用 ckd_mul）
void *alloc_array_safe(size_t n, size_t size) {
    if (n == 0 || size == 0) return NULL;
    if (n > SIZE_MAX / size) {   // 溢出检查
        errno = ENOMEM;
        return NULL;
    }
    return malloc(n * size);
}

// 或包装函数
#define SAFE_MUL(result, a, b) \
    ((a) != 0 && (b) > SIZE_MAX / (a) ? 0 : ((result) = (a) * (b), 1))
```

### 2.2 有符号/无符号混淆

```c
// ❌ 漏洞：有符号比较无符号
int read_data(int sockfd, char *buf, size_t len) {
    int n = recv(sockfd, buf, len, 0);
    if (n < len) {   // 如果 len > INT_MAX，比较错误！
        // ...
    }
    return n;
}

// ✅ 安全：统一使用有符号或无 signed
ssize_t read_data_safe(int sockfd, char *buf, size_t len) {
    ssize_t n = recv(sockfd, buf, len, 0);
    if (n < 0) return -1;        // 错误
    if ((size_t)n < len) {       // 显式转换后比较
        // 短读取处理
    }
    return n;
}
```

---

## 3. 内存管理安全

### 3.1 Use-After-Free (UAF)

```c
// ❌ 漏洞：释放后使用
void process_item(Item *item) {
    free(item->data);
    if (item->data != NULL) {    // 释放后检查无意义（可能为假阳性）
        item->data[0] = 0;       // UAF！
    }
}

// ✅ 安全：释放后立即置 NULL
void process_item_safe(Item *item) {
    free(item->data);
    item->data = NULL;           // 防止后续误用
}

// ✅ 更好：统一封装
void safe_free(void **pp) {
    if (pp && *pp) {
        free(*pp);
        *pp = NULL;
    }
}
// 使用
safe_free((void**)&item->data);
```

### 3.2 Double-Free

```c
// ❌ 漏洞：错误路径重复释放
void parse_buffer(char *buf) {
    char *copy = strdup(buf);
    if (!validate(copy)) {
        free(copy);       // 第一次释放
        return;
    }
    process(copy);
    free(copy);           // 正常释放
}

// 如果 validate 内部出错调用 longjmp → copy 泄漏
// 如果 process 也释放了 copy → double-free

// ✅ 安全：单一所有者，单一释放点
typedef struct { char *data; } Buffer;

Buffer *buffer_create(const char *src) {
    Buffer *b = malloc(sizeof(Buffer));
    b->data = strdup(src);
    return b;
}

void buffer_destroy(Buffer *b) {
    if (b) {
        free(b->data);
        free(b);
    }
}
```

---

## 4. 并发安全

### 4.1 数据竞争

```c
// ❌ 漏洞：无锁访问共享变量
int counter = 0;

void *thread_func(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;   // 非原子，数据竞争
    }
    return NULL;
}

// ✅ 安全：使用原子操作
_Atomic int counter_safe = 0;

void *thread_func_safe(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        atomic_fetch_add(&counter_safe, 1);
    }
    return NULL;
}

// ✅ 或互斥锁
typedef struct { mtx_t lock; int count; } SafeCounter;

void increment(SafeCounter *c) {
    mtx_lock(&c->lock);
    c->count++;
    mtx_unlock(&c->lock);
}
```

### 4.2 TOCTOU（检查时间到使用时间）

```c
// ❌ 漏洞：检查和使用之间文件可能被替换
if (access("/tmp/data", R_OK) == 0) {   // 检查
    FILE *fp = fopen("/tmp/data", "r");  // 使用：文件可能已被替换！
}

// ✅ 安全：原子操作，或使用文件描述符
int fd = open("/tmp/data", O_RDONLY);
if (fd >= 0) {
    // 通过 fd 操作，文件已被锁定
    FILE *fp = fdopen(fd, "r");
    // ...
}
```

---

## 5. 输入验证

### 5.1 白名单验证

```c
// ❌ 黑名单：总是不完整
if (strchr(input, ';') == NULL && strchr(input, '|') == NULL) {
    // 攻击者使用其他字符绕过
}

// ✅ 白名单：只接受已知安全的字符
bool is_valid_username(const char *s) {
    for (const char *p = s; *p; p++) {
        if (!isalnum((unsigned char)*p) && *p != '_' && *p != '-') {
            return false;
        }
    }
    return strlen(s) <= 32 && strlen(s) > 0;
}
```

### 5.2 路径遍历防护

```c
// ❌ 漏洞：用户控制文件路径
void read_user_file(const char *user_path) {
    char path[256];
    snprintf(path, sizeof(path), "/data/%s", user_path);
    // 用户传入 "../../../etc/passwd"
    fopen(path, "r");
}

// ✅ 安全：规范化并验证
bool read_user_file_safe(const char *user_path) {
    char resolved[PATH_MAX];
    if (!realpath(user_path, resolved)) return false;

    // 确保解析后的路径在允许目录内
    const char *allowed = "/data/";
    if (strncmp(resolved, allowed, strlen(allowed)) != 0) {
        return false;
    }
    // 现在安全
    fopen(resolved, "r");
}
```

---

## 6. 安全API速查表

| 危险函数 | 安全替代 | 原因 |
|:---------|:---------|:-----|
| `strcpy` | `strlcpy`, `strncpy`+null | 无边界检查 |
| `strcat` | `strlcat`, `strncat` | 无边界检查 |
| `sprintf` | `snprintf` | 缓冲区溢出 |
| `gets` | `fgets`, `getline` | 无限读取 |
| `scanf("%s")` | `scanf("%63s")` | 无边界 |
| `malloc(n * size)` | `calloc` 或溢出检查 | 整数溢出 |
| `system(cmd)` | `execve` 系列 | 命令注入 |

---

## 7. 工具辅助检测

```bash
# 编译期
gcc -D_FORTIFY_SOURCE=3 -Wformat=2 -Wstack-protector

# 运行时
valgrind --tool=memcheck ./program
./program_asan    # AddressSanitizer 构建

# 静态分析
clang-tidy --checks='cert-*' src/*.c
infer run -- make

# 模糊测试
afl-fuzz -i in -o out ./target
```

---

> **最后更新**: 2026-05-11
> **参考**: CERT C Secure Coding Standard, CWE/SANS Top 25, SEI CERT
