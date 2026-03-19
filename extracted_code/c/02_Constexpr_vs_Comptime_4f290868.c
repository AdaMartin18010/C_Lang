/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 2814
 * Language: c
 * Block ID: 4f290868
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 容器宏
#define DEFINE_STACK(T, NAME, CAP) \
    typedef struct { \
        T data[CAP]; \
        int top; \
    } NAME; \
    static inline void NAME##_init(NAME* s) { s->top = -1; } \
    static inline int NAME##_push(NAME* s, T val) { \
        if (s->top >= CAP - 1) return -1; \
        s->data[++s->top] = val; \
        return 0; \
    } \
    static inline T NAME##_pop(NAME* s) { \
        return s->data[s->top--]; \
    }

DEFINE_STACK(int, IntStack, 100)
DEFINE_STACK(double, DoubleStack, 50)
