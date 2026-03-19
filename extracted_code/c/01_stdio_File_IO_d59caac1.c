/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\01_stdio_File_IO.md
 * Line: 325
 * Language: c
 * Block ID: d59caac1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误
FILE *fp = fopen("file.txt", "r");
fprintf(fp, "data");  // 如果文件不存在，fp为NULL，崩溃！

// 正确
FILE *fp = fopen("file.txt", "r");
if (!fp) {
    perror("fopen failed");
    return 1;
}
