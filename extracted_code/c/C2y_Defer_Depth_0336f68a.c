/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 564
 * Language: c
 * Block ID: 0336f68a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void loop_outside_defer(void) {
    FILE *fp = NULL;

    for (int i = 0; i < 3; i++) {
        fp = fopen("data.txt", "r");
        // 处理...
        // 注意：这里没有defer，需要手动关闭
    }

    defer fclose(fp);  // 只执行一次，只关闭最后一次打开的文件！
    // 前面的文件句柄泄漏了！
}
