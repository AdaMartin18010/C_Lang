/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_New_Proposals_2026.md
 * Line: 118
 * Language: c
 * Block ID: 1892d5bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23及之前: 需要goto或额外标志
for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
        if (condition) {
            goto outer_done;  // 跳出外层循环
        }
    }
}
outer_done:

// C2y 提案 (N3658)
outer: for (int i = 0; i < 10; i++) {
    inner: for (int j = 0; j < 10; j++) {
        if (condition) {
            break outer;  // 跳出外层循环
        }
    }
}
