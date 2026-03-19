/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 303
 * Language: c
 * Block ID: 514c2611
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用预处理 */
#define DEBUG 0

#if DEBUG
    printf("Debug\n");
#endif

/* ✅ 合规 - 使用条件编译 */
#ifdef DEBUG
    debug_log("message");
#endif

/* ✅ 合规 - 变量条件 */
bool is_debug_enabled = get_debug_flag();
if (is_debug_enabled) {
    printf("Debug\n");
}
