/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\20_MISRA_C_2023_Rules_20.md
 * Line: 70
 * Language: c
 * Block ID: d2941803
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 检查返回值 */
void *ptr = malloc(size);
if (ptr == NULL) {
    /* 错误处理 */
    return ERROR_NO_MEMORY;
}

/* ✅ 零长度分配 */
if (size == 0) {
    /* 处理或返回错误 */
    return;
}
void *ptr = malloc(size);

/* ✅ 避免内存泄漏 */
void process(void)
{
    char *buffer = malloc(BUF_SIZE);
    if (buffer == NULL) {
        return;
    }

    /* 使用buffer */

    free(buffer);  /* 总是释放 */
}

/* ✅ 配对使用 */
void *create_object(void)
{
    return malloc(sizeof(Object));
}

void destroy_object(void *obj)
{
    free(obj);
}
