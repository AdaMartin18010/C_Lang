/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 2151
 * Language: c
 * Block ID: aea65a25
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * C23 现代 C 程序模板
 * 编译: gcc -std=c23 -Wall -Wextra -O2 -o program program.c
 */

/* 模块导入 (C23 新特性，替代 #include 的实验性特性) */
/* 注意: 当前 GCC 15 仍使用传统 #include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdalign.h>
#include <threads.h>

/* C23: constexpr 用于编译时常量 */
constexpr int MAX_BUFFER_SIZE = 4096;
constexpr double PI_APPROX = 3.14159265358979323846;

/* C23: 属性声明 */
[[nodiscard]] int allocate_resources(void);
[[nodiscard]] int process_data(const char* input);

[[maybe_unused]] static void debug_print(const char* msg) {
    #ifdef DEBUG
    fprintf(stderr, "[DEBUG] %s\n", msg);
    #endif
}

/* C23: static_assert 为原生关键字 */
static_assert(MAX_BUFFER_SIZE >= 1024, "Buffer too small");
static_assert(sizeof(void*) == 8, "64-bit required");

/* C23: typeof 类型推导 */
typedef typeof(int) my_int_t;
typedef typeof(nullptr) null_ptr_t;

/* C23: auto 类型推断 */
auto global_value = 42;  /* int */

/* C23: 对齐说明符为原生关键字 */
alignas(64) char cache_line_buffer[128];

/* C23: thread_local 为原生关键字 */
static thread_local int thread_counter = 0;

/* C23: bool 为原生类型 */
bool is_initialized = false;

/* 结构体定义 */
typedef struct {
    alignas(32) float data[16];  /* 32 字节对齐 */
    size_t count;
    [[maybe_unused]] size_t capacity;
} vector_t;

/* C23: [[nodiscard]] 函数 */
[[nodiscard]] int vector_init(vector_t* vec, size_t initial_capacity) {
    if (!vec) return -1;

    memset_explicit(vec, 0, sizeof(vector_t));
    vec->count = 0;

    (void)initial_capacity;  /* C23: 显式忽略参数 */

    return 0;
}

[[nodiscard]] int vector_push(vector_t* vec, float value) {
    if (!vec || vec->count >= 16) return -1;

    vec->data[vec->count++] = value;
    return 0;
}

void vector_free(vector_t* vec) {
    if (vec) {
        /* C23: memset_explicit 安全清除 */
        memset_explicit(vec, 0, sizeof(vector_t));
    }
}

/* C23: 新数学函数使用 */
double calculate_circle_area(double radius) {
    /* 使用 C23 新常量 */
    return PI_APPROX * radius * radius;
}

/* C23: timespect_getres 使用 */
void print_timer_resolution(void) {
    struct timespec res;
    if (timespec_getres(&res, TIME_UTC) == TIME_UTC) {
        printf("Timer resolution: %ld.%09ld seconds\n",
               (long)res.tv_sec, res.tv_nsec);
    }
}

/* C23: 线程安全时间处理 */
void print_current_time(bool utc) {
    time_t now = time(nullptr);
    struct tm timeinfo;
    char buffer[80];

    if (utc) {
        gmtime_r(&now, &timeinfo);
        strftime(buffer, sizeof(buffer),
                 "%Y-%m-%d %H:%M:%S UTC", &timeinfo);
    } else {
        localtime_r(&now, &timeinfo);
        strftime(buffer, sizeof(buffer),
                 "%Y-%m-%d %H:%M:%S Local", &timeinfo);
    }

    printf("Current time: %s\n", buffer);
}

/* C23: strdup 和 strndup */
char* duplicate_string(const char* source, size_t max_len) {
    if (!source) return nullptr;

    /* C23: strndup 是标准函数 */
    return strndup(source, max_len);
}

/* C23: memccpy */
size_t copy_until_char(char* dest, const char* src,
                       char stop_char, size_t max_len) {
    void* end = memccpy(dest, src, stop_char, max_len);

    if (end) {
        /* 找到停止字符 */
        return (size_t)((char*)end - dest);
    } else {
        /* 未找到，完整复制 */
        dest[max_len - 1] = '\0';
        return strlen(src);
    }
}

/* C23: 新数学函数演示 */
void demonstrate_new_math(void) {
    printf("=== C23 新数学函数 ===\n");

    /* sinpi, cospi: 以 π 为单位的角度 */
    printf("sinpi(0.5) = %f (sin(π/2))\n", sinpi(0.5));
    printf("cospi(1.0) = %f (cos(π))\n", cospi(1.0));

    /* asinpi, acospi: 返回值为 π 的比例 */
    printf("asinpi(0.5) = %f (30°/180° = 1/6)\n", asinpi(0.5));
    printf("acospi(0.5) = %f (60°/180° = 1/3)\n", acospi(0.5));

    /* exp10, exp2m1 等 */
    printf("exp10(3) = %f\n", exp10(3.0));
    printf("logp1(0.001) = %.17g\n", logp1(0.001));
}

/* 主函数 */
int main(int argc, char* argv[]) {
    (void)argc;  /* C23: 显式忽略 */
    (void)argv;

    printf("=== C23 现代 C 演示 ===\n\n");

    /* 显示编译信息 */
    printf("Compiler: GCC %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    printf("C Standard: C%ld\n", __STDC_VERSION__);

    /* 向量演示 */
    printf("\n=== 向量演示 ===\n");
    vector_t vec;
    vector_init(&vec, 16);

    vector_push(&vec, 1.0f);
    vector_push(&vec, 2.0f);
    vector_push(&vec, 3.0f);

    printf("Vector: ");
    for (size_t i = 0; i < vec.count; i++) {
        printf("%f ", vec.data[i]);
    }
    printf("\n");

    vector_free(&vec);

    /* 数学演示 */
    printf("\n=== 数学计算 ===\n");
    printf("Circle area (r=5): %f\n", calculate_circle_area(5.0));

    /* 时间演示 */
    printf("\n=== 时间处理 ===\n");
    print_timer_resolution();
    print_current_time(true);
    print_current_time(false);

    /* 字符串演示 */
    printf("\n=== 字符串处理 ===\n");
    const char* original = "Hello, C23 World!";
    char* copy = duplicate_string(original, 10);
    if (copy) {
        printf("Original: %s\n", original);
        printf("Copy (max 10): %s\n", copy);
        free(copy);
    }

    char buffer[32];
    size_t copied = copy_until_char(buffer, original, ',', sizeof(buffer));
    printf("Copied until ',': '%s' (len=%zu)\n", buffer, copied);

    /* 新数学函数 */
    printf("\n");
    demonstrate_new_math();

    printf("\n=== 完成 ===\n");
    return 0;
}
