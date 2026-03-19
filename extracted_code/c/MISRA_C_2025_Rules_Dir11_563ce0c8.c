/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 270
 * Language: c
 * Block ID: 563ce0c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 nullptr使用指导 [待官方确认] */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] - 使用nullptr */
    int *p = nullptr;  /* 类型安全 */

    /* ✅ 推荐 [待官方确认] - 函数重载场景 */
    void func(int *p);
    void func(int val);
    func(nullptr);  /* 调用指针版本 */

    /* ✅ 推荐 [待官方确认] - 比较 */
    if (p == nullptr) { /* ... */ }
    if (p != nullptr) { /* ... */ }

#else
    /* C11/C17兼容 */
    #ifndef nullptr
        #define nullptr NULL
    #endif
    int *p = nullptr;
#endif
