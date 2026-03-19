/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 409
 * Language: c
 * Block ID: d8060f86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 安全字符串输入 */
char buf[100];
if (fgets(buf, sizeof(buf), stdin) != NULL) {
    /* 移除可能的换行符 */
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') {
        buf[len-1] = '\0';
    }
}

/* ✅ 合规 - 安全字符串复制 */
char dest[10];
strncpy(dest, "source", sizeof(dest) - 1);
dest[sizeof(dest) - 1] = '\0';

/* ✅ 合规 - 安全格式化 */
char buf[10];
snprintf(buf, sizeof(buf), "%d", x);  /* 自动截断 */

/* ✅ 合规 - 安全输入解析 */
char line[256];
if (fgets(line, sizeof(line), stdin)) {
    int value;
    if (sscanf(line, "%d", &value) == 1) {
        /* 使用value */
    }
}
