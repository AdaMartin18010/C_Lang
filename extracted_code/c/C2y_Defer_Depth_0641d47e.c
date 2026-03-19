/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 407
 * Language: c
 * Block ID: 0641d47e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void goto_order_example(int flag) {
    if (flag) {
        goto skip;  // 不执行下面的defer
    }

    FILE *fp = fopen("test.txt", "r");
    defer fclose(fp);

    // 使用fp...

skip:
    // fp不会在这里关闭（如果flag为真）
}
