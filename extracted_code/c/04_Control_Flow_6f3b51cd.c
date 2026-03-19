/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 704
 * Language: c
 * Block ID: 6f3b51cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 危险宏
#define SQUARE(x) x * x
int a = SQUARE(5 + 1);  // 展开为 5 + 1 * 5 + 1 = 11，不是36

// ✅ 安全宏
define SQUARE_SAFE(x) ((x) * (x))

// ❌ 多表达式宏
#define SWAP(a, b) int t = a; a = b; b = t;
if (condition)
    SWAP(x, y);  // 只执行第一句
else
    ...

// ✅ 使用do-while(0)
define SWAP_SAFE(a, b) do { \
    typeof(a) t = (a); \
    (a) = (b); \
    (b) = t; \
} while(0)
