/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 528
 * Language: c
 * Block ID: 58cccbd7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 允许 [待官方确认] - C23更灵活 */
    if (condition) {
        label:
        int x = 5;  /* C23允许 */
        /* ... */
    }

    /* ✅ 推荐 [待官方确认] - 保持可读性 */
    if (condition) {
        int x = 5;
        label:
        /* 更清晰的位置 */
    }

#else
    /* C11/C17 - 标签必须在语句前 */
    if (condition) {
        int x = 5;
        goto label;  /* 错误：x在作用域内但标签前 */
        label:
        /* ... */
    }
#endif
