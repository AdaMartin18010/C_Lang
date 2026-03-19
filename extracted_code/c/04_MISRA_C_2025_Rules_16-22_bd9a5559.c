/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 641
 * Language: c
 * Block ID: bd9a5559
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 - 更灵活 */
if (condition) {
    label:
    int x = 5;  /* C23允许 */
    /* ... */
}

/* 但保持可读性 */
if (condition) {
    int x = 5;
    label:
    /* 更清晰的位置 */
}
