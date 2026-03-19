/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 775
 * Language: c
 * Block ID: f760ca52
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ========== 基础用法 ==========

#include <emscripten/emscripten.h>

// 简单函数导出
EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {
    return a + b;
}

// 带参数验证的导出函数
EMSCRIPTEN_KEEPALIVE
int safe_divide(int a, int b) {
    if (b == 0) {
        return 0;  // 或设置错误码
    }
    return a / b;
}

// 浮点数运算
EMSCRIPTEN_KEEPALIVE
double calculate_distance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

// ========== 数据指针操作 ==========

// 处理数组
EMSCRIPTEN_KEEPALIVE
int sum_array(int* arr, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

// 修改数组（原地操作）
EMSCRIPTEN_KEEPALIVE
void scale_array(float* arr, int length, float scale) {
    for (int i = 0; i < length; i++) {
        arr[i] *= scale;
    }
}

// 处理二维数组（矩阵）
EMSCRIPTEN_KEEPALIVE
void matrix_multiply(
    const float* A,
    const float* B,
    float* C,
    int m, int n, int p
) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * p + j];
            }
            C[i * p + j] = sum;
        }
    }
}

// ========== 字符串处理 ==========

// 返回静态字符串
EMSCRIPTEN_KEEPALIVE
const char* get_version() {
    return "Library v1.0.0";
}

// 写入字符串到缓冲区
EMSCRIPTEN_KEEPALIVE
int format_message(char* buffer, int buffer_size, int code) {
    const char* msg;
    switch (code) {
        case 0: msg = "Success"; break;
        case 1: msg = "Invalid argument"; break;
        case 2: msg = "Out of memory"; break;
        default: msg = "Unknown error"; break;
    }
    int len = strlen(msg);
    if (len >= buffer_size) len = buffer_size - 1;
    memcpy(buffer, msg, len);
    buffer[len] = '\0';
    return len;
}

// 处理输入字符串
EMSCRIPTEN_KEEPALIVE
int count_vowels(const char* str) {
    int count = 0;
    while (*str) {
        char c = tolower(*str++);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            count++;
        }
    }
    return count;
}

// ========== 结构体操作 ==========

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    float r, g, b, a;
} Color;

// 向量运算
EMSCRIPTEN_KEEPALIVE
float vector_dot(const Vector3* a, const Vector3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

EMSCRIPTEN_KEEPALIVE
void vector_cross(const Vector3* a, const Vector3* b, Vector3* result) {
    result->x = a->y * b->z - a->z * b->y;
    result->y = a->z * b->x - a->x * b->z;
    result->z = a->x * b->y - a->y * b->x;
}

EMSCRIPTEN_KEEPALIVE
float vector_length(const Vector3* v) {
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

// 颜色混合
EMSCRIPTEN_KEEPALIVE
void color_lerp(const Color* a, const Color* b, float t, Color* result) {
    result->r = a->r + (b->r - a->r) * t;
    result->g = a->g + (b->g - a->g) * t;
    result->b = a->b + (b->b - a->b) * t;
    result->a = a->a + (b->a - a->a) * t;
}

// ========== 回调和状态 ==========

// 静态状态管理
static int counter = 0;

EMSCRIPTEN_KEEPALIVE
int increment_counter() {
    return ++counter;
}

EMSCRIPTEN_KEEPALIVE
int get_counter() {
    return counter;
}

EMSCRIPTEN_KEEPALIVE
void reset_counter() {
    counter = 0;
}

// 上下文句柄模式
typedef struct {
    int id;
    void* data;
    size_t size;
} Context;

static Context* contexts[100];
static int next_id = 1;

EMSCRIPTEN_KEEPALIVE
int context_create(size_t size) {
    Context* ctx = (Context*)malloc(sizeof(Context));
    if (!ctx) return 0;

    ctx->data = malloc(size);
    if (!ctx->data) {
        free(ctx);
        return 0;
    }

    ctx->id = next_id++;
    ctx->size = size;
    memset(ctx->data, 0, size);

    contexts[ctx->id] = ctx;
    return ctx->id;
}

EMSCRIPTEN_KEEPALIVE
void context_destroy(int handle) {
    if (handle <= 0 || handle >= 100) return;
    Context* ctx = contexts[handle];
    if (ctx) {
        free(ctx->data);
        free(ctx);
        contexts[handle] = NULL;
    }
}

EMSCRIPTEN_KEEPALIVE
void* context_get_data(int handle) {
    if (handle <= 0 || handle >= 100) return NULL;
    Context* ctx = contexts[handle];
    return ctx ? ctx->data : NULL;
}
