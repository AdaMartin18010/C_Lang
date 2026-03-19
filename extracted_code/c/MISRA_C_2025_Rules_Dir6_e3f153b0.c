/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 350
 * Language: c
 * Block ID: e3f153b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 变量必须初始化 */

/* ❌ 违反 [待官方确认] */
int x;  /* 未初始化 */
use(x);  /* 未定义行为 */

/* ✅ 合规 [待官方确认] */
int x = 0;  /* 明确初始化 */
int y = calculate();  /* 函数初始化 */

/* [C23相关] - constexpr初始化 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    constexpr int arr[] = {1, 2, 3};  /* C23编译时初始化 */
#else
    static const int arr[] = {1, 2, 3};  /* C11/C17 */
#endif
