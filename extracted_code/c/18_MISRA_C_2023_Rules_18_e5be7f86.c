/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 115
 * Language: c
 * Block ID: e5be7f86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 同一数组内的指针减法 */
int arr[100];
int *p1 = &arr[10];
int *p2 = &arr[50];
ptrdiff_t diff = p2 - p1;  /* 40 */

/* ✅ 合规 - 计算数组元素个数 */
size_t count_elements(const int *start, const int *end)
{
    return (size_t)(end - start);  /* 同一数组 */
}

/* ✅ 合规 - 字符串长度计算 */
size_t my_strlen(const char *str)
{
    const char *end = str;
    while (*end != '\0') {
        end++;
    }
    return (size_t)(end - str);  /* 同一字符串 */
}
