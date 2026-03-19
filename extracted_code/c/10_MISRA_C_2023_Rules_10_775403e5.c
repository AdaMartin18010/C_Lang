/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 449
 * Language: c
 * Block ID: 775403e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - C89风格（MISRA兼容）*/
void process_data(const char *input)
{
    /* 1. 所有声明在块开始 */
    int result = 0;
    size_t len = 0;
    char *buffer = NULL;

    /* 2. 然后是语句 */
    if (input == NULL) {
        return;
    }

    len = strlen(input);
    buffer = malloc(len + 1);
    if (buffer == NULL) {
        return;
    }

    /* 处理 */

    free(buffer);
}

/* ✅ 合规 - C99+风格（允许混合，但建议集中）*/
void modern_style(const char *input)
{
    /* 主要声明在开头 */
    int result = 0;

    if (input == NULL) {
        return;
    }

    /* for循环变量在循环中 */
    for (size_t i = 0; i < strlen(input); i++) {
        /* ... */
    }
}
