/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 412
 * Language: c
 * Block ID: ad214344
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 正常循环控制 */
for (int i = 0; i < n; i++) {
    if (skip_condition(i)) {
        continue;  /* 跳到下一次迭代 */
    }

    if (done_condition(i)) {
        break;  /* 退出循环 */
    }

    process(i);
}

/* ✅ 合规 - 标签化break（MISRA允许）*/
search:
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        if (found(i, j)) {
            break search;  /* 跳出外层循环 */
        }
    }
}
