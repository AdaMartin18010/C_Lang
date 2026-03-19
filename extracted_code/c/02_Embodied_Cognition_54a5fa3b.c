/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 236
 * Language: c
 * Block ID: 54a5fa3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 函数调用追踪
#include <stdio.h>
#include <time.h>

#ifdef DEBUG
    #define TRACE_ENTER() \
        do { \
            static int depth = 0; \
            printf("%*s→ %s (%s:%d)\n", depth * 2, "", \
                   __func__, __FILE__, __LINE__); \
            depth++; \
        } while(0)

    #define TRACE_EXIT() \
        do { \
            static int depth = 0; \
            depth--; \
            printf("%*s← %s\n", depth * 2, "", __func__); \
        } while(0)
#else
    #define TRACE_ENTER()
    #define TRACE_EXIT()
#endif

// 使用示例
int factorial(int n) {
    TRACE_ENTER();
    int result = (n <= 1) ? 1 : n * factorial(n - 1);
    TRACE_EXIT();
    return result;
}

// 输出：
// → factorial (test.c:25)
//   → factorial (test.c:25)
//     → factorial (test.c:25)
//     ← factorial
//   ← factorial
// ← factorial
