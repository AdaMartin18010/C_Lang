/*
 * C23 static_assert 可选消息示例
 * 编译: gcc -std=c23 -o 09_static_assert 09_static_assert.c
 *       clang -std=c23 -o 09_static_assert 09_static_assert.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// C23 static_assert 消息是可选的

// 基本静态断言（无消息）
static_assert(sizeof(int) >= 4);
static_assert(CHAR_BIT == 8);

// 带消息的静态断言（仍然支持）
static_assert(sizeof(void*) == 8, "64-bit platform required");

// 编译期计算断言
constexpr int max_items = 100;
static_assert(max_items > 0);
static_assert(max_items < 10000);

// 结构体布局断言
struct Packet {
    uint32_t header;
    uint8_t data[256];
    uint32_t checksum;
};

static_assert(sizeof(struct Packet) == 264);
static_assert(offsetof(struct Packet, data) == 4);
static_assert(offsetof(struct Packet, checksum) == 260);

// 类型特性断言
static_assert(sizeof(int32_t) == 4);
static_assert(sizeof(int64_t) == 8);

// 对齐断言
static_assert(alignof(double) >= alignof(int));
static_assert(alignof(max_align_t) >= alignof(double));

// 数组大小断言
int buffer[1024];
static_assert(sizeof(buffer) / sizeof(buffer[0]) == 1024);

// 枚举断言
enum Color {
    RED,
    GREEN,
    BLUE,
    COLOR_COUNT
};

static_assert(COLOR_COUNT == 3);

// 复杂的编译期断言
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

static_assert(factorial(5) == 120);
static_assert(factorial(0) == 1);

// 指针大小断言（条件编译）
#if defined(__x86_64__) || defined(_M_X64)
    static_assert(sizeof(void*) == 8, "Expected 64-bit pointers");
#elif defined(__i386__) || defined(_M_IX86)
    static_assert(sizeof(void*) == 4, "Expected 32-bit pointers");
#endif

// 特性检测断言
#ifdef __STDC_VERSION__
    static_assert(__STDC_VERSION__ >= 202311L);
#endif

int main(void) {
    printf("C23 static_assert Demo\n\n");
    
    printf("All static assertions passed!\n\n");
    
    printf("Verified compile-time checks:\n");
    printf("- int size: %zu bytes (>= 4)\n", sizeof(int));
    printf("- pointer size: %zu bytes\n", sizeof(void*));
    printf("- char bits: %d\n", CHAR_BIT);
    printf("- Packet struct size: %zu bytes\n", sizeof(struct Packet));
    printf("- factorial(5): %d\n", factorial(5));
    
    printf("\nAll assertions verified at compile time!\n");
    
    return 0;
}
