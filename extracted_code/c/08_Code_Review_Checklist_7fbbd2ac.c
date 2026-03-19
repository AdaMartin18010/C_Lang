/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 971
 * Language: c
 * Block ID: 7fbbd2ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file naming_conventions.h
 * @brief C代码命名规范示例
 *
 * 命名规范总览：
 * - 变量名: snake_case, 描述性强
 * - 函数名: snake_case, 动词+名词
 * - 宏名: SCREAMING_SNAKE_CASE
 * - 类型名: snake_case_t 或 PascalCase
 * - 常量: kCamelCase 或 全大写
 * - 枚举: 前缀统一
 */

#ifndef NAMING_CONVENTIONS_H
#define NAMING_CONVENTIONS_H

/* ========== 宏命名（全大写+下划线） ========== */

#define MAX_BUFFER_SIZE 1024
#define DEFAULT_TIMEOUT_MS 5000
#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_PATCH 3

/* 带作用域前缀的宏 */
#define CONFIG_FILE_PATH "/etc/myapp/config.conf"
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

/* 条件编译宏 */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

/* ========== 类型命名（snake_case_t） ========== */

typedef struct {
    char name[64];
    int age;
    double salary;
} employee_t;

typedef struct {
    size_t capacity;
    size_t count;
    employee_t *items;
} employee_list_t;

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR_INVALID_PARAM = -1,
    STATUS_ERROR_OUT_OF_MEMORY = -2,
    STATUS_ERROR_NOT_FOUND     = -3,
    STATUS_ERROR_IO            = -4
} operation_status_t;

/* 函数指针类型 */
typedef int (*compare_func_t)(const void *a, const void *b);
typedef void (*callback_func_t)(int event_id, void *user_data);

/* ========== 函数命名（动词_名词） ========== */

/* 获取/设置函数 */
int employee_get_age(const employee_t *emp);
void employee_set_age(employee_t *emp, int age);
const char* employee_get_name(const employee_t *emp);

/* 检查函数 */
bool employee_is_valid(const employee_t *emp);
bool list_is_empty(const employee_list_t *list);
bool string_starts_with(const char *str, const char *prefix);

/* 转换函数 */
int string_to_int(const char *str, int *result);
char* int_to_string(int value, char *buffer, size_t size);
double celsius_to_fahrenheit(double celsius);

/* 分配/释放函数 */
employee_t* employee_create(const char *name, int age);
void employee_destroy(employee_t *emp);
employee_list_t* list_create(size_t initial_capacity);
void list_destroy(employee_list_t *list);

/* 操作函数 */
int list_append(employee_list_t *list, const employee_t *emp);
int list_remove_at(employee_list_t *list, size_t index);
void list_clear(employee_list_t *list);

/* ========== 变量命名 ========== */

void example_function(void) {
    /* 局部变量: snake_case */
    int item_count = 0;
    double total_price = 0.0;
    char file_path[256] = {0};
    bool is_processing = false;

    /* 指针变量: p_ 前缀或明确命名 */
    employee_t *current_employee = NULL;
    char *buffer_ptr = NULL;

    /* 循环变量: 简洁但可识别 */
    for (int i = 0; i < item_count; i++) {           /* 简单循环用 i, j, k */
        for (int row = 0; row < matrix_rows; row++) { /* 嵌套循环用有意义的名称 */
            for (int col = 0; col < matrix_cols; col++) {
                /* ... */
            }
        }
    }

    /* 布尔变量: is_/has_/can_/should_ 前缀 */
    bool is_enabled = true;
    bool has_error = false;
    bool can_write = false;
    bool should_retry = true;

    /* 计数器/累加器 */
    int error_count = 0;
    size_t bytes_read = 0;
    double running_total = 0.0;

    (void)current_employee; (void)buffer_ptr;
    (void)is_enabled; (void)has_error;
    (void)can_write; (void)should_retry;
    (void)error_count; (void)bytes_read;
    (void)running_total; (void)file_path;
    (void)is_processing; (void)total_price;
}

/* ========== 全局变量（尽量避免） ========== */

/* 非const全局变量必须加 g_ 前缀 */
static int g_connection_count = 0;
static employee_list_t *g_global_employee_list = NULL;

/* const 全局变量可以用 k 前缀或全大写 */
static const int k_max_retries = 3;
static const char k_default_config[] = "default.conf";

/* ========== 静态函数 ========== */

/* 模块内部静态函数，不需要前缀 */
static int validate_input(const char *input) {
    return (input != NULL && strlen(input) > 0) ? 0 : -1;
}

static void internal_helper(void) {
    /* ... */
}

#endif /* NAMING_CONVENTIONS_H */
