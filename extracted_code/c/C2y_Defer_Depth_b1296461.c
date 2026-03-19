/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 583
 * Language: c
 * Block ID: b1296461
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void conditional_defer(int flag) {
    FILE *fp = fopen("data.txt", "r");
    if (!fp) return;

    if (flag) {
        defer fclose(fp);  // 只在flag为真时注册
    } else {
        // 需要手动关闭，或在这里也注册defer
        fclose(fp);
    }

    // 业务逻辑...
}
