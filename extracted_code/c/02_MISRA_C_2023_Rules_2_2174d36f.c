/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 138
 * Language: c
 * Block ID: 2174d36f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.1 */
void run_forever(void)
{
    while (1) {
        process_data();
    }
    cleanup();  /* 不可达！ */
}
