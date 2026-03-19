/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\README.md
 * Line: 80
 * Language: c
 * Block ID: 1563ca30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 参数验证 */
void process(const char *data, size_t len)
{
    if (data == NULL || len == 0) {
        return;  /* 提前返回 */
    }
    /* 处理 */
}

/* ✅ 默认情况处理 */
switch (status) {
    case OK:    /* ... */ break;
    case ERROR: /* ... */ break;
    default:
        /* 意外情况 */
        log_error("Unexpected status");
        break;
}
