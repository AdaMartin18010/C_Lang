/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 191
 * Language: c
 * Block ID: 538d9f50
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 浮点常量类型明确 */

/* ❌ 违反 [待官方确认] */
float f = 3.14159;  /* double转float，可能警告 */

/* ✅ 合规 [待官方确认] */
float f = 3.14159F;  /* 明确float */
double d = 3.14159;  /* double */
long double ld = 3.14159L;  /* long double */

/* C23十进制浮点 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* _Decimal32 d32 = 3.14159DF; */  /* [待确认] */
#endif
