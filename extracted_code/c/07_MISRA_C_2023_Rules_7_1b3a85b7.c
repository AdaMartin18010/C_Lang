/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 291
 * Language: c
 * Block ID: 1b3a85b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 数组内指针运算 */
int arr[10];
int *p = &arr[0];
p++;  /* 指向arr[1] */
*p = 42;  /* arr[1] = 42 */

/* ✅ 合规 - 边界检查 */
size_t index = 5;
if (index < 10) {
    *(arr + index) = 42;  /* 安全 */
}

/* ✅ 合规 - 使用数组索引 */
arr[5] = 42;  /* 更清晰、更安全 */

/* ✅ 合规 - 遍历数组 */
int *end = arr + 10;  /* 指向末尾后一个（允许）*/
for (int *p = arr; p < end; p++) {
    *p = 0;
}
