/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2284
 * Language: c
 * Block ID: f7e882c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 单语句
FILE *fp = fopen("test.txt", "r");
defer fclose(fp);

// 块形式
defer {
    fclose(fp);
    free(buffer);
}
