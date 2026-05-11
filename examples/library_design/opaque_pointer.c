/*
 * 不透明指针（Opaque Pointer）模式示例
 * 编译: gcc -Wall -Wextra -o opaque_pointer opaque_pointer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===== 头文件部分（api.h） ===== */
/* 只声明类型名，不暴露内部结构 */
typedef struct Buffer buffer_t;

buffer_t *buffer_create(size_t initial);
void buffer_destroy(buffer_t *b);
int buffer_append(buffer_t *b, const char *data, size_t len);
size_t buffer_len(const buffer_t *b);
const char *buffer_data(const buffer_t *b);
void buffer_clear(buffer_t *b);

/* ===== 实现部分（api.c） ===== */
struct Buffer {
    char *data;
    size_t len;
    size_t cap;
};

buffer_t *buffer_create(size_t initial) {
    buffer_t *b = calloc(1, sizeof(buffer_t));
    b->data = malloc(initial);
    b->cap = initial;
    return b;
}

void buffer_destroy(buffer_t *b) {
    if (b) {
        free(b->data);
        free(b);
    }
}

int buffer_append(buffer_t *b, const char *data, size_t len) {
    if (!b || !data) return -1;
    if (b->len + len > b->cap) {
        size_t new_cap = b->cap * 2;
        while (new_cap < b->len + len) new_cap *= 2;
        char *new_data = realloc(b->data, new_cap);
        if (!new_data) return -1;
        b->data = new_data;
        b->cap = new_cap;
    }
    memcpy(b->data + b->len, data, len);
    b->len += len;
    return 0;
}

size_t buffer_len(const buffer_t *b) {
    return b ? b->len : 0;
}

const char *buffer_data(const buffer_t *b) {
    return b ? b->data : NULL;
}

void buffer_clear(buffer_t *b) {
    if (b) b->len = 0;
}

/* ===== 使用示例 ===== */
int main(void) {
    buffer_t *buf = buffer_create(64);
    buffer_append(buf, "Hello, ", 7);
    buffer_append(buf, "World!", 6);
    
    printf("Buffer (%zu bytes): %.*s\n",
           buffer_len(buf), (int)buffer_len(buf), buffer_data(buf));
    
    buffer_clear(buf);
    buffer_append(buf, "Opaque pointer demo", 19);
    printf("Buffer (%zu bytes): %.*s\n",
           buffer_len(buf), (int)buffer_len(buf), buffer_data(buf));
    
    buffer_destroy(buf);
    return 0;
}
