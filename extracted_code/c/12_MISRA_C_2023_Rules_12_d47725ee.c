/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 291
 * Language: c
 * Block ID: d47725ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 单一出口 */
bool done = false;
while (!done) {
    if (error) {
        done = true;  /* 设置标志而非break */
    } else {
        process();
        done = is_complete();
    }
}

/* ✅ 合规 - 清晰的退出条件 */
while (has_more_data() && !timeout_occurred()) {
    process_next();
}

/* ✅ 合规 - do-while替代（如果适用）*/
do {
    process();
} while (should_continue());

/* ✅ 合规 - 无限循环（明确意图）*/
for (;;) {  /* 或 while (1) */
    process_messages();
}
