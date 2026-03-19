/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 524
 * Language: c
 * Block ID: f12bcf46
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 检查返回值 */
FILE *fp = fopen("file", "r");
if (fp == NULL) {
    if (errno == ENOENT) {
        printf("File not found\n");
    } else {
        printf("Open error: %d\n", errno);
    }
}

/* ✅ 合规 - 使用errno前清零 */
errno = 0;
long val = strtol(str, &endptr, 10);
if (errno == ERANGE) {
    /* 溢出错误 */
}

/* ✅ 合规 - 优先使用返回值 */
int result = some_function();
if (result != 0) {
    /* 使用result判断错误 */
}
