/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 335
 * Language: c
 * Block ID: ae6fa365
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 建议避免 */
do {
    process();
} while (condition);

/* ✅ 替代 - for循环（已知次数）*/
for (int i = 0; i < n; i++) {
    process();
}

/* ✅ 替代 - while循环（先检查）*/
while (condition) {
    process();
}

/* ✅ 如果必须使用do-while */
/* 使用场景：至少执行一次，且条件依赖执行结果 */
do {
    status = try_operation();
    if (status != OK) {
        retry_delay();
    }
} while (status != OK && retries++ < MAX_RETRIES);
