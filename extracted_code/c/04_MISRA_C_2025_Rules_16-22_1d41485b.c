/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 40
 * Language: c
 * Block ID: 1d41485b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 */
FILE* fp = fopen("data.txt", "r");
fread(buffer, 1, size, fp);  /* 未检查fopen和fread返回值 */
fclose(fp);

/* 符合规范 */
FILE* fp = fopen("data.txt", "r");
if (fp != NULL) {
    size_t n = fread(buffer, 1, size, fp);
    if (n == size) {
        /* 处理数据 */
    }
    fclose(fp);
} else {
    /* 错误处理 */
}
