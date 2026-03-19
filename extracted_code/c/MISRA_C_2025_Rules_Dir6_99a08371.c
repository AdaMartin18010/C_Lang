/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 430
 * Language: c
 * Block ID: 99a08371
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 指针必须初始化 */

/* ❌ 违反 [待官方确认] */
int *p;  /* 未初始化 */
*p = 5;  /* 未定义行为 */

/* ✅ 合规 [C23相关] */
#if __STDC_VERSION__ >= 202311L
    int *p1 = nullptr;  /* C23类型安全空指针 */
#else
    int *p1 = NULL;  /* C11/C17 */
#endif

int *p2 = &variable;  /* 指向有效对象 */
int *p3 = NULL;  /* 明确为空 */

/* [C23相关] - nullptr优势 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* nullptr是std::nullptr_t类型，避免重载混淆 */
    void func(int *p);
    void func(int val);
    func(nullptr);  /* 调用指针版本 */
#endif
