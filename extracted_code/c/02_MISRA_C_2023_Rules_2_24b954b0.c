/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 276
 * Language: c
 * Block ID: 24b954b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.2 */
void read_sensor(void)
{
    int value = adc_read();  /* 读取但不使用 */
    /* 应该使用value */
}
