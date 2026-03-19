/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\README.md
 * Line: 281
 * Language: c
 * Block ID: 83b1c3ef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 常见错误：char 类型存储 EOF */
void wrong_eof_check() {
    FILE *fp = fopen("data.txt", "r");
    char c;  /* 错误：char 可能无符号 */

    while ((c = fgetc(fp)) != EOF) {  /* 类型转换问题 */
        /* 如果 char 无符号，EOF (-1) 会变成 255，永远无法匹配 */
        putchar(c);
    }
    fclose(fp);
}

/* 正确做法：使用 int */
void correct_eof_check() {
    FILE *fp = fopen("data.txt", "r");
    int c;  /* 正确：int 可以存储所有 char 值和 EOF */

    while ((c = fgetc(fp)) != EOF) {
        putchar(c);
    }

    /* 区分 EOF 和错误 */
    if (ferror(fp)) {
        perror("读取错误");
    }

    fclose(fp);
}
