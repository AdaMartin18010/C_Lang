/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 78
 * Language: c
 * Block ID: 3a752b98
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y提案语法 (N3742)
// 使用冒号后接标识符为循环命名

outer: for (int i = 0; i < N; i++) {
    middle: for (int j = 0; j < M; j++) {
        inner: for (int k = 0; k < P; k++) {
            if (some_condition(i, j, k)) {
                break outer;      // 跳出最外层循环
            }
            if (other_condition(k)) {
                continue middle;  // 跳到middle循环的下一次迭代
            }
        }
    }
}
