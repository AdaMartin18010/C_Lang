/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 616
 * Language: c
 * Block ID: ad7bd6b2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 检测C23
#if __STDC_VERSION__ >= 202311L
    #define C23_AVAILABLE 1
#else
    #define C23_AVAILABLE 0
#endif

// nullptr兼容
#if C23_AVAILABLE
    #define NULLPTR nullptr
#else
    #define NULLPTR NULL
#endif

// 属性兼容
#if C23_AVAILABLE
    #define NODISCARD [[nodiscard]]
#elif defined(__GNUC__)
    #define NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
    #define NODISCARD _Check_return_
#else
    #define NODISCARD
#endif

// 使用
NODISCARD int *allocate_buffer(size_t size);
