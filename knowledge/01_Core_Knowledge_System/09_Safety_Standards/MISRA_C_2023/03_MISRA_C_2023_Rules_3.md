# MISRA C:2023 Rules 3.1-3.4 - 注释规范

> 代码文档化、可读性提升、维护性增强

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 3.1 | 注释格式 | Required | 注释语法必须正确 |
| Rule 2.2 | 代码序列化 | Required | 程序必须被正确序列化 |
| Rule 3.2 | 行内注释 | Required | 行内注释有特殊要求 |
| Rule 3.3 | 嵌套注释 | Required | 禁止嵌套注释 |
| Rule 3.4 | 文档注释 | Advisory | 建议文档化所有公共接口 |

---

## Rule 3.1 - 注释格式正确性

### 规则原文

> **Rule 3.1 (Required)**: The character sequences /* and // shall not be used within a comment.
>
> **中文**: 注释中不得使用 /* 和 // 字符序列。

### 详细解释

C标准注释规则：

- `/* */` 块注释不能嵌套
- `//` 行注释到行尾结束
- 注释内出现 `/*` 或 `//` 可能导致意外行为

### 违反示例

```c
/* ❌ 违反 Rule 3.1 - 块注释中包含 /* */
/* This is a comment with /* nested */ marker */
/* 编译器会将第一个 */ 视为注释结束，留下语法错误 */

/* ❌ 违反 Rule 3.1 - 块注释中包含 // */
/* This comment has // inside it */
/* 虽然某些编译器允许，但不符合标准 */

/* ❌ 违反 Rule 3.1 - 行注释中包含代码 */
// Comment with code: x = 5; /* This will cause issues */
/* 行注释中的 /* 不会开始块注释，但代码质量差 */

/* ❌ 违反 - URL在注释中被误解 */
/* Visit http://example.com/path */
/* 这里的 // 可能被某些工具误处理 */
```

### 合规示例

```c
/* ✅ 合规 - 正确的块注释 */
/* This is a proper block comment */

/* ✅ 合规 - 多行块注释 */
/*
 * This is a multi-line comment
 * with proper formatting
 */

/* ✅ 合规 - 安全的URL表示 */
/* Visit: example.com/path (without http://) */

/* ✅ 合规 - 使用协议相对URL */
/* Visit: https://example.com/path */

/* ✅ 合规 - 将URL放在字符串中 */
const char *url = "http://example.com/path";  /* URL in string, not comment */

/* ✅ 合规 - 使用其他方式表示 */
/* Protocol: http, Host: example.com, Path: /path */
```

### 特殊情况处理

```c
/* 方案1: URL放在字符串常量中 */
const char *docs = "http://docs.example.com/api";

/* 方案2: 删除协议前缀 */
/* Documentation: docs.example.com/api/v1 */

/* 方案3: 使用特殊注释格式 */
/**
 * @url https://example.com
 * 使用Doxygen格式，工具能正确处理
 */
```

---

## Rule 3.2 - 代码序列化（Rule 2.2修订）

### 规则原文

> **Rule 3.2 (Required)**: Line-splicing shall not be used in // comments.
>
> **中文**: 不得在行注释（//）中使用行拼接。

### 行拼接是什么

```c
/* 行拼接（Line-splicing）使用反斜杠\在行尾 */
/* 将物理上的多行连接成逻辑上的一行 */

/* 示例：宏定义中使用 */
#define LONG_MACRO \
    (very_long_expression + \
     another_expression)

/* 问题：在 // 注释中使用行拼接 */
```

### 违反示例

```c
/* ❌ 违反 Rule 3.2 - 行注释中的行拼接 */
// This is a comment \
   that continues on the next line

/* 实际效果： */
/* 第1行: // This is a comment that continues on the next line */
/* 第2行: 空的，但可能被视为代码！ */

/* 危险示例 */
int x;  // Variable x \
int y;  // This line is now commented out too!

/* 实际效果： */
/* int x;  // Variable x int y; */
/* 注意：int y; 也被注释掉了！ */
```

### 合规示例

```c
/* ✅ 合规 - 使用块注释跨多行 */
/* This is a comment
 * that spans multiple lines
 * properly */

/* ✅ 合规 - 每行独立的行注释 */
// First part of the comment
// Second part of the comment
// Third part of the comment

/* ✅ 合规 - 使用宏时的正确做法 */
#define SAFE_MACRO do { \
    operation1(); \
    operation2(); \
} while(0)

/* ✅ 合规 - 字符串字面量中的行拼接是允许的 */
const char *msg = "This is a very long string "
                  "that continues on the next line "
                  "and is properly concatenated.";
```

---

## Rule 3.3 - 禁止嵌套注释

### 规则原文

> **Rule 3.3 (Required)**: Neither the // comment token, nor the comment open token /*, shall be used within a comment.
>
> **中文**: 注释中不得使用 // 注释标记或 /* 注释开始标记。

### 本质问题

C语言不支持嵌套注释：

```c
/* 外层注释开始
   /* 试图嵌套内层注释
   这是内层内容
   */  /* 这里被解释为外层注释结束！ */
/* 这行代码会被编译！ */
```

### 违反示例

```c
/* ❌ 违反 Rule 3.3 - 试图嵌套注释 */
/* Outer comment
   /* Inner comment - this will cause problems
   */
   This line is outside any comment and will be compiled!
*/

/* ❌ 违反 - 注释中包含代码块 */
/*
 * Example code:
 * if (x > 0) {
 *     return x; /* return positive */
 * }
 * 上面的 /* 结束了整个注释！
 */

/* ❌ 违反 - 条件编译在注释中 */
/*
 * #ifdef DEBUG
 * printf("Debug: %d\n", x);
 * #endif
 * 虽然这不一定是错误，但代码质量差
 */
```

### 合规示例

```c
/* ✅ 合规 - 使用条件编译代替注释代码 */
#ifdef COMMENTED_OUT_CODE
    /* This code is temporarily disabled */
    if (x > 0) {
        return x;  /* return positive */
    }
#endif

/* ✅ 合规 - 使用版本控制 */
/* Old implementation removed, see git history */
/* Git commit: abc123 for previous version */

/* ✅ 合规 - 文档化删除 */
/* REMOVED: Old function process_data()
 * Reason: Replaced with process_data_v2()
 * Date: 2024-01-15
 * Author: John Doe
 */

/* ✅ 合规 - 使用 #if 0 */
#if 0  /* Disabled code block */
    /* This entire block is safely disabled */
    int old_function(void) {
        /* Can safely use /* */ inside */
        return 0;
    }
#endif
```

### 注释代码的最佳实践

```c
/* 方法1: #if 0（推荐用于大块代码）*/
#if 0  /* LEGACY: Old implementation */
/*
 * 这里可以安全地使用任何注释
 * /* 嵌套注释 */
 * // 行注释
 */
void legacy_function(void) {
    /* ... */
}
#endif

/* 方法2: 删除并记录（推荐）*/
/* DELETED: legacy_function()
 * Commit: a1b2c3d
 * Reason: Replaced by new_function()
 */

/* 方法3: 条件编译（用于可选功能）*/
#ifdef ENABLE_DEPRECATED_API
void deprecated_function(void);
#endif
```

---

## Rule 3.4 - 文档注释（Advisory）

### 规则原文

> **Rule 3.4 (Advisory)**: All public interfaces should be documented.
>
> **中文**: 所有公共接口都应该被文档化。

### 文档化要求

**必须文档化的内容**:

1. 公共API函数（非static）
2. 头文件中定义的宏
3. 结构体/联合体/枚举类型
4. 全局变量（如果必须使用）

### 推荐文档格式（Doxygen风格）

```c
/**
 * @file sensor_driver.h
 * @brief Temperature and humidity sensor driver
 * @author Your Name
 * @date 2024-01-15
 * @version 1.0
 *
 * This driver provides an interface to the BME280
 * temperature, humidity, and pressure sensor.
 */

#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Sensor operation result codes
 */
enum sensor_result {
    SENSOR_OK = 0,           /**< Operation successful */
    SENSOR_ERROR_INIT,       /**< Initialization failed */
    SENSOR_ERROR_READ,       /**< Read operation failed */
    SENSOR_ERROR_TIMEOUT,    /**< Communication timeout */
    SENSOR_ERROR_CRC         /**< CRC check failed */
};

/**
 * @brief Sensor configuration structure
 *
 * Configuration parameters for the sensor.
 * Use sensor_default_config() for default values.
 */
struct sensor_config {
    uint8_t i2c_address;     /**< I2C device address (0x76 or 0x77) */
    uint32_t timeout_ms;     /**< Communication timeout in milliseconds */
    bool oversampling;       /**< Enable oversampling for better accuracy */
};

/**
 * @brief Sensor data structure
 */
struct sensor_data {
    float temperature;       /**< Temperature in Celsius */
    float humidity;          /**< Relative humidity in percentage */
    float pressure;          /**< Pressure in hPa */
    uint32_t timestamp;      /**< Reading timestamp */
};

/**
 * @brief Get default sensor configuration
 *
 * @return Default configuration with:
 *         - i2c_address: 0x76
 *         - timeout_ms: 1000
 *         - oversampling: true
 */
struct sensor_config sensor_default_config(void);

/**
 * @brief Initialize the sensor
 *
 * @param[in] config Pointer to configuration structure
 * @return SENSOR_OK on success, error code otherwise
 *
 * @note Must be called before any other sensor functions
 * @warning This function is not thread-safe
 *
 * Example usage:
 * @code
 * struct sensor_config cfg = sensor_default_config();
 * cfg.timeout_ms = 2000;
 *
 * enum sensor_result rc = sensor_init(&cfg);
 * if (rc != SENSOR_OK) {
 *     printf("Init failed: %d\n", rc);
 *     return -1;
 * }
 * @endcode
 */
enum sensor_result sensor_init(const struct sensor_config *config);

/**
 * @brief Read sensor data
 *
 * @param[out] data Pointer to data structure to fill
 * @return SENSOR_OK on success, error code otherwise
 *
 * @pre sensor_init() must have been called successfully
 * @post data->timestamp is set to current time
 *
 * @code
 * struct sensor_data data;
 * if (sensor_read(&data) == SENSOR_OK) {
 *     printf("Temp: %.2f C\n", data.temperature);
 * }
 * @endcode
 */
enum sensor_result sensor_read(struct sensor_data *data);

/**
 * @brief Deinitialize the sensor
 *
 * Releases resources allocated by sensor_init().
 * Safe to call even if sensor_init() failed.
 */
void sensor_deinit(void);

#endif /* SENSOR_DRIVER_H */
```

### Doxygen标记速查表

| 标记 | 用途 | 示例 |
|:-----|:-----|:-----|
| `@file` | 文件描述 | `@file driver.c` |
| `@brief` | 简短描述 | `@brief Initialize driver` |
| `@param` | 参数说明 | `@param[in] addr I2C address` |
| `@return` | 返回值 | `@return 0 on success` |
| `@note` | 注意事项 | `@note Not thread-safe` |
| `@warning` | 警告 | `@warning May block` |
| `@pre` | 前置条件 | `@pre Driver initialized` |
| `@post` | 后置条件 | `@post Resource freed` |
| `@code` | 代码示例 | `@code ... @endcode` |
| `@see` | 相关参考 | `@see sensor_read()` |

---

## 综合最佳实践

### 注释风格指南

```c
/*
 * 文件头注释
 * ==========
 * 每个文件应该有文件头注释
 */

/**
 * @file example.c
 * @brief Brief description of this file
 * @copyright Copyright (c) 2024
 */

/*
 * 节注释（分隔不同功能区域）
 */
/*===========================================================================*/
/* Private constants                                                         */
/*===========================================================================*/

static const int MAX_BUFFER_SIZE = 1024;

/*===========================================================================*/
/* Public functions                                                          */
/*===========================================================================*/

/**
 * @brief Process input data
 *
 * Detailed description of what this function does,
 * its algorithm, and any important details.
 *
 * @param[in] input Input data buffer
 * @param[in] len Length of input data
 * @param[out] output Output buffer (must be at least len bytes)
 * @return Number of bytes written to output, or -1 on error
 *
 * @note This function modifies internal state
 * @warning Output buffer must not overlap with input
 */
int process_data(const uint8_t *input, size_t len, uint8_t *output);

/*
 * 行内注释
 */
int calculate(int x, int y)
{
    /* Validate inputs */
    if (x < 0 || y < 0) {
        return -1;  /* Invalid parameter */
    }

    /* Algorithm: use Kahan summation for precision */
    double sum = 0.0;
    double c = 0.0;  /* Compensation for lost low-order bits */

    /* ... */

    return (int)sum;
}
```

### 注释检查清单

```markdown
□ 所有公共函数都有文档注释
□ 复杂算法有实现注释
□ 所有 TODO/FIXME 都有日期和作者
□ 没有嵌套注释
□ 没有注释中的可疑代码
□ URL在注释中格式正确
□ 禁用代码使用 #if 0 而不是注释
```

---

## 总结

| 规则 | 强制 | 核心要求 |
|:----:|:----:|:---------|
| 3.1 | Required | 注释中不出现 `/*` 或 `//` |
| 3.2 | Required | 行注释不使用 `\` 行拼接 |
| 3.3 | Required | 禁止嵌套注释，禁用代码用 `#if 0` |
| 3.4 | Advisory | 公共接口使用Doxygen文档化 |

### 关键要点

1. **注释是文档，不是禁用的手段**
2. **禁用代码用 `#if 0` 或版本控制**
3. **公共API必须文档化**
4. **使用Doxygen自动生成文档**

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 3

---

> **下一章节**: [Rules 4.1-4.2 - 标识符命名规范](./04_MISRA_C_2023_Rules_4.md)
