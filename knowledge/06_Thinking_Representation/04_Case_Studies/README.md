# 案例研究 (Case Studies)

## 概述

案例研究是理解软件工程实践中最有效的学习方法之一。通过深入分析真实的项目案例，我们可以学习到成功的设计模式、识别常见的失败陷阱，以及理解重构的最佳时机和方法。

## 目录

- [案例研究 (Case Studies)](#案例研究-case-studies)
  - [概述](#概述)
  - [目录](#目录)
  - [成功的 C 项目分析](#成功的-c-项目分析)
    - [案例 1: Linux 内核 - 模块化设计的典范](#案例-1-linux-内核---模块化设计的典范)
      - [项目概况](#项目概况)
      - [架构设计亮点](#架构设计亮点)
      - [设计模式应用](#设计模式应用)
      - [成功要素分析](#成功要素分析)
    - [案例 2: Redis - 高性能内存数据库](#案例-2-redis---高性能内存数据库)
      - [核心设计决策](#核心设计决策)
      - [事件循环实现](#事件循环实现)
      - [数据结构优化](#数据结构优化)
    - [案例 3: Nginx - 高并发 Web 服务器](#案例-3-nginx---高并发-web-服务器)
      - [架构设计](#架构设计)
      - [连接处理模型](#连接处理模型)
  - [失败案例分析](#失败案例分析)
    - [案例 1: 缓冲区溢出安全漏洞 - Heartbleed](#案例-1-缓冲区溢出安全漏洞---heartbleed)
      - [漏洞概述](#漏洞概述)
      - [问题代码分析](#问题代码分析)
      - [修复方案](#修复方案)
      - [经验教训](#经验教训)
    - [案例 2: 内存泄漏导致的系统崩溃](#案例-2-内存泄漏导致的系统崩溃)
      - [问题描述](#问题描述)
      - [问题代码](#问题代码)
      - [修复与改进](#修复与改进)
      - [防御性编程建议](#防御性编程建议)
    - [案例 3: 竞态条件导致的数据损坏](#案例-3-竞态条件导致的数据损坏)
      - [问题场景](#问题场景)
      - [问题代码](#问题代码-1)
      - [修复方案](#修复方案-1)
  - [重构案例](#重构案例)
    - [案例 1: 从混乱到清晰 - 网络协议解析器重构](#案例-1-从混乱到清晰---网络协议解析器重构)
      - [重构前代码](#重构前代码)
      - [重构策略](#重构策略)
      - [重构后代码](#重构后代码)
    - [案例 2: 消除重复代码 - 配置系统重构](#案例-2-消除重复代码---配置系统重构)
      - [重构前：重复的配置解析](#重构前重复的配置解析)
      - [重构后：统一抽象层](#重构后统一抽象层)
    - [案例 3: 提高可测试性 - 依赖注入重构](#案例-3-提高可测试性---依赖注入重构)
      - [重构前：紧耦合代码](#重构前紧耦合代码)
      - [重构后：依赖注入和接口抽象](#重构后依赖注入和接口抽象)
  - [总结](#总结)
    - [成功案例的共同特征](#成功案例的共同特征)
    - [失败案例的常见原因](#失败案例的常见原因)
    - [重构最佳实践](#重构最佳实践)

---

## 成功的 C 项目分析

### 案例 1: Linux 内核 - 模块化设计的典范

#### 项目概况

- **代码规模**: 超过 3000 万行 C 代码
- **开发周期**: 30+ 年持续演进
- **贡献者**: 全球 5000+ 开发者
- **成功因素**: 严格的编码规范、模块化设计、高效的社区协作

#### 架构设计亮点

```c
// Linux 内核的驱动模型设计 - 展示松耦合架构
// include/linux/device.h

struct device_driver {
    const char      *name;
    struct bus_type *bus;

    // 操作函数表 - 实现多态性
    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);
    void (*shutdown)(struct device *dev);
};

struct device {
    struct device_driver *driver;
    void                 *platform_data;
    void                 *driver_data;

    // 设备树节点引用
    struct device_node   *of_node;
};
```

#### 设计模式应用

| 设计模式 | 应用场景 | 实现方式 |
|---------|---------|---------|
| 观察者模式 | 设备状态通知 | notifier_chain |
| 策略模式 | 调度算法切换 | sched_class 结构体 |
| 模板方法 | 文件系统操作 | super_operations |
| 桥接模式 | 设备与驱动分离 | device/driver 分离 |

#### 成功要素分析

1. **层次化架构**

   ```text
   用户空间系统调用接口
            ↓
   虚拟文件系统层 (VFS)
            ↓
   具体文件系统实现 (ext4, xfs, btrfs)
            ↓
   块设备层 (Block Layer)
            ↓
   设备驱动层
   ```

2. **配置系统的灵活性**

   ```c
   // Kconfig 驱动的条件编译
   #ifdef CONFIG_SMP
   // 多处理器支持代码
   static DEFINE_PER_CPU(struct task_struct *, curr_task);
   #else
   // 单处理器简化实现
   static struct task_struct *curr_task;
   #endif
   ```

### 案例 2: Redis - 高性能内存数据库

#### 核心设计决策

| 设计决策 | 选择 | 理由 |
|---------|-----|------|
| 单线程模型 | 采用 | 避免锁竞争，简化代码 |
| 内存优先 | 采用 | 最大化读写性能 |
| C 语言实现 | 采用 | 极致性能控制 |
| 异步 I/O | 采用 | 处理高并发连接 |

#### 事件循环实现

```c
// Redis 事件循环核心 - ae.c

typedef struct aeEventLoop {
    int maxfd;
    int setsize;
    long long timeEventNextId;
    aeFileEvent *events;  // 文件事件数组
    aeFiredEvent *fired;  // 已触发事件
    aeTimeEvent *timeEventHead;
    int stop;
    void *apidata;  // 平台特定数据 (epoll/kqueue/select)
    aeBeforeSleepProc *beforesleep;
} aeEventLoop;

// 事件处理主循环
void aeMain(aeEventLoop *eventLoop) {
    eventLoop->stop = 0;
    while (!eventLoop->stop) {
        if (eventLoop->beforesleep)
            eventLoop->beforesleep(eventLoop);
        aeProcessEvents(eventLoop, AE_ALL_EVENTS|AE_CALL_AFTER_SLEEP);
    }
}
```

#### 数据结构优化

```c
// SDS (Simple Dynamic String) - Redis 的字符串实现
struct __attribute__ ((__packed__)) sdshdr64 {
    uint64_t len;        // 已使用长度
    uint64_t alloc;      // 分配的总长度
    unsigned char flags; // 标志位
    char buf[];          // 柔性数组存储实际数据
};

// 优势分析：
// 1. O(1) 时间获取长度
// 2. 预分配减少内存重分配
// 3. 二进制安全
// 4. 兼容 C 字符串函数
```

### 案例 3: Nginx - 高并发 Web 服务器

#### 架构设计

```c
// Nginx 的模块化架构
// 核心思想：将功能分解为独立模块

typedef struct {
    ngx_int_t   (*preconfiguration)(ngx_conf_t *cf);
    ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);

    void       *(*create_main_conf)(ngx_conf_t *cf);
    char       *(*init_main_conf)(ngx_conf_t *cf, void *conf);

    void       *(*create_srv_conf)(ngx_conf_t *cf);
    char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);

    void       *(*create_loc_conf)(ngx_conf_t *cf);
    char       *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
} ngx_http_module_t;
```

#### 连接处理模型

| 模型 | Nginx 实现 | 优势 |
|-----|-----------|------|
| Master-Worker | 多进程模型 | 利用多核，故障隔离 |
| 事件驱动 | epoll/kqueue | 高并发低资源 |
| 异步 I/O | 非阻塞操作 | 无等待状态 |
| 连接池 | 预分配连接 | 减少内存分配 |

---

## 失败案例分析

### 案例 1: 缓冲区溢出安全漏洞 - Heartbleed

#### 漏洞概述

- **影响**: OpenSSL 库
- **类型**: 缓冲区过度读取
- **影响范围**: 全球 17% 的安全服务器
- **根本原因**: 缺少边界检查

#### 问题代码分析

```c
// 有问题的原始代码 (简化)
int dtls1_process_heartbeat(SSL *s) {
    unsigned char *p = &s->s3->rrec.data[0];
    unsigned short payload;

    // 从网络数据包读取 payload 长度 - 未验证！
    n2s(p, payload);  // payload 可被攻击者控制

    // 分配响应缓冲区
    buffer = OPENSSL_malloc(1 + 2 + payload + padding);

    // 复制数据 - 可能读取越界！
    memcpy(bp, p, payload);  // 如果 payload > 实际数据长度

    return 0;
}
```

#### 修复方案

```c
// 修复后的代码
int dtls1_process_heartbeat(SSL *s) {
    unsigned char *p = &s->s3->rrec.data[0];
    unsigned short payload;
    unsigned short max_payload;

    // 计算实际可用的最大 payload
    max_payload = s->s3->rrec.length - 1 - 2;  // 减去类型和长度字段

    n2s(p, payload);

    // 关键验证：确保请求的长度不超过实际数据
    if (payload > max_payload) {
        // 错误处理：请求长度超过实际数据
        return -1;
    }

    // 安全分配
    buffer = OPENSSL_malloc(1 + 2 + payload + padding);
    if (buffer == NULL) return -1;

    // 安全复制 - 现在 payload 已被验证
    memcpy(bp, p, payload);

    return 0;
}
```

#### 经验教训

| 方面 | 教训 |
|-----|------|
| 输入验证 | 永远不要信任外部输入的长度字段 |
| 边界检查 | 所有内存操作前必须验证边界 |
| 代码审查 | 安全关键代码需要专门的安全审计 |
| 测试覆盖 | 需要模糊测试发现边界问题 |

### 案例 2: 内存泄漏导致的系统崩溃

#### 问题描述

- **场景**: 长期运行的嵌入式设备
- **现象**: 运行 72 小时后系统无响应
- **根因**: 循环中的内存未释放

#### 问题代码

```c
// 有问题的网络连接处理代码
void handle_connections(int server_fd) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        // 接受新连接
        int client_fd = accept(server_fd,
                               (struct sockaddr*)&client_addr,
                               &addr_len);

        // 为客户端数据分配内存
        client_data_t* data = malloc(sizeof(client_data_t));
        data->fd = client_fd;

        // 处理请求
        process_request(data);

        // 错误：忘记释放 data！
        // 也忘记关闭 client_fd！
    }
}
```

#### 修复与改进

```c
// 修复后的代码
void handle_connections(int server_fd) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd,
                               (struct sockaddr*)&client_addr,
                               &addr_len);
        if (client_fd < 0) {
            log_error("Accept failed: %s", strerror(errno));
            continue;
        }

        // 使用 RAII 风格的资源管理
        client_data_t* data = malloc(sizeof(client_data_t));
        if (!data) {
            close(client_fd);
            log_error("Memory allocation failed");
            continue;
        }

        data->fd = client_fd;

        // 处理请求
        int result = process_request(data);

        // 确保资源被正确释放
        cleanup:
            if (data) {
                if (data->buffer) free(data->buffer);
                free(data);
            }
            if (client_fd >= 0) {
                close(client_fd);
            }
    }
}
```

#### 防御性编程建议

```c
// 更好的方案：使用包装函数确保资源释放
typedef struct {
    int fd;
    void* buffer;
} managed_client_t;

managed_client_t* client_new(int fd) {
    managed_client_t* c = calloc(1, sizeof(managed_client_t));
    if (c) c->fd = fd;
    return c;
}

void client_free(managed_client_t* c) {
    if (c) {
        if (c->fd >= 0) close(c->fd);
        free(c->buffer);
        free(c);
    }
}

// 使用示例
void handle_connections_improved(int server_fd) {
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        managed_client_t* client = client_new(client_fd);

        if (!client) {
            close(client_fd);
            continue;
        }

        process_request(client);
        client_free(client);  // 确保释放
    }
}
```

### 案例 3: 竞态条件导致的数据损坏

#### 问题场景

- **环境**: 多线程日志系统
- **现象**: 日志文件中出现乱码
- **根因**: 多个线程同时写入无保护的缓冲区

#### 问题代码

```c
// 线程不安全的日志系统
static char log_buffer[4096];
static FILE* log_file;

void log_message(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // 危险：所有线程共享 log_buffer！
    vsnprintf(log_buffer, sizeof(log_buffer), format, args);

    fprintf(log_file, "%s\n", log_buffer);
    fflush(log_file);

    va_end(args);
}
```

#### 修复方案

```c
// 线程安全的日志系统
#include <pthread.h>

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE* log_file;

// 使用线程局部存储
_Thread_local char thread_buffer[4096];

void log_message_safe(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // 方案 1：使用线程局部缓冲区
    vsnprintf(thread_buffer, sizeof(thread_buffer), format, args);

    pthread_mutex_lock(&log_mutex);
    fprintf(log_file, "[%lu] %s\n", pthread_self(), thread_buffer);
    fflush(log_file);
    pthread_mutex_unlock(&log_mutex);

    va_end(args);
}

// 更高效的方案：批量写入
#define LOG_BUFFER_SIZE 65536

typedef struct {
    char buffer[LOG_BUFFER_SIZE];
    size_t used;
    pthread_mutex_t mutex;
} log_buffer_t;

void log_message_buffered(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // 格式化到线程局部缓冲区
    char temp[4096];
    int len = vsnprintf(temp, sizeof(temp), format, args);
    va_end(args);

    pthread_mutex_lock(&global_log_buffer.mutex);

    // 检查是否需要刷新
    if (global_log_buffer.used + len + 2 > LOG_BUFFER_SIZE) {
        fwrite(global_log_buffer.buffer, 1, global_log_buffer.used, log_file);
        global_log_buffer.used = 0;
    }

    // 追加到缓冲区
    memcpy(global_log_buffer.buffer + global_log_buffer.used, temp, len);
    global_log_buffer.used += len;
    global_log_buffer.buffer[global_log_buffer.used++] = '\n';

    pthread_mutex_unlock(&global_log_buffer.mutex);
}
```

---

## 重构案例

### 案例 1: 从混乱到清晰 - 网络协议解析器重构

#### 重构前代码

```c
// 原始代码：混乱的协议解析器
int parse_packet(char* data, int len) {
    if (len < 4) return -1;
    int type = data[0];
    int flags = data[1];
    int length = (data[2] << 8) | data[3];

    if (type == 1) {
        if (len < 4 + length) return -1;
        // 处理类型 1
        for (int i = 0; i < length; i++) {
            printf("%c", data[4 + i]);
        }
        return 4 + length;
    } else if (type == 2) {
        if (len < 8) return -1;
        int id = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
        printf("ID: %d\n", id);
        return 8;
    } else if (type == 3) {
        // 更多混乱的代码...
        return -1;
    }
    return -1;
}
```

#### 重构策略

| 重构步骤 | 具体操作 | 改进效果 |
|---------|---------|---------|
| 提取结构体 | 定义协议头结构 | 类型安全 |
| 分离解析逻辑 | 每种类型独立函数 | 可维护性 |
| 添加验证层 | 统一边界检查 | 安全性 |
| 错误处理 | 统一的错误码 | 可调试性 |

#### 重构后代码

```c
// 重构后：清晰的分层架构

// 1. 协议定义层
#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    uint8_t flags;
    uint16_t length;
} packet_header_t;

typedef struct {
    packet_header_t header;
    uint32_t id;
} id_packet_t;
#pragma pack(pop)

// 2. 错误码定义
typedef enum {
    PARSE_OK = 0,
    PARSE_ERROR_TOO_SHORT = -1,
    PARSE_ERROR_INVALID_TYPE = -2,
    PARSE_ERROR_LENGTH_MISMATCH = -3
} parse_result_t;

// 3. 处理器接口
typedef parse_result_t (*packet_handler_t)(const uint8_t* data, size_t len, size_t* consumed);

// 4. 具体处理器
static parse_result_t handle_data_packet(const uint8_t* data, size_t len, size_t* consumed) {
    const packet_header_t* hdr = (const packet_header_t*)data;
    size_t total_len = sizeof(packet_header_t) + ntohs(hdr->length);

    if (len < total_len) {
        return PARSE_ERROR_TOO_SHORT;
    }

    // 处理数据
    process_payload(data + sizeof(packet_header_t), ntohs(hdr->length));

    *consumed = total_len;
    return PARSE_OK;
}

static parse_result_t handle_id_packet(const uint8_t* data, size_t len, size_t* consumed) {
    if (len < sizeof(id_packet_t)) {
        return PARSE_ERROR_TOO_SHORT;
    }

    const id_packet_t* pkt = (const id_packet_t*)data;
    uint32_t id = ntohl(pkt->id);
    process_id(id);

    *consumed = sizeof(id_packet_t);
    return PARSE_OK;
}

// 5. 分发表
typedef struct {
    uint8_t type;
    size_t min_size;
    packet_handler_t handler;
    const char* name;
} packet_type_entry_t;

static const packet_type_entry_t packet_types[] = {
    { 1, sizeof(packet_header_t), handle_data_packet, "DATA" },
    { 2, sizeof(id_packet_t), handle_id_packet, "ID" },
    { 0, 0, NULL, NULL }
};

// 6. 主解析器
parse_result_t parse_packet_v2(const uint8_t* data, size_t len, size_t* consumed) {
    if (len < sizeof(packet_header_t)) {
        return PARSE_ERROR_TOO_SHORT;
    }

    const packet_header_t* hdr = (const packet_header_t*)data;
    uint8_t type = hdr->type;

    // 查找处理器
    for (const packet_type_entry_t* entry = packet_types; entry->handler; entry++) {
        if (entry->type == type) {
            if (len < entry->min_size) {
                return PARSE_ERROR_TOO_SHORT;
            }
            return entry->handler(data, len, consumed);
        }
    }

    return PARSE_ERROR_INVALID_TYPE;
}
```

### 案例 2: 消除重复代码 - 配置系统重构

#### 重构前：重复的配置解析

```c
// 原始代码：多个配置文件格式，重复逻辑

int parse_config_xml(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return -1;

    // XML 解析逻辑 (100+ 行)
    // ... 大量重复代码

    fclose(f);
    return 0;
}

int parse_config_json(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return -1;

    // JSON 解析逻辑 (100+ 行)
    // ... 大量重复代码

    fclose(f);
    return 0;
}

int parse_config_ini(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return -1;

    // INI 解析逻辑 (100+ 行)
    // ... 大量重复代码

    fclose(f);
    return 0;
}
```

#### 重构后：统一抽象层

```c
// 重构后：通用配置系统

// 1. 统一的配置项抽象
typedef enum {
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_BOOL,
    CONFIG_TYPE_FLOAT,
    CONFIG_TYPE_ARRAY,
    CONFIG_TYPE_OBJECT
} config_value_type_t;

typedef struct config_value {
    config_value_type_t type;
    union {
        char* string_val;
        int64_t int_val;
        bool bool_val;
        double float_val;
        struct { struct config_value* items; size_t count; } array_val;
        struct { char** keys; struct config_value* values; size_t count; } object_val;
    };
} config_value_t;

// 2. 解析器接口
typedef struct {
    const char* name;
    const char** extensions;  // 支持的文件扩展名
    config_value_t* (*parse)(const char* content, size_t len, char** error);
    void (*free)(config_value_t* value);
} config_parser_t;

// 3. 解析器注册表
static config_parser_t* parsers[16];
static int parser_count = 0;

void config_register_parser(config_parser_t* parser) {
    if (parser_count < 16) {
        parsers[parser_count++] = parser;
    }
}

// 4. 统一加载函数
config_value_t* config_load(const char* filename, char** error) {
    // 读取文件
    FILE* f = fopen(filename, "rb");
    if (!f) {
        if (error) *error = strdup("Cannot open file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* content = malloc(len + 1);
    fread(content, 1, len, f);
    content[len] = '\0';
    fclose(f);

    // 根据扩展名选择解析器
    const char* ext = strrchr(filename, '.');
    config_parser_t* parser = NULL;

    for (int i = 0; i < parser_count; i++) {
        for (const char** p = parsers[i]->extensions; *p; p++) {
            if (strcasecmp(ext, *p) == 0) {
                parser = parsers[i];
                break;
            }
        }
        if (parser) break;
    }

    if (!parser) {
        free(content);
        if (error) *error = strdup("No parser for file type");
        return NULL;
    }

    // 解析
    config_value_t* result = parser->parse(content, len, error);
    free(content);
    return result;
}

// 5. 统一访问 API
const char* config_get_string(config_value_t* root, const char* path, const char* default_val) {
    config_value_t* val = config_resolve_path(root, path);
    if (val && val->type == CONFIG_TYPE_STRING) {
        return val->string_val;
    }
    return default_val;
}

int64_t config_get_int(config_value_t* root, const char* path, int64_t default_val) {
    config_value_t* val = config_resolve_path(root, path);
    if (val && val->type == CONFIG_TYPE_INT) {
        return val->int_val;
    }
    return default_val;
}
```

### 案例 3: 提高可测试性 - 依赖注入重构

#### 重构前：紧耦合代码

```c
// 原始代码：直接依赖数据库连接
// 难以测试，无法 mock

typedef struct {
    int id;
    char name[256];
    double balance;
} account_t;

int transfer_funds(int from_id, int to_id, double amount) {
    // 直接硬编码数据库连接
    MYSQL* conn = mysql_init(NULL);
    mysql_real_connect(conn, "prod.db.server", "user", "pass", "bank", 0, NULL, 0);

    // 检查余额
    char query[256];
    snprintf(query, sizeof(query), "SELECT balance FROM accounts WHERE id=%d", from_id);
    mysql_query(conn, query);

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    double balance = atof(row[0]);
    mysql_free_result(result);

    if (balance < amount) {
        mysql_close(conn);
        return -1;  // 余额不足
    }

    // 执行转账...
    mysql_query(conn, "BEGIN");
    // 更新账户...
    mysql_query(conn, "COMMIT");

    mysql_close(conn);
    return 0;
}
```

#### 重构后：依赖注入和接口抽象

```c
// 重构后：可测试的架构

// 1. 定义数据访问接口
typedef struct account_repository account_repository_t;

struct account_repository {
    // 虚函数表
    account_t* (*find_by_id)(account_repository_t* self, int id);
    int (*update)(account_repository_t* self, account_t* account);
    int (*begin_transaction)(account_repository_t* self);
    int (*commit)(account_repository_t* self);
    int (*rollback)(account_repository_t* self);
    void (*destroy)(account_repository_t* self);
    void* impl_data;  // 实现特定数据
};

// 2. MySQL 实现
account_repository_t* account_repository_mysql_create(const db_config_t* config);

// 3. 内存实现（用于测试）
typedef struct {
    account_t* accounts;
    size_t count;
    size_t capacity;
    bool in_transaction;
    account_t* backup;  // 事务回滚用
} memory_repo_impl_t;

account_repository_t* account_repository_memory_create(void) {
    account_repository_t* repo = calloc(1, sizeof(account_repository_t));
    memory_repo_impl_t* impl = calloc(1, sizeof(memory_repo_impl_t));

    impl->capacity = 16;
    impl->accounts = calloc(impl->capacity, sizeof(account_t));

    repo->impl_data = impl;
    repo->find_by_id = memory_find_by_id;
    repo->update = memory_update;
    repo->begin_transaction = memory_begin_transaction;
    repo->commit = memory_commit;
    repo->rollback = memory_rollback;
    repo->destroy = memory_destroy;

    return repo;
}

// 4. 业务逻辑 - 现在可测试
typedef struct {
    account_repository_t* account_repo;
    transaction_repository_t* transaction_repo;
    notification_service_t* notification_service;
} transfer_service_t;

transfer_result_t transfer_funds_improved(transfer_service_t* service,
                                          int from_id, int to_id,
                                          double amount) {
    // 验证输入
    if (amount <= 0) {
        return TRANSFER_INVALID_AMOUNT;
    }
    if (from_id == to_id) {
        return TRANSFER_SAME_ACCOUNT;
    }

    // 查找账户
    account_t* from = service->account_repo->find_by_id(service->account_repo, from_id);
    if (!from) return TRANSFER_ACCOUNT_NOT_FOUND;

    account_t* to = service->account_repo->find_by_id(service->account_repo, to_id);
    if (!to) return TRANSFER_ACCOUNT_NOT_FOUND;

    // 检查余额
    if (from->balance < amount) {
        return TRANSFER_INSUFFICIENT_FUNDS;
    }

    // 执行转账
    service->account_repo->begin_transaction(service->account_repo);

    from->balance -= amount;
    to->balance += amount;

    int result1 = service->account_repo->update(service->account_repo, from);
    int result2 = service->account_repo->update(service->account_repo, to);

    if (result1 != 0 || result2 != 0) {
        service->account_repo->rollback(service->account_repo);
        return TRANSFER_UPDATE_FAILED;
    }

    service->account_repo->commit(service->account_repo);

    // 发送通知
    service->notification_service->notify_transfer(
        service->notification_service, from_id, to_id, amount);

    return TRANSFER_SUCCESS;
}

// 5. 单元测试示例
void test_transfer_success() {
    // 使用内存仓库进行测试
    account_repository_t* repo = account_repository_memory_create();

    // 准备测试数据
    account_t acc1 = { .id = 1, .name = "Alice", .balance = 1000.0 };
    account_t acc2 = { .id = 2, .name = "Bob", .balance = 500.0 };
    repo->update(repo, &acc1);
    repo->update(repo, &acc2);

    // 创建服务
    transfer_service_t service = {
        .account_repo = repo,
        .transaction_repo = NULL,
        .notification_service = mock_notification_service()
    };

    // 执行测试
    transfer_result_t result = transfer_funds_improved(&service, 1, 2, 100.0);

    // 验证结果
    assert(result == TRANSFER_SUCCESS);
    account_t* updated1 = repo->find_by_id(repo, 1);
    account_t* updated2 = repo->find_by_id(repo, 2);
    assert(updated1->balance == 900.0);
    assert(updated2->balance == 600.0);

    repo->destroy(repo);
}
```

---

## 总结

### 成功案例的共同特征

1. **清晰的架构边界** - 模块间通过定义良好的接口交互
2. **防御性编程** - 假设输入都是恶意的，进行严格验证
3. **渐进式演进** - 持续小规模重构而非大规模重写
4. **全面的测试** - 单元测试、集成测试、压力测试并重

### 失败案例的常见原因

| 类别 | 具体问题 | 预防措施 |
|-----|---------|---------|
| 安全性 | 缓冲区溢出、注入攻击 | 边界检查、参数化查询 |
| 稳定性 | 内存泄漏、死锁 | 代码审查、静态分析 |
| 可维护性 | 代码重复、紧耦合 | 重构、依赖注入 |
| 性能 | 算法复杂度、资源竞争 | 性能测试、剖析优化 |

### 重构最佳实践

1. **先写测试，后重构** - 确保行为不变
2. **小步快跑** - 每次只做一件事
3. **代码审查** - 重构后必须有人审查
4. **性能基准** - 对比重构前后的性能
