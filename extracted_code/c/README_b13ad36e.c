/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\README.md
 * Line: 245
 * Language: c
 * Block ID: b13ad36e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 枚举类型定义
enum Color {
    RED,        // 0
    GREEN = 2,  // 2
    BLUE        // 3
};

enum Status {
    OK = 0,
    ERROR_INVALID = -1,
    ERROR_NOMEM = -2,
    ERROR_IO = -3
};

// 匿名枚举作为命名常量
enum {
    BUFFER_SIZE = 4096,
    MAX_FILENAME = 256,
    MAX_PATH = 4096
};

// typedef 创建类型别名
typedef enum Color Color;
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

// 现代类型推导 (C23 _Generic 模拟)
#define TYPE_OF(x) _Generic((x), \
    int: "int", \
    double: "double", \
    char*: "string", \
    default: "other")

// auto 类型推导 (C23)
// auto x = 42;  // x 推导为 int
