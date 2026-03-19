/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 1413
 * Language: c
 * Block ID: e6444b6b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ========== C 代码: 数据交互接口 ==========

#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <string.h>

// ===== 基本数值类型 =====

// 整数运算
EMSCRIPTEN_KEEPALIVE
int add_integers(int a, int b) {
    return a + b;
}

EMSCRIPTEN_KEEPALIVE
long long multiply_long(long long a, long long b) {
    return a * b;
}

// 浮点数运算
EMSCRIPTEN_KEEPALIVE
double calculate_circle_area(double radius) {
    return 3.14159265359 * radius * radius;
}

EMSCRIPTEN_KEEPALIVE
float vector_length(float x, float y, float z) {
    return sqrt(x*x + y*y + z*z);
}

// ===== 数组操作 =====

// 整数数组求和
EMSCRIPTEN_KEEPALIVE
int sum_int_array(const int* arr, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

// 浮点数数组操作
EMSCRIPTEN_KEEPALIVE
double sum_double_array(const double* arr, int length) {
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

EMSCRIPTEN_KEEPALIVE
void scale_float_array(float* arr, int length, float scale) {
    for (int i = 0; i < length; i++) {
        arr[i] *= scale;
    }
}

// 查找最大值及其索引
EMSCRIPTEN_KEEPALIVE
void find_max_float(const float* arr, int length, float* max_value, int* max_index) {
    if (length <= 0) return;

    *max_value = arr[0];
    *max_index = 0;

    for (int i = 1; i < length; i++) {
        if (arr[i] > *max_value) {
            *max_value = arr[i];
            *max_index = i;
        }
    }
}

// ===== 字符串操作 =====

// 计算字符串长度（字节数）
EMSCRIPTEN_KEEPALIVE
int string_byte_length(const char* str) {
    return strlen(str);
}

// 计算字符串中的字符数（UTF-8）
EMSCRIPTEN_KEEPALIVE
int string_char_count(const char* str) {
    int count = 0;
    while (*str) {
        unsigned char c = *str;
        // 跳过 UTF-8 多字节序列的后续字节
        if ((c & 0xC0) != 0x80) {
            count++;
        }
        str++;
    }
    return count;
}

// 字符串复制
EMSCRIPTEN_KEEPALIVE
int copy_string(const char* source, char* dest, int max_length) {
    int len = strlen(source);
    if (len >= max_length) {
        len = max_length - 1;
    }
    memcpy(dest, source, len);
    dest[len] = '\0';
    return len;
}

// 字符串反转（原地）
EMSCRIPTEN_KEEPALIVE
void reverse_string(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

// 转换为大写
EMSCRIPTEN_KEEPALIVE
void to_uppercase(char* str) {
    while (*str) {
        if (*str >= 'a' && *str <= 'z') {
            *str = *str - 'a' + 'A';
        }
        str++;
    }
}

// ===== 结构体操作 =====

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    float m[16];
} Matrix4;

typedef struct {
    int id;
    float x, y;
    char name[32];
} Entity;

// 向量运算
EMSCRIPTEN_KEEPALIVE
float vector3_dot(const Vector3* a, const Vector3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

EMSCRIPTEN_KEEPALIVE
void vector3_cross(const Vector3* a, const Vector3* b, Vector3* result) {
    result->x = a->y * b->z - a->z * b->y;
    result->y = a->z * b->x - a->x * b->z;
    result->z = a->x * b->y - a->y * b->x;
}

EMSCRIPTEN_KEEPALIVE
void vector3_normalize(Vector3* v) {
    float len = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len > 0) {
        v->x /= len;
        v->y /= len;
        v->z /= len;
    }
}

// 矩阵乘法
EMSCRIPTEN_KEEPALIVE
void matrix4_multiply(const Matrix4* a, const Matrix4* b, Matrix4* result) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += a->m[i * 4 + k] * b->m[k * 4 + j];
            }
            result->m[i * 4 + j] = sum;
        }
    }
}

// 实体操作
EMSCRIPTEN_KEEPALIVE
void entity_update_position(Entity* entity, float dx, float dy) {
    entity->x += dx;
    entity->y += dy;
}

EMSCRIPTEN_KEEPALIVE
void entity_set_name(Entity* entity, const char* name) {
    strncpy(entity->name, name, 31);
    entity->name[31] = '\0';
}

// ===== 内存管理 =====

// 分配内存
EMSCRIPTEN_KEEPALIVE
void* allocate_memory(int size) {
    return malloc(size);
}

// 释放内存
EMSCRIPTEN_KEEPALIVE
void free_memory(void* ptr) {
    free(ptr);
}

// 重新分配内存
EMSCRIPTEN_KEEPALIVE
void* reallocate_memory(void* ptr, int new_size) {
    return realloc(ptr, new_size);
}

// 获取指针指向的值
EMSCRIPTEN_KEEPALIVE
int read_int(const int* ptr) {
    return *ptr;
}

EMSCRIPTEN_KEEPALIVE
double read_double(const double* ptr) {
    return *ptr;
}

EMSCRIPTEN_KEEPALIVE
void write_int(int* ptr, int value) {
    *ptr = value;
}

EMSCRIPTEN_KEEPALIVE
void write_double(double* ptr, double value) {
    *ptr = value;
}

// ===== 回调函数 =====

typedef void (*ProgressCallback)(int current, int total, const char* message);

static ProgressCallback g_progress_callback = NULL;

EMSCRIPTEN_KEEPALIVE
void set_progress_callback(ProgressCallback callback) {
    g_progress_callback = callback;
}

EMSCRIPTEN_KEEPALIVE
void perform_long_operation(int iterations) {
    for (int i = 0; i < iterations; i++) {
        // 模拟工作
        for (volatile int j = 0; j < 100000; j++);

        // 调用进度回调
        if (g_progress_callback) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Processing item %d", i);
            g_progress_callback(i, iterations, msg);
        }
    }
}
