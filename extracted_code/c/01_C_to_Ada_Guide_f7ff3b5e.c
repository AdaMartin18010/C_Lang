/*
 * Auto-generated from: 17_Ada_SPARK\06_Migration_from_C\01_C_to_Ada_Guide.md
 * Line: 728
 * Language: c
 * Block ID: f7ff3b5e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

FILE* fp = fopen("data.txt", "r");
if (fp == NULL) {
    fprintf(stderr, "Cannot open file\n");
    return -1;
}
// 使用文件...
fclose(fp);
