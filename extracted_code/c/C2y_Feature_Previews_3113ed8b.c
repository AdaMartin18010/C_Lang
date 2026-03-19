/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 1125
 * Language: c
 * Block ID: 3113ed8b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// N3457: __COUNTER__ (已接受)

// __COUNTER__ 每次求值递增
// 用途1: 生成唯一标识符
#define UNIQUE_NAME(prefix) _paste(prefix, __COUNTER__)
#define _paste(a, b) a##b

int UNIQUE_NAME(var);  // var0
int UNIQUE_NAME(var);  // var1
int UNIQUE_NAME(var);  // var2

// 用途2: 静态断言
#define STATIC_ASSERT(expr) \
    typedef char _paste(_static_assert_, __COUNTER__)[(expr) ? 1 : -1]

// 用途3: 注册表模式
#define REGISTER(func) \
    static void _paste(_reg_, __COUNTER__)(void) __attribute__((constructor)); \
    static void _paste(_reg_, __COUNTER__)(void) { register_function(func); }
