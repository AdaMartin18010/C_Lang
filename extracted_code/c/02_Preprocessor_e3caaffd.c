/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\02_Preprocessor.md
 * Line: 55
 * Language: c
 * Block ID: e3caaffd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 危险宏
define SQUARE(x) x * x
SQUARE(5 + 1);  // 5 + 1 * 5 + 1 = 11

// ✅ 安全宏：括号保护
define SQUARE(x) ((x) * (x))

// ❌ 多语句宏陷阱
#define SWAP(a, b) int t = a; a = b; b = t;
if (x < y)
    SWAP(x, y);  // 只执行第一句
else
    ...

// ✅ 使用do-while(0)惯用法
define SWAP(a, b) do { \
    typeof(a) _t = (a); \
    (a) = (b); \
    (b) = _t; \
} while(0)

// ❌ 副作用问题
define MAX(a, b) ((a) > (b) ? (a) : (b))
int x = 5, y = 3;
int m = MAX(x++, y++);  // x++执行两次！

// ✅ 使用GCC扩展避免多次求值（非标准）
#ifdef __GNUC__
    #define MAX(a, b) ({ \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a > _b ? _a : _b; \
    })
#else
    // 标准版本：注意副作用
    #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
