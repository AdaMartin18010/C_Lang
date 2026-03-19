/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 142
 * Language: c
 * Block ID: fef6d217
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 字符常量使用规范 */

/* ❌ 违反 [待官方确认] */
char c = 'ab';  /* 多字符常量 - 实现定义 */

/* ✅ 合规 [待官方确认] */
char c = 'a';  /* 单字符 */
wchar_t wc = L'a';  /* 宽字符 */

/* [C23相关] - UTF-8字符 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* C23可能有新的字符类型 */
#endif
