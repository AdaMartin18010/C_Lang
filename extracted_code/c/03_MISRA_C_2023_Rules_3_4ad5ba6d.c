/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 256
 * Language: c
 * Block ID: 4ad5ba6d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用条件编译代替注释代码 */
#ifdef COMMENTED_OUT_CODE
    /* This code is temporarily disabled */
    if (x > 0) {
        return x;  /* return positive */
    }
#endif

/* ✅ 合规 - 使用版本控制 */
/* Old implementation removed, see git history */
/* Git commit: abc123 for previous version */

/* ✅ 合规 - 文档化删除 */
/* REMOVED: Old function process_data()
 * Reason: Replaced with process_data_v2()
 * Date: 2024-01-15
 * Author: John Doe
 */

/* ✅ 合规 - 使用 #if 0 */
#if 0  /* Disabled code block */
    /* This entire block is safely disabled */
    int old_function(void) {
        /* Can safely use /* */ inside */
        return 0;
    }
#endif
