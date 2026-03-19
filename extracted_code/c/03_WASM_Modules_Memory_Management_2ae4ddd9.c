/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 205
 * Language: c
 * Block ID: 2ae4ddd9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// memory_alignment.c
#include <stdint.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

// 对齐要求
/*
类型        对齐要求    大小
char        1           1
short       2           2
int         4           4
long        4           4 (32位) / 8 (64位)
long long   8           8
float       4           4
double      8           8
指针        4           4 (32位 WASM)
*/

// 未对齐的结构体（有填充）
struct Misaligned {
    char a;      // 1 byte + 3 bytes padding
    int b;       // 4 bytes
    char c;      // 1 byte + 3 bytes padding
};  // 总大小: 12 bytes (不是 6!)

// 对齐的结构体（紧凑）
struct Aligned {
    int b;       // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    // 2 bytes padding at end
};  // 总大小: 8 bytes

// 使用 packed 属性（谨慎使用，可能影响性能）
struct Packed {
    char a;
    int b;
    char c;
} __attribute__((packed));  // 总大小: 6 bytes

EMSCRIPTEN_KEEPALIVE
void print_alignment_info() {
    printf("=== 内存对齐信息 ===\n\n");

    printf("基本类型对齐:\n");
    printf("  alignof(char):      %zu\n", alignof(char));
    printf("  alignof(short):     %zu\n", alignof(short));
    printf("  alignof(int):       %zu\n", alignof(int));
    printf("  alignof(long):      %zu\n", alignof(long));
    printf("  alignof(long long): %zu\n", alignof(long long));
    printf("  alignof(float):     %zu\n", alignof(float));
    printf("  alignof(double):    %zu\n", alignof(double));
    printf("  alignof(void*):     %zu\n", alignof(void*));

    printf("\n结构体大小:\n");
    printf("  sizeof(Misaligned): %zu\n", sizeof(struct Misaligned));
    printf("  sizeof(Aligned):    %zu\n", sizeof(struct Aligned));
    printf("  sizeof(Packed):     %zu\n", sizeof(struct Packed));

    printf("\n结构体布局 (Misaligned):\n");
    struct Misaligned m;
    printf("  &m.a: %p (offset: %zu)\n", (void*)&m.a, offsetof(struct Misaligned, a));
    printf("  &m.b: %p (offset: %zu)\n", (void*)&m.b, offsetof(struct Misaligned, b));
    printf("  &m.c: %p (offset: %zu)\n", (void*)&m.c, offsetof(struct Misaligned, c));
}

// 手动对齐分配
EMSCRIPTEN_KEEPALIVE
void* aligned_malloc(size_t size, size_t alignment) {
    // 分配额外空间用于对齐和存储原始指针
    void* raw = malloc(size + alignment + sizeof(void*));
    if (!raw) return NULL;

    // 计算对齐地址
    uintptr_t aligned = ((uintptr_t)raw + sizeof(void*) + alignment - 1)
                        & ~(alignment - 1);

    // 存储原始指针（在对齐地址前）
    ((void**)aligned)[-1] = raw;

    return (void*)aligned;
}

EMSCRIPTEN_KEEPALIVE
void aligned_free(void* ptr) {
    if (ptr) {
        // 获取原始指针并释放
        free(((void**)ptr)[-1]);
    }
}
