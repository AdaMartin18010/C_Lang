/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 228
 * Language: c
 * Block ID: 3b079f40
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 3.3 - 试图嵌套注释 */
/* Outer comment
   /* Inner comment - this will cause problems
   */
   This line is outside any comment and will be compiled!
*/

/* ❌ 违反 - 注释中包含代码块 */
/*
 * Example code:
 * if (x > 0) {
 *     return x; /* return positive */
 * }
 * 上面的 /* 结束了整个注释！
 */

/* ❌ 违反 - 条件编译在注释中 */
/*
 * #ifdef DEBUG
 * printf("Debug: %d\n", x);
 * #endif
 * 虽然这不一定是错误，但代码质量差
 */
