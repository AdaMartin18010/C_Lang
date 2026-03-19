/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 2312
 * Language: c
 * Block ID: 5392fe39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file misra_examples.c
 * @brief MISRA C:2012 合规示例
 */

#include <stdint.h>
#include <stdbool.h>

/* ========== Dir 4.6: 使用typedef定义基本类型 ========== */

/* ❌ 不合规：依赖实现定义的类型 */
/* unsigned int counter; */
/* long timestamp; */

/* ✅ 合规：使用stdint.h类型 */
uint32_t counter;
int64_t timestamp;
uint8_t checksum;

/* ========== Rule 9.1: 变量使用前必须初始化 ========== */

/* ❌ 不合规 */
/* int result; */
/* calculate_value(&result);  // 如果函数失败，result未初始化 */
/* use_result(result); */

/* ✅ 合规 */
int32_t calculate_and_use(void)
{
    int32_t result = 0;  /* 初始化 */

    if (calculate_value(&result) == 0) {
        use_result(result);
    }

    return 0;
}

/* ========== Rule 10.3: 隐式类型转换 ========== */

/* ❌ 不合规 */
/* uint16_t a = 40000; */
/* uint16_t b = 30000; */
/* uint32_t c = a + b;  // 加法在uint16_t溢出 */

/* ✅ 合规 */
void add_example(void)
{
    uint16_t a = 40000U;
    uint16_t b = 30000U;
    uint32_t c = (uint32_t)a + (uint32_t)b;  /* 显式转换 */
    (void)c;
}

/* ========== Rule 14.4: for循环控制变量不可修改 ========== */

/* ❌ 不合规 */
/* for (i = 0; i < 10; i++) { */
/*     if (condition) i += 2;  // 修改循环变量 */
/* } */

/* ✅ 合规 */
void loop_example(void)
{
    for (uint32_t i = 0U; i < 10U; i++) {
        if (some_condition()) {
            break;  /* 使用break而非修改循环变量 */
        }
    }
}

/* ========== Rule 15.5: 函数单出口点 ========== */

/* ❌ 不合规：多个return */
/* int check_value(int x) { */
/*     if (x < 0) return -1; */
/*     if (x > 100) return -2; */
/*     return process(x); */
/* } */

/* ✅ 合规：单一出口 */
int32_t check_value(int32_t x)
{
    int32_t result = 0;

    if (x < 0) {
        result = -1;
    } else if (x > 100) {
        result = -2;
    } else {
        result = process(x);
    }

    return result;
}

/* ========== Rule 17.7: 函数返回值必须使用 ========== */

/* ❌ 不合规 */
/* strcpy(dest, src);  // 返回值被忽略 */
/* malloc(100);        // 返回值被忽略 */

/* ✅ 合规 */
void use_result_example(void)
{
    char dest[100];
    const char *src = "test";

    char *p = strcpy(dest, src);  /* 使用返回值 */
    (void)p;  /* 如果确实不需要，显式转换void */

    void *mem = malloc(100U);
    if (mem != NULL) {
        /* 使用内存 */
        free(mem);
    }
}

/* ========== Rule 21.3: 避免使用malloc/free ========== */

/* MISRA要求嵌入式系统使用静态分配 */
#define MAX_ITEMS 100

static item_t g_item_pool[MAX_ITEMS];
static bool g_item_used[MAX_ITEMS] = {false};

item_t *allocate_item(void)
{
    for (uint32_t i = 0U; i < MAX_ITEMS; i++) {
        if (!g_item_used[i]) {
            g_item_used[i] = true;
            return &g_item_pool[i];
        }
    }
    return NULL;  /* 池耗尽 */
}

void free_item(item_t *item)
{
    if (item != NULL) {
        ptrdiff_t index = item - g_item_pool;
        if (index >= 0 && (size_t)index < MAX_ITEMS) {
            g_item_used[(size_t)index] = false;
        }
    }
}

/* 占位定义 */
typedef struct { int data; } item_t;
int calculate_value(int *p) { (void)p; return 0; }
void use_result(int x) { (void)x; }
bool some_condition(void) { return false; }
int process(int x) { return x; }
#include <stdlib.h>
