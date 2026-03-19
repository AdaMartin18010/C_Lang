/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\13_MISRA_C_2023_Rules_13.md
 * Line: 215
 * Language: c
 * Block ID: 8fecda04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 检查返回值 */
void *mem = malloc(100);
if (mem == NULL) {
    /* 错误处理 */
}

/* ✅ 合规 - 显式忽略（有文档说明）*/
(void)printf("Debug: %d\n", value);  /* 显式转换void */

/* ✅ 合规 - 错误处理 */
if (fclose(fp) != 0) {
    log_error("Failed to close file");
}

/* ✅ 合规 - 包装函数 */
void safe_remove(const char *path)
{
    if (remove(path) != 0) {
        /* 记录但不一定终止 */
        log_warning("Failed to remove %s", path);
    }
}
