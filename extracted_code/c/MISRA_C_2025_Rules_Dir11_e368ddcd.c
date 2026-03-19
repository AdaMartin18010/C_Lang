/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 136
 * Language: c
 * Block ID: e368ddcd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 指针运算必须在数组边界内 */

int arr[10];
int *p = arr;

/* ✅ 合规 [待官方确认] */
for (int i = 0; i < 10; i++) {
    *(p + i) = i;  /* 在边界内 */
}

/* ✅ 合规 [待官方确认] - 允许指向末尾 */
int *end = arr + 10;  /* 指向末尾后一位，允许 */
if (p < end) {
    /* 安全 */
}

/* ❌ 违反 [待官方确认] */
int *bad = arr + 20;  /* 超出边界 */
*bad = 0;  /* 未定义行为 */

/* ❌ 违反 [待官方确认] - 不同对象间运算 */
int arr1[10], arr2[10];
ptrdiff_t diff = arr2 - arr1;  /* 未定义行为 */
