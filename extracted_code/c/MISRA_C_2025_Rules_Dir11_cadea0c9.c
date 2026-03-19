/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 773
 * Language: c
 * Block ID: cadea0c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23新类型转换指导 [待官方确认] */

#if __STDC_VERSION__ >= 202311L
    /* [待官方确认] - _BitInt转换 */
    _BitInt(17) small = 100;
    int normal = (int)small;  /* 扩展 */

    /* [待官方确认] - typeof使用 */
    int x = 5;
    typeof(x) y = 10;  /* y也是int */

    /* ⚠️ 注意 [待官方确认] - 避免复杂转换 */
    /* typeof()内的复杂表达式 */
    /* typeof(func()[0]) confusing; */  /* 避免 */
#else
    /* C11/C17兼容 */
    int normal = 100;
    int y = 10;
#endif
