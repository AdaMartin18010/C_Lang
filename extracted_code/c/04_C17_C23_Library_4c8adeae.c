/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\04_C17_C23_Library.md
 * Line: 562
 * Language: c
 * Block ID: 4c8adeae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：假设C23特性一定可用
void unsafe_code(void) {
    int *p = nullptr;  // 可能在旧编译器上编译失败
}

// ✅ 正确：使用特性检测
#if __STDC_VERSION__ >= 202311L
    // C23可用
    void modern_code(void) {
        int *p = nullptr;
    }
#else
    // 回退方案
    void modern_code(void) {
        int *p = NULL;
    }
#endif

// ✅ 或者使用宏封装
#if __STDC_VERSION__ >= 202311L
    #define MY_NULLPTR nullptr
#else
    #define MY_NULLPTR NULL
#endif
