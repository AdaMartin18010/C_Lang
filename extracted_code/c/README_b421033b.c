/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\README.md
 * Line: 499
 * Language: c
 * Block ID: b421033b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 低效：逐字符读取 */
void inefficient_read(FILE *fp) {
    int c;
    while ((c = fgetc(fp)) != EOF) {  /* 每次可能触发系统调用 */
        process(c);
    }
}

/* 高效：批量读取后处理 */
void efficient_read(FILE *fp) {
    char buffer[4096];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        for (size_t i = 0; i < n; i++) {
            process(buffer[i]);
        }
    }
}
