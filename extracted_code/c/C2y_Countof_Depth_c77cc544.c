/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1781
 * Language: c
 * Block ID: c77cc544
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 项目配置头文件
#ifdef PROJECT_USE_C23
    #define ARRAY_COUNT _Countof
#else
    #define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

// 在特定文件中启用C23检查
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    // 这些代码可以使用_Countof并享受指针检查
    void modern_function(void) {
        int arr[10];
        size_t n = _Countof(arr);  // 原生支持
    }
#else
    // 这些代码使用兼容层
    void legacy_function(void) {
        int arr[10];
        size_t n = ARRAY_COUNT(arr);  // 兼容宏
    }
#endif
