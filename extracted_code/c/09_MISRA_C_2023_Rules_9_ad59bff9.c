/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 163
 * Language: c
 * Block ID: ad59bff9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 所有权明确 */

/* 模块A - 分配 */
void *module_a_alloc(size_t size)
{
    return malloc(size);
}

/* 模块A - 释放 */
void module_a_free(void *ptr)
{
    free(ptr);
}

/* 模块B - 使用但不释放 */
void module_b_use(void)
{
    void *data = module_a_alloc(100);
    /* 使用data */
    module_a_free(data);  /* 或者转移所有权 */
}

/* ✅ 合规 - 所有权转移文档化 */
/**
 * @brief Create a connection object
 * @return Ownership transferred to caller
 */
connection_t *connection_create(void);

/**
 * @brief Destroy a connection object
 * @param conn Ownership transferred from caller
 */
void connection_destroy(connection_t *conn);
