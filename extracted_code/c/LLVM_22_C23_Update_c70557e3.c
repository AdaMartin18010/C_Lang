/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 154
 * Language: c
 * Block ID: c70557e3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用 continue 跳转到特定循环的下一轮迭代
void skip_processing_pattern(int n, int m) {
    outer: for (int i = 0; i < n; i++) {
        middle: for (int j = 0; j < m; j++) {
            if (i == j) {
                continue outer;  // 跳过当前外层循环迭代
            }

            if (is_special_case(i, j)) {
                continue middle;  // 跳过当前中层循环迭代
            }

            process(i, j);
        }
    }
}
