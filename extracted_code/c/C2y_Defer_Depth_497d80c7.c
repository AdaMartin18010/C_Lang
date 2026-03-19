/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 542
 * Language: c
 * Block ID: 497d80c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void loop_example(void) {
    for (int i = 0; i < 3; i++) {
        FILE *fp = fopen("data.txt", "r");
        defer fclose(fp);  // 每次迭代结束时执行

        printf("Iteration %d\n", i);
        // 处理文件...
    }  // 每次迭代结束，defer执行
}

// 输出：
// Iteration 0
// [fclose]
// Iteration 1
// [fclose]
// Iteration 2
// [fclose]
