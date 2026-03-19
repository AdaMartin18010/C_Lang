/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 288
 * Language: c
 * Block ID: 03f887f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 方法1: #if 0（推荐用于大块代码）*/
#if 0  /* LEGACY: Old implementation */
/*
 * 这里可以安全地使用任何注释
 * /* 嵌套注释 */
 * // 行注释
 */
void legacy_function(void) {
    /* ... */
}
#endif

/* 方法2: 删除并记录（推荐）*/
/* DELETED: legacy_function()
 * Commit: a1b2c3d
 * Reason: Replaced by new_function()
 */

/* 方法3: 条件编译（用于可选功能）*/
#ifdef ENABLE_DEPRECATED_API
void deprecated_function(void);
#endif
