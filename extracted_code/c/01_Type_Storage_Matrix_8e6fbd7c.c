/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\01_Type_Storage_Matrix.md
 * Line: 572
 * Language: c
 * Block ID: 8e6fbd7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// comprehensive_type_analysis.c
#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

// 类型信息结构体
typedef struct {
    const char *name;
    size_t size;
    size_t align;
} type_info_t;

#define TYPE_INFO(t) {#t, sizeof(t), alignof(t)}

// 打印类型信息表
void print_type_table(void) {
    type_info_t types[] = {
        TYPE_INFO(char),
        TYPE_INFO(short),
        TYPE_INFO(int),
        TYPE_INFO(long),
        TYPE_INFO(long long),
        TYPE_INFO(float),
        TYPE_INFO(double),
        TYPE_INFO(long double),
        TYPE_INFO(void*),
        TYPE_INFO(size_t),
        TYPE_INFO(bool),
        TYPE_INFO(wchar_t),
        TYPE_INFO(int8_t),
        TYPE_INFO(int16_t),
        TYPE_INFO(int32_t),
        TYPE_INFO(int64_t),
    };

    int count = sizeof(types) / sizeof(types[0]);

    printf("┌──────────────────┬──────────┬──────────┐\n");
    printf("│ Type             │ Size     │ Align    │\n");
    printf("├──────────────────┼──────────┼──────────┤\n");

    for (int i = 0; i < count; i++) {
        printf("│ %-16s │ %3zu bytes│ %3zu bytes│\n",
               types[i].name, types[i].size, types[i].align);
    }

    printf("└──────────────────┴──────────┴──────────┘\n");
}

// 结构体布局分析器
typedef struct {
    const char *name;
    size_t offset;
    size_t size;
} member_info_t;

#define MEMBER_INFO(s, m) {#m, offsetof(s, m), sizeof(((s*)0)->m)}

// 分析并打印结构体布局
#define ANALYZE_STRUCT(sname, ...) do { \
    member_info_t members[] = {__VA_ARGS__}; \
    int n = sizeof(members) / sizeof(members[0]); \
    size_t total_size = sizeof(sname); \
    size_t max_align = alignof(sname); \
    \
    printf("\n=== " #sname " 布局分析 ===\n"); \
    printf("总大小: %zu bytes, 对齐: %zu bytes\n\n", total_size, max_align); \
    printf("┌──────────────────┬─────────┬─────────┬─────────────────┐\n"); \
    printf("│ Member           │ Offset  │ Size    │ Padding After   │\n"); \
    printf("├──────────────────┼─────────┼─────────┼─────────────────┤\n"); \
    \
    size_t prev_end = 0; \
    for (int i = 0; i < n; i++) { \
        size_t padding = members[i].offset - prev_end; \
        printf("│ %-16s │ %5zu   │ %5zu   │ %5zu bytes     │\n", \
               members[i].name, members[i].offset, members[i].size, padding); \
        prev_end = members[i].offset + members[i].size; \
    } \
    size_t tail_padding = total_size - prev_end; \
    printf("│ %-16s │ %5s   │ %5s   │ %5zu bytes     │\n", \
           "[end]", "-", "-", tail_padding); \
    printf("└──────────────────┴─────────┴─────────┴─────────────────┘\n"); \
} while(0)

// 测试结构体
struct Example {
    char a;
    int b;
    short c;
    double d;
    char e;
};

struct ExampleOptimized {
    double d;
    int b;
    short c;
    char a;
    char e;
};

// 内存可视化
void visualize_memory(void *ptr, size_t size, const char *label) {
    printf("\n=== %s 内存视图 ===\n", label);
    unsigned char *p = ptr;

    for (size_t i = 0; i < size; i += 16) {
        printf("%04zx: ", i);

        // 十六进制
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            printf("%02x ", p[i + j]);
        }

        // 对齐填充
        for (size_t j = size - i; j < 16 && i + j >= size; j++) {
            printf("   ");
        }

        printf(" |");

        // ASCII
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            unsigned char c = p[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }

        printf("|\n");
    }
}

int main(void) {
    printf("========================================\n");
    printf("    C 类型存储完整分析工具\n");
    printf("========================================\n");

    printf("\n=== 基本类型信息表 ===\n");
    print_type_table();

    ANALYZE_STRUCT(struct Example,
        MEMBER_INFO(struct Example, a),
        MEMBER_INFO(struct Example, b),
        MEMBER_INFO(struct Example, c),
        MEMBER_INFO(struct Example, d),
        MEMBER_INFO(struct Example, e)
    );

    ANALYZE_STRUCT(struct ExampleOptimized,
        MEMBER_INFO(struct ExampleOptimized, d),
        MEMBER_INFO(struct ExampleOptimized, b),
        MEMBER_INFO(struct ExampleOptimized, c),
        MEMBER_INFO(struct ExampleOptimized, a),
        MEMBER_INFO(struct ExampleOptimized, e)
    );

    // 可视化
    struct Example ex = {'X', 0x12345678, 0xABCD, 3.14159, 'Y'};
    visualize_memory(&ex, sizeof(ex), "struct Example");

    printf("\n=== 数值范围 ===\n");
    printf("CHAR_BIT   = %d\n", CHAR_BIT);
    printf("INT_MIN    = %d\n", INT_MIN);
    printf("INT_MAX    = %d\n", INT_MAX);
    printf("UINT_MAX   = %u\n", UINT_MAX);
    printf("LONG_MIN   = %ld\n", LONG_MIN);
    printf("LONG_MAX   = %ld\n", LONG_MAX);
    printf("FLT_MIN    = %e\n", FLT_MIN);
    printf("FLT_MAX    = %e\n", FLT_MAX);
    printf("DBL_MIN    = %e\n", DBL_MIN);
    printf("DBL_MAX    = %e\n", DBL_MAX);

    return 0;
}
