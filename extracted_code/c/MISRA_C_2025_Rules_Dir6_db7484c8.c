/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 115
 * Language: c
 * Block ID: db7484c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 对象声明必须明确 */

/* ❌ 违反 [待官方确认] */
extern int x;  /* 声明但未定义，需确保在其他地方定义 */
/* 如果未定义则链接错误 */

/* ✅ 合规 [C23相关] - 使用constexpr */
#if __STDC_VERSION__ >= 202311L
    constexpr int MAX_SIZE = 100;  /* C23编译时常量 */
#else
    #define MAX_SIZE 100
#endif
