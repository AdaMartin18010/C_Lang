/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 599
 * Language: c
 * Block ID: edae512d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 */
constexpr int MAX_SIZE = 100;  /* 编译时常量 */

/* C17兼容 */
#ifndef constexpr
    #define constexpr const
#endif
