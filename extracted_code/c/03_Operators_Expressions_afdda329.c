/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 545
 * Language: c
 * Block ID: afdda329
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 危险宏
define SQUARE(x) ((x) * (x))
int a = 5;
int b = SQUARE(a++);  // ((a++) * (a++))  UB!

// ✅ 使用GCC扩展避免多次求值
#if defined(__GNUC__)
    #define SAFE_SQUARE(x) ({ \
        typeof(x) _x = (x); \
        _x * _x; \
    })
#else
    // 调用者注意：不要有副作用
    #define SAFE_SQUARE(x) ((x) * (x))
#endif
