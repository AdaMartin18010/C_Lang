/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 287
 * Language: c
 * Block ID: fe317145
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 依赖GCC扩展，其他编译器不兼容
void func(void) {
    __asm__("int $3");  // x86特定
}

// ✅ 使用条件编译
#if defined(__GNUC__) && defined(__x86_64__)
    #define DEBUG_BREAK() __asm__("int $3")
#elif defined(_MSC_VER)
    #define DEBUG_BREAK() __debugbreak()
#else
    #define DEBUG_BREAK() ((void)0)
#endif
