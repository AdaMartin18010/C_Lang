# 系统架构设计


---

## 📑 目录

- [系统架构设计](#系统架构设计)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [1. 分层架构 (Layered Architecture)](#1-分层架构-layered-architecture)
    - [1.1 架构原理](#11-架构原理)
    - [1.2 C语言实现示例](#12-c语言实现示例)
    - [1.3 分层架构优势](#13-分层架构优势)
  - [2. 微内核设计 (Microkernel Architecture)](#2-微内核设计-microkernel-architecture)
    - [2.1 核心概念](#21-核心概念)
    - [2.2 C语言微内核实现框架](#22-c语言微内核实现框架)
    - [2.3 微内核 vs 宏内核](#23-微内核-vs-宏内核)
  - [3. 插件系统 (Plugin System)](#3-插件系统-plugin-system)
    - [3.1 插件架构设计](#31-插件架构设计)
    - [3.2 插件系统C语言实现](#32-插件系统c语言实现)
    - [3.3 示例插件实现](#33-示例插件实现)
  - [4. 配置管理 (Configuration Management)](#4-配置管理-configuration-management)
    - [4.1 配置系统设计](#41-配置系统设计)
    - [4.2 配置管理C语言实现](#42-配置管理c语言实现)
    - [4.3 JSON配置文件示例](#43-json配置文件示例)
  - [5. 架构设计最佳实践](#5-架构设计最佳实践)
    - [5.1 设计原则](#51-设计原则)
    - [5.2 C语言实现技巧](#52-c语言实现技巧)
    - [5.3 推荐书籍与资源](#53-推荐书籍与资源)
  - [总结](#总结)


---

## 概述

系统架构设计是软件工程的基石，决定了系统的可维护性、可扩展性和可靠性。在C语言系统编程中，良好的架构设计尤为重要，因为它直接影响内存管理、性能优化和代码复用。

---

## 1. 分层架构 (Layered Architecture)

### 1.1 架构原理

分层架构将系统划分为多个逻辑层，每层只与相邻层交互，实现关注点分离：

```
┌─────────────────────────────────────────┐
│           应用层 (Application)           │  ← 业务逻辑、用户接口
├─────────────────────────────────────────┤
│           服务层 (Service)               │  ← 核心业务服务
├─────────────────────────────────────────┤
│           数据层 (Data Access)           │  ← 数据持久化、缓存
├─────────────────────────────────────────┤
│           基础设施层 (Infrastructure)     │  ← 硬件抽象、驱动
└─────────────────────────────────────────┘
```

### 1.2 C语言实现示例

```c
/* layer_architecture.h - 分层架构核心定义 */
#ifndef LAYER_ARCHITECTURE_H
#define LAYER_ARCHITECTURE_H

#include <stdint.h>
#include <stdbool.h>

/* 错误码定义 */
typedef enum {
    LAYER_OK = 0,
    LAYER_ERROR_INVALID_PARAM = -1,
    LAYER_ERROR_NOT_INITIALIZED = -2,
    LAYER_ERROR_IO = -3,
    LAYER_ERROR_MEMORY = -4
} layer_error_t;

/* 基础设施层 - 硬件抽象 */
typedef struct {
    int (*init)(void *config);
    int (*read)(uint32_t addr, void *buf, size_t len);
    int (*write)(uint32_t addr, const void *buf, size_t len);
    int (*deinit)(void);
} hal_interface_t;

/* 数据层 - 数据访问对象 */
typedef struct {
    hal_interface_t *hal;
    uint8_t *buffer;
    size_t buffer_size;
    bool initialized;
} data_layer_t;

/* 服务层 - 业务逻辑 */
typedef struct {
    data_layer_t *data;
    int (*process)(void *input, void *output);
} service_layer_t;

/* 应用层 - 用户接口 */
typedef struct {
    service_layer_t *service;
    int (*run)(int argc, char **argv);
} app_layer_t;

/* API声明 */
layer_error_t data_layer_init(data_layer_t *layer, hal_interface_t *hal, size_t buf_size);
layer_error_t data_layer_read(data_layer_t *layer, uint32_t offset, void *buf, size_t len);
layer_error_t service_layer_init(service_layer_t *service, data_layer_t *data);
layer_error_t app_layer_init(app_layer_t *app, service_layer_t *service);

#endif /* LAYER_ARCHITECTURE_H */
```

```c
/* layer_architecture.c - 分层架构实现 */
#include "layer_architecture.h"
#include <stdlib.h>
#include <string.h>

/* 数据层实现 */
layer_error_t data_layer_init(data_layer_t *layer, hal_interface_t *hal, size_t buf_size) {
    if (!layer || !hal || buf_size == 0) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    layer->buffer = malloc(buf_size);
    if (!layer->buffer) {
        return LAYER_ERROR_MEMORY;
    }

    layer->buffer_size = buf_size;
    layer->hal = hal;
    layer->initialized = true;

    /* 初始化硬件抽象层 */
    if (hal->init(NULL) != 0) {
        free(layer->buffer);
        return LAYER_ERROR_IO;
    }

    return LAYER_OK;
}

layer_error_t data_layer_read(data_layer_t *layer, uint32_t offset, void *buf, size_t len) {
    if (!layer || !layer->initialized || !buf) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    if (offset + len > layer->buffer_size) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    /* 通过HAL层读取数据 */
    if (layer->hal->read(offset, buf, len) != 0) {
        return LAYER_ERROR_IO;
    }

    return LAYER_OK;
}

/* 服务层实现 */
layer_error_t service_layer_init(service_layer_t *service, data_layer_t *data) {
    if (!service || !data) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    service->data = data;
    service->process = NULL; /* 由具体业务实现 */

    return LAYER_OK;
}

/* 应用层实现 */
layer_error_t app_layer_init(app_layer_t *app, service_layer_t *service) {
    if (!app || !service) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    app->service = service;
    return LAYER_OK;
}
```

### 1.3 分层架构优势

| 特性 | 说明 |
|------|------|
| 关注点分离 | 每层只负责特定功能，降低复杂度 |
| 可测试性 | 每层可独立单元测试，便于Mock |
| 可替换性 | 实现细节可在不影响其他层的情况下替换 |
| 可维护性 | 修改局限在单层，降低回归风险 |

---

## 2. 微内核设计 (Microkernel Architecture)

### 2.1 核心概念

微内核架构将系统核心功能最小化，其他服务以用户态进程运行：

```
┌─────────────────────────────────────────────────────┐
│  用户态服务: 文件系统 │ 网络协议栈 │ 设备驱动 │ GUI   │
├─────────────────────────────────────────────────────┤
│                   进程间通信 (IPC)                    │
├─────────────────────────────────────────────────────┤
│  微内核核心: 调度器 │ 内存管理 │ 中断处理 │ 基础IPC   │
├─────────────────────────────────────────────────────┤
│                   硬件抽象层 (HAL)                    │
└─────────────────────────────────────────────────────┘
```

### 2.2 C语言微内核实现框架

```c
/* microkernel.h - 微内核架构定义 */
#ifndef MICROKERNEL_H
#define MICROKERNEL_H

#include <stdint.h>
#include <stddef.h>

/* 系统调用号 */
#define SYSCALL_SEND        1
#define SYSCALL_RECEIVE     2
#define SYSCALL_REGISTER    3
#define SYSCALL_UNREGISTER  4

/* 服务ID */
typedef uint32_t service_id_t;
typedef uint32_t message_type_t;

/* 消息结构 */
typedef struct {
    service_id_t sender;
    service_id_t receiver;
    message_type_t type;
    uint32_t payload_size;
    uint8_t payload[];      /* 柔性数组 */
} message_t;

/* 服务描述符 */
typedef struct service {
    service_id_t id;
    const char *name;
    int (*handler)(message_t *msg);
    uint32_t priority;
    struct service *next;
} service_t;

/* 微内核API */
int mk_init(void);
service_id_t mk_register_service(const char *name, int (*handler)(message_t *));
int mk_unregister_service(service_id_t id);
int mk_send_message(service_id_t to, message_t *msg);
int mk_receive_message(message_t *msg, size_t max_size, uint32_t timeout);

/* 消息队列管理 */
typedef struct message_queue {
    message_t *head;
    message_t *tail;
    uint32_t count;
    uint32_t max_count;
} message_queue_t;

int mq_init(message_queue_t *mq, uint32_t max_count);
int mq_send(message_queue_t *mq, message_t *msg);
message_t *mq_receive(message_queue_t *mq);

#endif /* MICROKERNEL_H */
```

```c
/* microkernel.c - 微内核核心实现 */
#include "microkernel.h"
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

/* 全局服务表 */
static service_t *service_list = NULL;
static atomic_uint next_service_id = 1;
static atomic_flag service_lock = ATOMIC_FLAG_INIT;

/* 自旋锁 */
static void spinlock_acquire(atomic_flag *lock) {
    while (atomic_flag_test_and_set(lock)) {
        /* 自旋等待 */
    }
}

static void spinlock_release(atomic_flag *lock) {
    atomic_flag_clear(lock);
}

/* 查找服务 */
static service_t *find_service(service_id_t id) {
    service_t *svc = service_list;
    while (svc) {
        if (svc->id == id) return svc;
        svc = svc->next;
    }
    return NULL;
}

/* 注册服务 */
service_id_t mk_register_service(const char *name, int (*handler)(message_t *)) {
    if (!name || !handler) return 0;

    service_t *svc = calloc(1, sizeof(service_t));
    if (!svc) return 0;

    svc->id = atomic_fetch_add(&next_service_id, 1);
    svc->name = name;
    svc->handler = handler;
    svc->priority = 10; /* 默认优先级 */

    spinlock_acquire(&service_lock);
    svc->next = service_list;
    service_list = svc;
    spinlock_release(&service_lock);

    return svc->id;
}

/* 消息处理循环示例 */
void service_worker_thread(void *arg) {
    service_t *self = (service_t *)arg;
    message_t msg_buffer[16];

    while (1) {
        int ret = mk_receive_message(msg_buffer, sizeof(msg_buffer), 1000);
        if (ret > 0) {
            /* 调用服务处理函数 */
            self->handler(msg_buffer);
        }
    }
}
```

### 2.3 微内核 vs 宏内核

| 特性 | 微内核 | 宏内核 |
|------|--------|--------|
| 核心大小 | 极小 (通常 < 10K 行代码) | 庞大 (Linux > 2000万行) |
| 扩展性 | 动态加载服务 | 编译时确定或模块加载 |
| 可靠性 | 服务崩溃不影响内核 | 内核模块错误可能导致崩溃 |
| 性能 | IPC开销较大 | 直接调用，性能更高 |
| 安全性 | 服务隔离，更安全 | 内核空间共享，风险较高 |

---

## 3. 插件系统 (Plugin System)

### 3.1 插件架构设计

插件系统允许动态扩展功能，核心应用与插件之间通过明确定义的接口交互：

```
┌─────────────────────────────────────────────────────────┐
│                    主应用程序 (Host)                      │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │   插件管理器     │  │   接口注册中心   │              │
│  │  (动态加载/卸载) │  │  (API暴露/查找) │              │
│  └────────┬────────┘  └─────────────────┘              │
└───────────┼─────────────────────────────────────────────┘
            │ dlopen/dlsym
┌───────────┴─────────────────────────────────────────────┐
│                      插件层 (Plugins)                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐              │
│  │ 日志插件  │  │ 网络插件  │  │ 加密插件  │  ...        │
│  │ plugin.so│  │ net.so   │  │ crypto.so│              │
│  └──────────┘  └──────────┘  └──────────┘              │
└─────────────────────────────────────────────────────────┘
```

### 3.2 插件系统C语言实现

```c
/* plugin_system.h - 插件系统接口定义 */
#ifndef PLUGIN_SYSTEM_H
#define PLUGIN_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

/* 插件版本信息 */
#define PLUGIN_API_VERSION  0x00010002  /* 1.2 */
#define PLUGIN_MAGIC        0x504C5547  /* "PLUG" */

/* 插件类型 */
typedef enum {
    PLUGIN_TYPE_GENERIC = 0,
    PLUGIN_TYPE_LOGGER,
    PLUGIN_TYPE_NETWORK,
    PLUGIN_TYPE_CRYPTO,
    PLUGIN_TYPE_CUSTOM = 0x100
} plugin_type_t;

/* 插件信息结构 */
typedef struct {
    uint32_t magic;          /* 魔数验证 */
    uint32_t api_version;    /* API版本 */
    plugin_type_t type;      /* 插件类型 */
    const char *name;        /* 插件名称 */
    const char *version;     /* 插件版本 */
    const char *author;      /* 作者 */
    const char *description; /* 描述 */
} plugin_info_t;

/* 插件接口结构 - 每个插件必须实现 */
typedef struct {
    plugin_info_t info;

    /* 生命周期函数 */
    int (*init)(void *host_context);    /* 初始化 */
    int (*deinit)(void);                /* 清理 */
    int (*configure)(const char *key, const char *value); /* 配置 */

    /* 能力查询 */
    bool (*has_capability)(const char *cap);
    int (*invoke)(const char *method, void *args, void *result);
} plugin_interface_t;

/* 宿主提供的API */
typedef struct {
    void *(*malloc)(size_t size);
    void (*free)(void *ptr);
    void *(*realloc)(void *ptr, size_t size);

    /* 日志接口 */
    void (*log_debug)(const char *fmt, ...);
    void (*log_info)(const char *fmt, ...);
    void (*log_warn)(const char *fmt, ...);
    void (*log_error)(const char *fmt, ...);

    /* 事件系统 */
    int (*register_event)(const char *event, void (*callback)(void *));
    int (*trigger_event)(const char *event, void *data);

    /* 配置存储 */
    const char *(*get_config)(const char *key);
    int (*set_config)(const char *key, const char *value);
} host_api_t;

/* 插件管理器 */
typedef struct plugin_manager plugin_manager_t;

plugin_manager_t *pm_create(const host_api_t *host_api);
void pm_destroy(plugin_manager_t *pm);
int pm_load_plugin(plugin_manager_t *pm, const char *path);
int pm_unload_plugin(plugin_manager_t *pm, const char *name);
int pm_init_all(plugin_manager_t *pm);
plugin_interface_t *pm_get_plugin(plugin_manager_t *pm, const char *name);
int pm_list_plugins(plugin_manager_t *pm, plugin_info_t **info_array, int max_count);

/* 插件导出符号 - 必须实现 */
#define PLUGIN_EXPORT __attribute__((visibility("default")))

/* 插件入口点宏 */
#define DEFINE_PLUGIN(plugin_struct) \
    PLUGIN_EXPORT const plugin_interface_t *get_plugin_interface(void) { \
        return &(plugin_struct); \
    }

#endif /* PLUGIN_SYSTEM_H */
```

```c
/* plugin_system.c - 插件管理器实现 */
#include "plugin_system.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 已加载插件节点 */
typedef struct plugin_node {
    plugin_interface_t *interface;
    void *handle;           /* dlopen句柄 */
    char path[256];
    bool initialized;
    struct plugin_node *next;
} plugin_node_t;

struct plugin_manager {
    host_api_t host_api;
    plugin_node_t *plugins;
    int plugin_count;
};

plugin_manager_t *pm_create(const host_api_t *host_api) {
    plugin_manager_t *pm = calloc(1, sizeof(plugin_manager_t));
    if (pm && host_api) {
        pm->host_api = *host_api;
    }
    return pm;
}

int pm_load_plugin(plugin_manager_t *pm, const char *path) {
    if (!pm || !path) return -1;

    void *handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "Failed to load plugin %s: %s\n", path, dlerror());
        return -1;
    }

    /* 获取插件接口 */
    const plugin_interface_t *(*get_interface)(void) =
        dlsym(handle, "get_plugin_interface");

    if (!get_interface) {
        fprintf(stderr, "Plugin %s missing get_plugin_interface\n", path);
        dlclose(handle);
        return -1;
    }

    const plugin_interface_t *iface = get_interface();

    /* 验证插件 */
    if (!iface || iface->info.magic != PLUGIN_MAGIC) {
        fprintf(stderr, "Plugin %s has invalid magic\n", path);
        dlclose(handle);
        return -1;
    }

    if (iface->info.api_version != PLUGIN_API_VERSION) {
        fprintf(stderr, "Plugin %s API version mismatch\n", path);
        dlclose(handle);
        return -1;
    }

    /* 创建节点 */
    plugin_node_t *node = calloc(1, sizeof(plugin_node_t));
    node->interface = (plugin_interface_t *)iface;
    node->handle = handle;
    strncpy(node->path, path, sizeof(node->path) - 1);

    /* 添加到列表 */
    node->next = pm->plugins;
    pm->plugins = node;
    pm->plugin_count++;

    printf("Loaded plugin: %s v%s (%s)\n",
           iface->info.name, iface->info.version, iface->info.description);

    return 0;
}

int pm_init_all(plugin_manager_t *pm) {
    plugin_node_t *node = pm->plugins;
    while (node) {
        if (!node->initialized && node->interface->init) {
            if (node->interface->init(&pm->host_api) == 0) {
                node->initialized = true;
            } else {
                fprintf(stderr, "Failed to initialize plugin %s\n",
                        node->interface->info.name);
            }
        }
        node = node->next;
    }
    return 0;
}
```

### 3.3 示例插件实现

```c
/* sample_logger_plugin.c - 示例日志插件 */
#include "plugin_system.h"
#include <stdio.h>
#include <time.h>

static const char *log_file_path = "/tmp/plugin.log";
static FILE *log_fp = NULL;

static int logger_init(void *host_context) {
    log_fp = fopen(log_file_path, "a");
    if (!log_fp) return -1;

    fprintf(log_fp, "=== Logger plugin initialized ===\n");
    fflush(log_fp);
    return 0;
}

static int logger_deinit(void) {
    if (log_fp) {
        fprintf(log_fp, "=== Logger plugin deinitialized ===\n");
        fclose(log_fp);
        log_fp = NULL;
    }
    return 0;
}

static bool logger_has_capability(const char *cap) {
    return (strcmp(cap, "logging") == 0 ||
            strcmp(cap, "file_output") == 0);
}

static int logger_invoke(const char *method, void *args, void *result) {
    if (strcmp(method, "log") == 0 && log_fp) {
        const char *msg = (const char *)args;
        time_t now = time(NULL);
        fprintf(log_fp, "[%s] %s\n", ctime(&now), msg);
        fflush(log_fp);
        return 0;
    }
    return -1;
}

/* 插件接口定义 */
static const plugin_interface_t logger_plugin = {
    .info = {
        .magic = PLUGIN_MAGIC,
        .api_version = PLUGIN_API_VERSION,
        .type = PLUGIN_TYPE_LOGGER,
        .name = "file_logger",
        .version = "1.0.0",
        .author = "System Architect",
        .description = "File-based logging plugin"
    },
    .init = logger_init,
    .deinit = logger_deinit,
    .configure = NULL,
    .has_capability = logger_has_capability,
    .invoke = logger_invoke
};

/* 导出插件 */
DEFINE_PLUGIN(logger_plugin);
```

---

## 4. 配置管理 (Configuration Management)

### 4.1 配置系统设计

```
┌──────────────────────────────────────────────────────────┐
│                   配置管理器 (Config Manager)              │
├──────────────────────────────────────────────────────────┤
│  配置源:  环境变量 │ 配置文件 │ 命令行参数 │ 远程配置中心   │
├──────────────────────────────────────────────────────────┤
│  优先级:  命令行 > 环境变量 > 远程配置 > 配置文件 > 默认值   │
├──────────────────────────────────────────────────────────┤
│  功能:   类型转换 │ 验证 │ 热重载 │ 加密存储 │ 配置继承     │
└──────────────────────────────────────────────────────────┘
```

### 4.2 配置管理C语言实现

```c
/* config_manager.h */
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CONFIG_TYPE_STRING = 0,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_FLOAT,
    CONFIG_TYPE_BOOL,
    CONFIG_TYPE_ARRAY,
    CONFIG_TYPE_OBJECT
} config_value_type_t;

typedef struct {
    config_value_type_t type;
    union {
        char *string_val;
        int64_t int_val;
        double float_val;
        bool bool_val;
        struct {
            void **items;
            size_t count;
        } array_val;
    };
} config_value_t;

/* 配置项 */
typedef struct config_item {
    char *key;
    config_value_t value;
    char *source;           /* 配置来源 */
    uint32_t priority;      /* 优先级 */
    bool encrypted;         /* 是否加密 */
    struct config_item *next;
} config_item_t;

/* 配置管理器 */
typedef struct {
    config_item_t *items;
    char *config_dir;
    void (*on_change)(const char *key, config_value_t *old_val, config_value_t *new_val);
} config_manager_t;

/* API */
config_manager_t *cm_create(const char *config_dir);
void cm_destroy(config_manager_t *cm);

/* 加载配置 */
int cm_load_from_file(config_manager_t *cm, const char *path);
int cm_load_from_env(config_manager_t *cm, const char *prefix);
int cm_load_from_args(config_manager_t *cm, int argc, char **argv);

/* 获取/设置配置 */
const config_value_t *cm_get(config_manager_t *cm, const char *key);
int cm_set(config_manager_t *cm, const char *key, const config_value_t *value,
           uint32_t priority, const char *source);

/* 类型安全的获取 */
int64_t cm_get_int(config_manager_t *cm, const char *key, int64_t default_val);
double cm_get_float(config_manager_t *cm, const char *key, double default_val);
bool cm_get_bool(config_manager_t *cm, const char *key, bool default_val);
const char *cm_get_string(config_manager_t *cm, const char *key, const char *default_val);

/* 热重载 */
int cm_watch(config_manager_t *cm);
int cm_reload(config_manager_t *cm);

#endif /* CONFIG_MANAGER_H */
```

### 4.3 JSON配置文件示例

```json
{
    "application": {
        "name": "SystemService",
        "version": "2.1.0",
        "log_level": "info"
    },
    "database": {
        "host": "localhost",
        "port": 5432,
        "pool_size": 10,
        "ssl_enabled": true
    },
    "cache": {
        "type": "redis",
        "ttl_seconds": 3600,
        "cluster": ["node1:6379", "node2:6379", "node3:6379"]
    },
    "features": {
        "enable_metrics": true,
        "enable_tracing": false,
        "rate_limit": 1000
    }
}
```

---

## 5. 架构设计最佳实践

### 5.1 设计原则

1. **单一职责原则 (SRP)**: 每个模块只负责一个功能领域
2. **开闭原则 (OCP)**: 对扩展开放，对修改关闭
3. **依赖倒置原则 (DIP)**: 依赖抽象接口而非具体实现
4. **接口隔离原则 (ISP)**: 客户端不应依赖不需要的接口

### 5.2 C语言实现技巧

```c
/* 前向声明与opaque pointer - 实现信息隐藏 */
typedef struct internal_system system_t;

/* 公开API */
system_t *system_create(const system_config_t *config);
void system_destroy(system_t *sys);
int system_start(system_t *sys);

/* 实现细节仅在.c文件中可见 */
struct internal_system {
    module_a_t *mod_a;
    module_b_t *mod_b;
    state_machine_t *fsm;
    /* ... */
};
```

### 5.3 推荐书籍与资源

| 资源 | 描述 |
|------|------|
| 《Large-Scale C++ Software Design》 | 虽然是C++，但架构原则适用于C |
| 《The Art of Unix Programming》 | Unix设计哲学 |
| Linux Kernel源码 | 宏内核架构经典实现 |
| MINIX 3源码 | 微内核架构教学实现 |

---

## 总结

系统架构设计是构建可维护、可扩展软件系统的关键。通过分层架构实现关注点分离，微内核设计实现核心最小化，插件系统实现动态扩展，配置管理实现灵活部署，可以构建出既健壮又灵活的C语言系统。


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
