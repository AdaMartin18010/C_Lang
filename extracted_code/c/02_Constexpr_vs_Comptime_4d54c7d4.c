/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 2731
 * Language: c
 * Block ID: 4d54c7d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 宏定义模式
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// 带类型的泛型宏
#define PRINT_VAL(x) _Generic((x), \
    int: printf("%d\n", x), \
    double: printf("%f\n", x) \
)

// X-Macro 模式
#define COLOR_LIST \
    X(RED, 0xFF0000) \
    X(GREEN, 0x00FF00) \
    X(BLUE, 0x0000FF)

enum Color {
    #define X(name, value) COLOR_##name,
    COLOR_LIST
    #undef X
};
