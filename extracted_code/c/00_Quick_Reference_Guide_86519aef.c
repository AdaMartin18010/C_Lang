/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\00_Quick_Reference_Guide.md
 * Line: 272
 * Language: c
 * Block ID: 86519aef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 错误 */
int sum = a + b;  /* 可能溢出 */

/* ✅ 修复 */
int sum;
if (b > 0 && a > INT_MAX - b) {
    /* 溢出处理 */
} else if (b < 0 && a < INT_MIN - b) {
    /* 下溢处理 */
} else {
    sum = a + b;
}
