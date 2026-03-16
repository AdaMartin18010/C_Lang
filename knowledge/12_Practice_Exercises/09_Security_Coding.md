# 安全编码练习 (Security Coding)

> **难度**: L3-L5 | **预计时间**: 20-35小时

---

## 练习1: 安全字符串操作

### 描述

实现防止缓冲区溢出的字符串库。

### 接口

```c
typedef struct {
    char *data;
    size_t len;      // 当前长度
    size_t capacity; // 总容量
} SafeString;

SafeString* safestr_create(size_t initial_capacity);
void safestr_destroy(SafeString *s);

// 安全的字符串操作
bool safestr_append(SafeString *s, const char *src);
bool safestr_append_n(SafeString *s, const char *src, size_t n);
bool safestr_printf(SafeString *s, const char *fmt, ...);

// 安全的字符串输入
bool safestr_readline(SafeString *s, FILE *stream);

// 防止格式字符串漏洞的包装
bool safestr_format(SafeString *s, const char *fmt, ...);
```

### 对比示例

```c
// 不安全
char buf[256];
strcpy(buf, user_input);  // 溢出风险
sprintf(buf, "Hello %s", name);  // 溢出+格式字符串漏洞

// 安全
SafeString *s = safestr_create(256);
safestr_append(s, user_input);  // 自动扩容
safestr_printf(s, "Hello %s", name);  // 安全检查
```

---

## 练习2: 安全内存分配器

### 描述

实现防堆溢出的内存分配器。

### 设计

```c
typedef struct {
    uint64_t canary_before;
    size_t size;
    uint32_t flags;
    uint64_t canary_after;
    // 用户数据从这里开始
} BlockHeader;

#define CANARY_VALUE 0xDEADBEEFCAFEBABEULL

void* safe_malloc(size_t size);
void* safe_calloc(size_t nmemb, size_t size);
void* safe_realloc(void *ptr, size_t size);
void safe_free(void *ptr);

// 检查堆完整性
bool heap_check(void);

// 调试功能
void heap_dump_leaks(void);
size_t heap_total_allocated(void);
```

### 保护机制

- 金丝雀值检测溢出
- 内存清零（防止信息泄露）
- 双重释放检测
- 使用后释放检测

---

## 练习3: 输入验证库

### 描述

实现全面的输入验证框架。

### 接口

```c
typedef enum {
    VAL_OK = 0,
    VAL_NULL,
    VAL_EMPTY,
    VAL_TOO_LONG,
    VAL_INVALID_CHAR,
    VAL_FORMAT_ERROR,
    VAL_OUT_OF_RANGE
} ValidationResult;

// 整数验证
ValidationResult validate_int(const char *input, int64_t min,
                              int64_t max, int64_t *output);
ValidationResult validate_uint(const char *input, uint64_t min,
                               uint64_t max, uint64_t *output);

// 浮点数验证
ValidationResult validate_double(const char *input, double min,
                                 double max, double *output);

// 字符串验证
typedef struct {
    size_t min_len;
    size_t max_len;
    const char *allowed_chars;  // NULL=全部允许
    const char *forbidden_chars;
} StringValidationRules;

ValidationResult validate_string(const char *input,
                                  const StringValidationRules *rules);

// 路径验证（防止目录遍历）
ValidationResult validate_path(const char *input,
                               const char *allowed_base_dir);

// 正则验证
ValidationResult validate_regex(const char *input, const char *pattern);
```

### 使用示例

```c
// 验证用户ID（只能是数字，1-1000000）
int64_t user_id;
ValidationResult r = validate_int(user_input, 1, 1000000, &user_id);
if (r != VAL_OK) {
    log_security_event("Invalid user ID input", user_input);
    return ERROR_INVALID_INPUT;
}
```

---

## 练习4: 加密工具库

### 描述

实现安全的加密操作（使用libsodium或OpenSSL）。

### 密码哈希（Argon2）

```c
// 密码哈希
bool password_hash(const char *password, char *hash_out, size_t hash_len);

// 验证密码
bool password_verify(const char *password, const char *hash);

// 需要重新哈希（如果参数升级）
bool password_needs_rehash(const char *hash);
```

### 对称加密（AES-GCM或ChaCha20-Poly1305）

```c
typedef struct {
    uint8_t key[32];
    uint8_t nonce[12];
} SymmetricKey;

// 加密
bool encrypt_aes_gcm(const uint8_t *plaintext, size_t pt_len,
                     const SymmetricKey *key,
                     const uint8_t *aad, size_t aad_len,
                     uint8_t *ciphertext,
                     uint8_t tag[16]);

// 解密
bool decrypt_aes_gcm(const uint8_t *ciphertext, size_t ct_len,
                     const SymmetricKey *key,
                     const uint8_t *aad, size_t aad_len,
                     const uint8_t tag[16],
                     uint8_t *plaintext);
```

### 随机数生成

```c
// 加密安全随机数
bool secure_random_bytes(void *buf, size_t len);
uint32_t secure_random_uint32(void);
uint64_t secure_random_uint64(void);

// 随机打乱数组（Fisher-Yates）
void secure_shuffle(void *array, size_t n, size_t size);
```

---

## 练习5: 安全日志系统

### 描述

实现防篡改的审计日志。

### 接口

```c
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_SECURITY  // 安全事件
} LogLevel;

typedef struct {
    uint64_t sequence;      // 序列号
    uint64_t timestamp;
    uint8_t prev_hash[32];  // 前一条日志的哈希
    uint8_t data_hash[32];  // 当前日志数据的哈希
    // ... 日志内容
} LogEntry;

// 初始化日志系统
bool audit_log_init(const char *logfile, const uint8_t *key);

// 记录日志
void audit_log(LogLevel level, const char *event_type,
               const char *format, ...);

// 验证日志完整性
bool audit_log_verify(const char *logfile);

// 导出日志（过滤、解密）
bool audit_log_export(const char *logfile, const char *outfile,
                      uint64_t start_time, uint64_t end_time);
```

### 安全特性

- 日志链式哈希（类似区块链）
- 敏感信息脱敏
- 访问控制
- 定期归档

---

## 练习6: 沙箱执行环境

### 描述

实现受限代码执行环境。

### Linux seccomp沙箱

```c
typedef struct {
    // 允许的syscall白名单
    int *allowed_syscalls;
    size_t num_allowed;

    // 资源限制
    struct rlimit cpu_limit;
    struct rlimit memory_limit;
    struct rlimit file_limit;

    // 文件系统限制
    char *chroot_dir;
    bool read_only;
} SandboxConfig;

// 在沙箱中执行函数
int sandbox_execute(int (*func)(void *arg), void *arg,
                    const SandboxConfig *config);

// 示例：受限的计算
int sandbox_compute(void *arg) {
    // 这段代码在沙箱中运行
    // 只能使用白名单syscall
    int *nums = arg;
    return nums[0] + nums[1];
}
```

### 使用示例

```c
SandboxConfig config = {
    .allowed_syscalls = (int[]){SYS_read, SYS_write, SYS_exit, SYS_exit_group},
    .num_allowed = 4,
    .cpu_limit = {5, 0},  // 5秒CPU时间
    .memory_limit = {64 * 1024 * 1024, 64 * 1024 * 1024},  // 64MB
};

int result = sandbox_execute(sandbox_compute, args, &config);
```

---

## 练习7: SQL注入防护

### 描述

实现安全的数据库查询构建器。

### 接口

```c
typedef struct QueryBuilder QueryBuilder;

// 创建参数化查询
QueryBuilder* query_create(const char *table);

// 安全添加条件（自动转义）
void query_where_int(QueryBuilder *qb, const char *column,
                     const char *op, int64_t value);
void query_where_string(QueryBuilder *qb, const char *column,
                        const char *op, const char *value);
void query_where_in_ints(QueryBuilder *qb, const char *column,
                         const int64_t *values, size_t count);

// 构建查询（生成参数化SQL）
char* query_build(const QueryBuilder *qb);
void** query_get_params(const QueryBuilder *qb, size_t *count);

// 清理
void query_destroy(QueryBuilder *qb);
```

### 使用示例

```c
// 安全
QueryBuilder *qb = query_create("users");
query_where_int(qb, "id", "=", user_id);
query_where_string(qb, "name", "=", user_name);

// 生成: SELECT * FROM users WHERE id = ? AND name = ?
// 参数: [user_id, user_name]

// 不安全（传统方式）
// sprintf(sql, "SELECT * FROM users WHERE id = %d", user_id);  // 注入风险
```

---

## 练习8: 安全头文件与编译选项

### 安全编译模板

```c
// security_config.h
#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

// 禁用危险函数
#define strcpy(...)  _error_use_strlcpy_instead_of_strcpy
#define strcat(...)  _error_use_strlcat_instead_of_strcat
#define sprintf(...) _error_use_snprintf_instead_of_sprintf
#define gets(...)    _error_never_use_gets

// 安全检查宏
#define CHECK_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Null pointer at %s:%d\n", __FILE__, __LINE__); \
        abort(); \
    } \
} while(0)

#define CHECK_BOUNDS(index, size) do { \
    if ((index) >= (size)) { \
        fprintf(stderr, "Bounds check failed at %s:%d\n", __FILE__, __LINE__); \
        abort(); \
    } \
} while(0)

#endif
```

### 安全编译选项

```makefile
# Makefile
CFLAGS += -D_FORTIFY_SOURCE=2
CFLAGS += -fstack-protector-strong
CFLAGS += -Wformat -Wformat-security
CFLAGS += -fPIE -pie
CFLAGS += -Wl,-z,relro,-z,now
CFLAGS += -Wl,-z,noexecstack
```

---

## 安全审计检查清单

- [ ] 所有输入都经过验证
- [ ] 缓冲区操作有边界检查
- [ ] 使用参数化查询（无SQL注入）
- [ ] 敏感数据已加密存储
- [ ] 密码使用Argon2/bcrypt
- [ ] 随机数使用加密安全生成器
- [ ] 日志记录安全事件
- [ ] 无硬编码密钥
- [ ] 启用了ASLR和栈保护
- [ ] 使用静态分析工具检查

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)
