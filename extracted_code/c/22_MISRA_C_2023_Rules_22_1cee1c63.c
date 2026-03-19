/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\22_MISRA_C_2023_Rules_22.md
 * Line: 263
 * Language: c
 * Block ID: 1cee1c63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 复杂度 = 3 */
void func(int a, int b) {
    if (a && b) {        /* +1 for if, +1 for && */
        do_something();
    } else {
        do_other();
    }
}
