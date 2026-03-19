/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 216
 * Language: c
 * Block ID: 9cd738e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 字符串常量使用规范 */

/* ✅ 合规 [待官方确认] - 标准字符串 */
const char *str = "Hello World";

/* ✅ 合规 [待官方确认] - 宽字符串 */
const wchar_t *wstr = L"Hello World";

/* [C23相关] - UTF-8字符串 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* C23 UTF-8字符串 */
    /* const char8_t *u8str = u8"Hello World"; */  /* [待确认] */
#endif

/* ❌ 违反 [待官方确认] - 修改字符串常量 */
char *modifiable = "Hello";  /* 危险：修改未定义行为 */
modifiable[0] = 'h';  /* 崩溃 */

/* ✅ 合规 [待官方确认] - 使用数组 */
char modifiable[] = "Hello";  /* 可修改的副本 */
