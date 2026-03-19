/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\DO_178C\README.md
 * Line: 47
 * Language: c
 * Block ID: 6ddcb219
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 示例：MC/DC要求每个条件独立影响结果 */
if ((A && B) || C) {
    /* ... */
}

/* 需要测试用例：
 * 1. A=T, B=T, C=F -> 结果=T
 * 2. A=F, B=T, C=F -> 结果=F (A独立影响)
 * 3. A=T, B=F, C=F -> 结果=F (B独立影响)
 * 4. A=F, B=F, C=T -> 结果=T (C独立影响)
 */
