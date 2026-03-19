/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 57
 * Language: c
 * Block ID: 7ce331a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 边界检查 */
#define ARRAY_SIZE 10
int arr[ARRAY_SIZE];

void safe_access(size_t index)
{
    if (index >= ARRAY_SIZE) {
        /* 错误处理 */
        return;
    }
    arr[index] = 42;
}

/* ✅ 合规 - 使用安全函数 */
void safe_copy(char *dest, size_t dest_size, const char *src)
{
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

/* ✅ 合规 - 循环边界正确 */
for (int i = 0; i < ARRAY_SIZE; i++) {  /* < 不是 <= */
    arr[i] = 0;
}

/* ✅ 合规 - 使用sizeof计算大小 */
int arr[10];
size_t count = sizeof(arr) / sizeof(arr[0]);
for (size_t i = 0; i < count; i++) {
    arr[i] = 0;
}
