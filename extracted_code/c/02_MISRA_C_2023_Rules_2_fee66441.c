/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 320
 * Language: c
 * Block ID: fee66441
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 当故意忽略返回值时 */
#define IGNORE_RESULT(x) ((void)(x))

void example(void)
{
    IGNORE_RESULT(memcpy(dst, src, len));  /* 明确忽略 */
}
