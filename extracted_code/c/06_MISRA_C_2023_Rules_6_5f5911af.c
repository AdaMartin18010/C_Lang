/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 614
 * Language: c
 * Block ID: 5f5911af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 向前goto（资源清理）*/
void process(void)
{
    FILE *fp = fopen("file", "r");
    if (fp == NULL) {
        goto cleanup;  /* 向前跳转 */
    }

    /* 处理文件 */

cleanup:
    if (fp != NULL) {
        fclose(fp);
    }
    return;
}

/* ✅ 合规 - 使用循环代替向后goto */
while (condition) {
    if (retry_condition) {
        continue;  /* 向前到下一次迭代 */
    }
    /* 处理 */
}

/* ✅ 合规 - 结构化错误处理 */
enum status do_work(void)
{
    enum status s = STATUS_OK;

    if (!init()) {
        s = STATUS_INIT_FAIL;
    } else if (!process()) {
        s = STATUS_PROC_FAIL;
        cleanup_init();
    }

    return s;
}
