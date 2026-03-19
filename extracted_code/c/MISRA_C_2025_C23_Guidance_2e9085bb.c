/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 105
 * Language: c
 * Block ID: 2e9085bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] */
    int *p1 = nullptr;  /* 类型安全 */

    /* ✅ 推荐 [待官方确认] - 函数重载场景 */
    void func(int *p);      /* 版本1 */
    void func(int val);     /* 版本2 */

    func(nullptr);          /* 调用版本1（指针） */
    /* func(NULL); */      /* 可能调用版本2（整数） */

    /* ✅ 推荐 [待官方确认] - 比较 */
    if (p1 == nullptr) { /* ... */ }
    if (p1 != nullptr) { /* ... */ }

    /* ✅ 推荐 [待官方确认] - 作为默认参数 */
    void process(int *data = nullptr);  /* [待确认] */

#else
    /* C11/C17兼容层 */
    #ifndef nullptr
        #define nullptr ((void*)0)
    #endif
    int *p1 = nullptr;
    if (p1 == nullptr) { /* ... */ }
#endif
