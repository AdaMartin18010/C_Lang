/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\README.md
 * Line: 700
 * Language: c
 * Block ID: 004d8703
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译时断言
#define STATIC_ASSERT(expr) _Static_assert(expr, #expr)

// 编译时计算数组大小
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// 编译时最大值
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX3(a, b, c) MAX(MAX(a, b), c)

// 编译时字符串化
#define STR(x) #x
#define XSTR(x) STR(x)

// 编译时连接
#define CAT(a, b) a##b
#define XCAT(a, b) CAT(a, b)

// 生成唯一标识符
#define UNIQUE_ID(prefix) XCAT(prefix##_, __LINE__)

// 编译时表生成
#define TABLE_16(op) \
    op(0), op(1), op(2), op(3), op(4), op(5), op(6), op(7), \
    op(8), op(9), op(10), op(11), op(12), op(13), op(14), op(15)

#define SQUARE(x) ((x) * (x))

static const int squares[] = { TABLE_16(SQUARE) };
// 展开为: {0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225}
