/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\13_MISRA_C_2023_Rules_13.md
 * Line: 138
 * Language: c
 * Block ID: 5f2331f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 防御性编程模式 */

/* 1. 早期返回模式 */
int process_data(const Data *input, Result *output)
{
    if (input == NULL || output == NULL) {
        return ERROR_INVALID_PARAM;
    }

    /* 处理 */
    return SUCCESS;
}

/* 2. 断言模式（调试）*/
int process_data_assert(const Data *input, Result *output)
{
    assert(input != NULL);
    assert(output != NULL);

    /* 处理 */
    return SUCCESS;
}

/* 3. 范围验证 */
int set_timeout(int milliseconds)
{
    if (milliseconds < MIN_TIMEOUT || milliseconds > MAX_TIMEOUT) {
        return ERROR_OUT_OF_RANGE;
    }

    g_timeout = milliseconds;
    return SUCCESS;
}

/* 4. 数组长度验证 */
int copy_array(const int *src, size_t src_len,
               int *dst, size_t dst_len)
{
    if (src == NULL || dst == NULL) {
        return ERROR_NULL_POINTER;
    }

    if (src_len > dst_len) {
        return ERROR_BUFFER_TOO_SMALL;
    }

    memcpy(dst, src, src_len * sizeof(int));
    return SUCCESS;
}
