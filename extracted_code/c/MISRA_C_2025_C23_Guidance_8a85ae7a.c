/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 478
 * Language: c
 * Block ID: 8a85ae7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] - nodiscard */
    [[nodiscard]] int allocate_resource(void);
    /* 忽略返回值时编译器警告 */

    /* ✅ 推荐 [待官方确认] - maybe_unused */
    [[maybe_unused]] int debug_var = 0;
    /* 明确可能未使用，抑制警告 */

    /* ✅ 推荐 [待官方确认] - deprecated */
    [[deprecated("Use new_function() instead")]]
    int old_function(void);

    /* ✅ 推荐 [待官方确认] - noreturn */
    [[noreturn]] void fatal_error(const char *msg);

    /* ✅ 推荐 [待官方确认] - fallthrough */
    switch (cmd) {
        case CMD_A:
            prepare();
            [[fallthrough]];  /* 明确fall-through */
        case CMD_B:
            execute();
            break;
    }

#else
    /* C11/C17 - 使用编译器扩展 */
    #ifdef __GNUC__
        #define NODISCARD __attribute__((warn_unused_result))
        #define MAYBE_UNUSED __attribute__((unused))
        #define DEPRECATED(msg) __attribute__((deprecated(msg)))
        #define NORETURN __attribute__((noreturn))
    #elif defined(_MSC_VER)
        #define NODISCARD _Check_return_
        #define MAYBE_UNUSED
        #define DEPRECATED(msg) __declspec(deprecated(msg))
        #define NORETURN __declspec(noreturn)
    #else
        #define NODISCARD
        #define MAYBE_UNUSED
        #define DEPRECATED(msg)
        #define NORETURN
    #endif
#endif
