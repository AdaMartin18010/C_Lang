/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 650
 * Language: c
 * Block ID: cd592e94
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* c23_compat.h */
#ifndef C23_COMPAT_H
#define C23_COMPAT_H

#if __STDC_VERSION__ >= 202311L
    /* C23 - 使用原生特性 */
#else
    /* C11/C17 - 提供兼容层 */

    /* nullptr */
    #ifndef nullptr
        #define nullptr NULL
    #endif

    /* constexpr */
    #ifndef constexpr
        #define constexpr const
    #endif

    /* typeof - GCC扩展 */
    #if defined(__GNUC__) && !defined(typeof)
        #define typeof __typeof__
    #endif

    /* 属性 */
    #ifndef nodiscard
        #ifdef __GNUC__
            #define nodiscard __attribute__((warn_unused_result))
        #else
            #define nodiscard
        #endif
    #endif

    /* static_assert */
    #ifndef static_assert
        #define static_assert _Static_assert
    #endif
#endif

#endif /* C23_COMPAT_H */
