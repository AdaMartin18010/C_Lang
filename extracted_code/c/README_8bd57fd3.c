/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\Formal_Verification\README.md
 * Line: 339
 * Language: c
 * Block ID: 8bd57fd3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 可被 SMT 自动验证的简单性质
/*@ ensures \result == x + y; */
int add(int x, int y) {
    return x + y;
}
