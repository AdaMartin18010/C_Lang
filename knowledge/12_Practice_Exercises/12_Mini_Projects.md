# 综合迷你项目 (Mini Projects)

> **难度**: L2-L5 | **预计时间**: 15-40小时（每个项目）

---

## 项目1: HTTP服务器

### 描述

实现一个支持HTTP/1.1的Web服务器。

### 功能要求

- [ ] 处理GET/HEAD请求
- [ ] 支持静态文件服务
- [ ] 虚拟主机（多域名）
- [ ] 目录列表
- [ ] 范围请求（Range header）
- [ ] 持久连接（Keep-Alive）
- [ ] 压缩（gzip/deflate）
- [ ] 日志记录

### 接口

```c
typedef struct HttpServer HttpServer;
typedef struct {
    const char *document_root;
    int port;
    int max_connections;
    bool enable_gzip;
} ServerConfig;

HttpServer* http_server_create(const ServerConfig *config);
void http_server_destroy(HttpServer *server);
int http_server_run(HttpServer *server);  // 阻塞
void http_server_stop(HttpServer *server);
```

### 进阶

- 多线程/线程池处理请求
- CGI/FastCGI支持
- HTTPS（OpenSSL）
- WebSocket支持

---

## 项目2: 简单数据库

### 描述

实现一个键值存储数据库。

### 核心功能

```c
typedef struct Database Database;

typedef enum {
    DB_OK = 0,
    DB_NOT_FOUND,
    DB_FULL,
    DB_IO_ERROR
} DBError;

// 生命周期
Database* db_open(const char *path);
void db_close(Database *db);

// 基本操作
DBError db_put(Database *db, const char *key,
               const void *value, size_t value_len);
DBError db_get(Database *db, const char *key,
               void *out_buf, size_t buf_size, size_t *out_len);
DBError db_delete(Database *db, const char *key);

// 迭代
void* db_iterator_create(Database *db);
bool db_iterator_next(void *iter, char **key, void **value, size_t *len);
void db_iterator_destroy(void *iter);

// 事务
DBError db_begin(Database *db);
DBError db_commit(Database *db);
DBError db_rollback(Database *db);
```

### 存储引擎选项

1. **LSM-Tree**（日志结构合并树）
   - 写优化
   - LevelDB风格

2. **B+树**
   - 读优化
   - 范围查询友好

3. **哈希表**
   - O(1)读写
   - 仅支持点查询

### 进阶

- 压缩（Snappy/LZ4）
- 校验和（CRC32）
- 备份/恢复
- 网络接口（Redis协议）

---

## 项目3: 编译器前端

### 描述

为简单的类C语言实现编译器前端。

### 语言定义

```c
// 示例程序
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    int result = factorial(5);
    print(result);
    return 0;
}
```

### 实现步骤

1. **词法分析器（Lexer）**

```c
typedef enum {
    TOK_INT,
    TOK_IF,
    TOK_RETURN,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_SEMICOLON,
    TOK_EOF,
} TokenType;

typedef struct {
    TokenType type;
    char *text;
    int line, column;
} Token;

Token* lexer_tokenize(const char *source);
```

1. **语法分析器（Parser）**

```c
// 递归下降解析
// 生成AST

typedef struct AstNode AstNode;
struct AstNode {
    enum { AST_FUNC, AST_IF, AST_RETURN, AST_BINARY, AST_CALL, ... } type;
    // ...
    AstNode **children;
};

AstNode* parser_parse(Token *tokens);
```

1. **语义分析**

```c
// 类型检查
// 符号表构建
bool sema_check(AstNode *ast);
```

1. **代码生成**

```c
// 生成LLVM IR 或 x86汇编
void codegen_generate(AstNode *ast, FILE *out);
```

---

## 项目4: 网络协议实现

### 选择以下协议之一实现

#### 4.1 DNS客户端

```c
// 解析域名
DNSResult* dns_resolve(const char *hostname, DNSRecordType type);

// 示例
DNSResult *result = dns_resolve("www.example.com", DNS_A);
for (int i = 0; i < result->count; i++) {
    printf("%s -> %s\n", result->name, result->records[i].ip);
}
```

#### 4.2 SMTP客户端

```c
// 发送邮件
SMTPSession* smtp_connect(const char *server, int port);
void smtp_auth(SMTPSession *session, const char *user, const char *pass);
void smtp_send(SMTPSession *session, const Email *email);
void smtp_quit(SMTPSession *session);
```

#### 4.3 TFTP服务器/客户端

- 支持RRQ/WRQ
- 块传输
- 超时重传

#### 4.4 BitTorrent客户端（简化版）

- 解析.torrent文件
- Tracker通信
- Peer连接
- 块下载

---

## 项目5: 图形渲染

### 选择以下方向

#### 5.1 软件光栅化器

```c
// 2D渲染
void draw_line(FrameBuffer *fb, int x0, int y0, int x1, int y1, Color c);
void draw_triangle(FrameBuffer *fb, Vec2 v[3], Color c);
void fill_triangle(FrameBuffer *fb, Vec2 v[3], Color c);

// 3D渲染（软光栅）
void draw_mesh(FrameBuffer *fb, const Mesh *mesh,
               const Mat4 *mvp, const Camera *cam);
```

#### 5.2 光线追踪器

```c
// 简单光线追踪
typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct {
    bool hit;
    float t;
    Vec3 point;
    Vec3 normal;
    Material *material;
} HitRecord;

Color trace_ray(const Scene *scene, const Ray *ray, int depth);
```

#### 5.3 游戏模拟

- 生命游戏（Conway's Game of Life）
- 流体模拟（SPH或格子玻尔兹曼）
- 粒子系统

---

## 项目6: 实用工具

### 6.1 代码格式化器（类似clang-format）

```c
// 读取C代码，输出格式化后的代码
void format_file(const char *input_path, const char *output_path,
                 const FormatOptions *options);
```

### 6.2 静态网站生成器

```c
// Markdown -> HTML
typedef struct {
    char *title;
    char *content;
    char *template;
} Page;

void ssg_build(const char *src_dir, const char *out_dir);
```

### 6.3 包管理器（简化版）

```c
// 类似npm/cargo
void pkg_install(const char *package_name, const char *version);
void pkg_remove(const char *package_name);
void pkg_update(void);
```

### 6.4 配置文件解析器

支持格式：

- JSON
- YAML
- TOML
- INI

```c
Config* config_load(const char *path, ConfigFormat format);
int config_get_int(Config *cfg, const char *key, int default_val);
const char* config_get_string(Config *cfg, const char *key);
```

---

## 项目7: 嵌入式模拟

### 7.1 虚拟机实现

```c
// 简单栈式虚拟机

typedef enum {
    OP_PUSH, OP_POP,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_JMP, OP_JZ, OP_JNZ,
    OP_CALL, OP_RET,
    OP_LOAD, OP_STORE,
    OP_PRINT, OP_HALT
} OpCode;

typedef struct {
    uint8_t *code;
    size_t code_size;
    int *stack;
    size_t stack_size;
    int sp, ip;
} VM;

void vm_load(VM *vm, const uint8_t *bytecode, size_t len);
void vm_run(VM *vm);
```

### 7.2 CPU模拟器

模拟简单CPU：

- 寄存器组
- 内存
- ALU
- 指令集

### 7.3 CHIP-8模拟器

完整的CHIP-8实现，可运行经典游戏。

---

## 项目8: 数据处理

### 8.1 CSV处理器

```c
// 流式处理大CSV文件
CSVParser* csv_open(const char *path);
void csv_set_delimiter(CSVParser *parser, char delim);

// 逐行读取
while (csv_read_row(parser, &row)) {
    for (int i = 0; i < row.count; i++) {
        printf("%s\t", row.fields[i]);
    }
    printf("\n");
}

// 聚合查询（类似SQL）
CSVResult* csv_query(CSVParser *parser, const char *sql_like_query);
```

### 8.2 JSON解析器

```c
JSONValue* json_parse(const char *text);
JSONValue* json_parse_file(const char *path);

// 访问
int json_get_int(const JSONValue *val, const char *path);
const char* json_get_string(const JSONValue *val, const char *path);

// 序列化
char* json_stringify(const JSONValue *val, bool pretty);
```

### 8.3 日志分析器

```c
// 分析Web服务器日志
LogAnalyzer* analyzer_create(void);
void analyzer_add_filter(LogAnalyzer *a, FilterFunc filter);
void analyzer_add_aggregator(LogAnalyzer *a, AggFunc agg);

// 输出Top 10 IP、URL、状态码统计等
```

---

## 项目提交要求

每个项目应包含：

1. **源代码**（良好组织和注释）
2. **构建脚本**（Makefile/CMake）
3. **测试用例**（单元测试）
4. **README**（设计说明、使用方法）
5. **性能基准**（如适用）

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)
