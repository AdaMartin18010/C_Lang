/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 61
 * Language: c
 * Block ID: 4de0b3ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 不同级别的资源管理 */
void low_level_open(void)
{
    fp = fopen("file", "r");  /* 底层打开 */
}

void high_level_process(void)
{
    low_level_open();
    /* 处理 */
    fclose(fp);  /* 高层关闭 - 不匹配！ */
}

/* ❌ 违反 - 分散的资源管理 */
void init_system(void)
{
    mutex1 = create_mutex();
    mutex2 = create_mutex();
    /* ... 其他初始化 */
}

void cleanup_partial(void)
{
    destroy_mutex(mutex1);  /* 只释放部分资源 */
}
