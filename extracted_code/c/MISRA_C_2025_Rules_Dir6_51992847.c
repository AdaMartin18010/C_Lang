/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 298
 * Language: c
 * Block ID: 51992847
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 对象定义规则 */

/* ✅ 合规 [待官方确认] - 常量定义 */
#define MAX_SIZE 100  /* 预处理器常量 */

/* [C23相关] - constexpr对象 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    constexpr int MAX_SIZE = 100;  /* C23编译时常量 */
#else
    static const int MAX_SIZE = 100;  /* C11/C17 */
#endif
