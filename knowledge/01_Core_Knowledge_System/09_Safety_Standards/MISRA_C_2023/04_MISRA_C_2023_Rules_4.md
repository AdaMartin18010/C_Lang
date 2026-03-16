# MISRA C:2023 Rules 4.1-4.5 - 标识符命名与隐藏

> 命名规范、作用域管理、可读性提升

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 4.1 | 标识符长度 | Required | 标识符必须能区分前63个字符 |
| Rule 4.2 | 标识符隐藏 | Required | 不得隐藏其他标识符 |
| Rule 4.3 | 宏名称 | Required | 宏名不得与标识符冲突 |
| Rule 4.4 | 文档标识符 | Advisory | 建议文档化所有标识符 |
| Rule 4.5 | typedef名称 | Advisory | typedef名称应唯一 |

---

## Rule 4.1 - 标识符长度与唯一性

### 规则原文

> **Rule 4.1 (Required)**: Identifiers shall be distinct from macro names.
>
> **中文**: 标识符应与宏名称不同。

### 规则澄清

MISRA C:2023实际上包含多个与标识符相关的规则：
- **Rule 4.1**: 标识符必须唯一（考虑前63个字符）
- **Rule 4.2**: 不得隐藏其他标识符
- **Rule 5.1**: 外部标识符唯一性（考虑前31个字符）

### C11翻译限制

| 标识符类型 | 最小有效长度 | 建议 |
|:-----------|:-------------|:-----|
| 内部标识符 | 63字符 | 保持63字符内唯一 |
| 外部标识符 | 31字符 | 保持31字符内唯一 |

### 违反示例

```c
/* ❌ 违反 - 标识符前63字符相同 */
int analysis_of_sensor_data_from_temperature_probe_in_celsius;
int analysis_of_sensor_data_from_temperature_probe_in_fahrenheit;
/* 前63字符相同，编译器可能视为同一标识符 */

/* ❌ 违反 - 外部标识符前31字符相同 */
void process_temperature_sensor_data(void);
void process_temperature_sensor_value(void);
/* 某些链接器只识别前31字符 */

/* ❌ 违反 - 大小写不敏感系统冲突 */
int MyVariable;
int myVariable;  /* 在不区分大小写的文件系统中冲突 */
```

### 合规示例

```c
/* ✅ 合规 - 简洁明确的命名 */
int temp_celsius;
int temp_fahrenheit;

/* ✅ 合规 - 缩写使用 */
void process_temp_data(void);   /* process_temperature_data */
void process_temp_value(void);  /* process_temperature_value */

/* ✅ 合规 - 命名空间前缀 */
typedef struct {
    int sensor_id;
    float value;
} sensor_temp_t;    /* sensor_ 前缀 */

typedef struct {
    int ctrl_id;
    float setpoint;
} ctrl_param_t;     /* ctrl_ 前缀 */
```

---

## Rule 4.2 - 标识符隐藏

### 规则原文

> **Rule 4.2 (Required)**: Identifiers shall not have both internal and external linkage in the same translation unit.
>
> **中文**: 标识符在同一翻译单元中不得同时具有内部和外部链接。

### 什么是标识符隐藏

```c
/* 外层作用域 */
int count = 0;  /* 全局变量 */

void func(void)
{
    int count = 1;  /* ❌ 隐藏了全局count */
    /* 这里的count是局部变量，全局count被隐藏 */
}
```

### 违反示例

```c
/* ❌ 违反 - 局部变量隐藏全局变量 */
int debug_level = 0;  /* 全局 */

void set_debug_level(void)
{
    int debug_level = 1;  /* 隐藏全局变量 */
    /* 这里修改的是局部变量，不是全局变量！ */
}

/* ❌ 违反 - 参数隐藏全局变量 */
int timeout = 1000;  /* 全局默认超时 */

void init(int timeout)  /* 参数隐藏全局变量 */
{
    /* 内部使用timeout参数，全局timeout不可见 */
}

/* ❌ 违反 - 内部extern声明 */
static int var = 0;  /* 内部链接 */

void func(void)
{
    extern int var;  /* 试图声明外部链接，冲突！ */
}
```

### 合规示例

```c
/* ✅ 合规 - 明确区分 */
int g_debug_level = 0;  /* g_ 前缀表示全局 */

void set_debug_level(void)
{
    int local_debug = 1;  /* 明确不同的名称 */
    g_debug_level = local_debug;  /* 明确赋值给全局变量 */
}

/* ✅ 合规 - 参数使用不同名称 */
int timeout_ms = 1000;

void init(int timeout_value)  /* 不同名称 */
{
    timeout_ms = timeout_value;  /* 明确赋值 */
}

/* ✅ 合规 - 使用指针明确修改全局变量 */
int counter = 0;

void increment(void)
{
    extern int counter;  /* 正确：与全局声明一致 */
    counter++;
}
```

### 标识符隐藏检测工具

```bash
# GCC
 gcc -Wshadow -c file.c

# Clang（更详细）
 clang -Wshadow-all -c file.c

# PC-lint
 pc-lint -e578 -e578 file.c  /* 578: 隐藏了声明 */
```

---

## Rule 4.3 - 宏与标识符冲突

### 规则原文

> **Rule 4.3 (Required)**: A macro shall not be defined with the same name as a keyword before the last token of that translation unit.
>
> **中文**: 宏不得使用与C关键字相同的名称定义。

### 违反示例

```c
/* ❌ 严重违反 - 重定义关键字 */
#define if(x) if((x) != 0)  /* 改变了if的行为！ */
#define while(x) while((x))  /* 危险！ */

/* ❌ 违反 - 宏名与标准库标识符冲突 */
#define printf my_printf  /* 替换标准库函数 */
#define NULL 0  /* 可能改变NULL的定义 */

/* ❌ 违反 - 宏名与类型名冲突 */
typedef int int32_t;
#define int32_t long  /* 与typedef冲突！ */
```

### 合规示例

```c
/* ✅ 合规 - 使用前缀 */
#define MY_DEBUG_PRINT(x) printf("[DEBUG] %s\n", x)
#define MY_NULL_POINTER ((void*)0)

/* ✅ 合规 - 使用大写命名 */
#define BUFFER_SIZE 1024
#define MAX_RETRIES 3

/* ✅ 合规 - 条件编译保护 */
#ifndef MY_HEADER_H
#define MY_HEADER_H
/* ... */
#endif
```

---

## Rule 4.4 - 文档化标识符

### 规则原文

> **Rule 4.4 (Advisory)**: Identifiers should be documented.
>
> **中文**: 标识符应该被文档化。

### 文档化要求

```c
/**
 * @brief Maximum number of sensors supported
 * 
 * This constant defines the maximum number of sensors
 * that can be connected to the system simultaneously.
 * 
 * @note Increasing this value will increase RAM usage
 * @see sensor_init(), sensor_count
 */
#define MAX_SENSORS 16

/**
 * @brief System state enumeration
 * 
 * Defines the possible states of the system state machine.
 */
enum system_state {
    STATE_INIT,      /**< Initializing */
    STATE_IDLE,      /**< Ready for operation */
    STATE_RUNNING,   /**< Normal operation */
    STATE_ERROR,     /**< Error condition */
    STATE_SHUTDOWN   /**< Shutting down */
};

/**
 * @brief Sensor data structure
 * 
 * Contains the data read from a physical sensor.
 * All values are in SI units.
 */
struct sensor_data {
    float temperature;    /**< Temperature in Celsius */
    float humidity;       /**< Relative humidity in % */
    float pressure;       /**< Pressure in hPa */
    uint32_t timestamp;   /**< Unix timestamp of reading */
};
```

---

## Rule 4.5 - Typedef唯一性

### 规则原文

> **Rule 4.5 (Advisory)**: Typedef names should be unique.
>
> **中文**: Typedef名称应该是唯一的。

### 违反示例

```c
/* ❌ 违反 - 相同typedef名称不同定义 */
/* file1.h */
typedef int status_t;

/* file2.h */
typedef enum { OK, ERROR } status_t;  /* 冲突！ */

/* 同时包含时会导致未定义行为 */
```

### 合规示例

```c
/* ✅ 合规 - 使用命名空间前缀 */
/* sensor.h */
typedef enum {
    SENSOR_OK = 0,
    SENSOR_ERROR_INIT,
    SENSOR_ERROR_READ
} sensor_status_t;

/* network.h */
typedef enum {
    NET_OK = 0,
    NET_ERROR_CONNECT,
    NET_ERROR_TIMEOUT
} network_status_t;

/* ✅ 合规 - 统一类型定义 */
/* common_types.h */
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR_GENERIC,
    STATUS_ERROR_INVALID_PARAM,
    STATUS_ERROR_TIMEOUT,
    STATUS_ERROR_NO_MEMORY
} system_status_t;

#endif
```

---

## 命名规范最佳实践

### 命名约定表

| 类型 | 命名风格 | 示例 |
|:-----|:---------|:-----|
| 宏 | 全大写+下划线 | `MAX_BUFFER_SIZE` |
| 常量 | 全大写+下划线 | `PI_VALUE` |
| 枚举 | 小写+下划线 | `enum color_type` |
| 枚举值 | 大写+下划线 | `COLOR_RED` |
| 结构体 | 小写+下划线+_t | `struct sensor_data_t` |
| typedef | 小写+下划线+_t | `sensor_data_t` |
| 函数 | 小写+下划线 | `read_sensor_data()` |
| 变量 | 小写+下划线 | `sensor_value` |
| 全局变量 | g_前缀 | `g_system_status` |
| 静态变量 | s_前缀 | `s_instance_count` |
| 指针变量 | p前缀或_ptr后缀 | `p_data` 或 `data_ptr` |
| 布尔变量 | is_/has_/can_前缀 | `is_initialized` |
| 计数变量 | count/n/total前缀 | `byte_count`, `n_items` |

### 示例代码

```c
/* 完整的命名规范示例 */

#include <stdint.h>
#include <stdbool.h>

/* 常量 */
#define MAX_SENSOR_COUNT    16
#define DEFAULT_TIMEOUT_MS  1000
#define PI_VALUE            3.14159265359

/* 枚举 */
enum sensor_type {
    SENSOR_TYPE_TEMPERATURE = 0,
    SENSOR_TYPE_HUMIDITY,
    SENSOR_TYPE_PRESSURE,
    SENSOR_TYPE_COUNT  /* 总是最后一个，用于计数 */
};

/* 结构体 */
struct sensor_config_t {
    enum sensor_type type;
    uint32_t sample_rate_hz;
    uint32_t timeout_ms;
    bool enable_filtering;
};

/* 类型定义 */
typedef struct sensor_config_t sensor_config_t;
typedef float sensor_value_t;

/* 全局变量 */
static sensor_config_t g_sensor_configs[MAX_SENSOR_COUNT];
static uint32_t g_active_sensor_count = 0;

/* 函数声明 */
system_status_t sensor_init(const sensor_config_t *p_config);
system_status_t sensor_read(sensor_value_t *p_value);
void sensor_deinit(void);

/* 函数实现 */
system_status_t sensor_init(const sensor_config_t *p_config)
{
    /* 参数检查 */
    if (p_config == NULL) {
        return STATUS_ERROR_INVALID_PARAM;
    }
    
    /* 实现 */
    /* ... */
    
    return STATUS_OK;
}
```

---

## 检测工具配置

### GCC检查选项

```bash
# 标识符隐藏检查
gcc -Wshadow -c file.c

# 严格标识符检查
gcc -Wshadow-all -c file.c

# 宏重定义检查
gcc -Wundef -c file.c
```

### PC-lint配置

```bash
# 标识符唯一性检查
pc-lint \
    -e578 \  /* 隐藏了声明 */
    -e628 \  /* 推荐了另一种隐藏 */
    -e744 \  /* 局部符号与全局相同 */
    -e757 \  /* 全局声明未被使用 */
    -e768 \  /* 全局声明未被使用 */
    file.c
```

---

## 总结

| 规则 | 强制 | 核心要求 |
|:----:|:----:|:---------|
| 4.1 | Required | 标识符前63字符唯一 |
| 4.2 | Required | 不得隐藏其他标识符 |
| 4.3 | Required | 宏名不得与关键字冲突 |
| 4.4 | Advisory | 文档化所有标识符 |
| 4.5 | Advisory | typedef名称唯一 |

### 命名检查清单

```markdown
□ 标识符前63字符唯一
□ 外部标识符前31字符唯一
□ 无隐藏标识符（使用-Wshadow检查）
□ 宏名不与关键字冲突
□ 使用一致的命名规范
□ 全局变量有g_前缀
□ 宏使用全大写
□ 函数和变量使用小写+下划线
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 4

---

> **下一章节**: [Rules 5.1-5.9 - 类型系统与转换](./05_MISRA_C_2023_Rules_5.md)
