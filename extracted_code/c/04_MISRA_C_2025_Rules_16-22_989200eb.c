/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 587
 * Language: c
 * Block ID: 989200eb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 符合规范 */
typedef typeof(int) my_int_t;  /* 清晰明了 */

/* 避免过度复杂 */
typedef typeof(typeof(int (*)())[10]) complex_type;  /* 过于复杂 */
