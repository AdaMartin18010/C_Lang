/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CISA_2026_Mandate_Depth.md
 * Line: 995
 * Language: c
 * Block ID: ab95f2c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 规则 17.7: 函数的返回值必须被使用 */
/* 非合规： */
getchar();  /* 返回值被忽略，可能导致EOF处理错误 */

/* 合规： */
int ch = getchar();
if (ch == EOF) {
    /* 处理错误 */
}

/* 规则 18.1: 指针运算结果必须在有效范围内 */
/* 非合规： */
int arr[10];
int *p = &arr[10];  /* 越界，有效索引是0-9 */

/* 合规： */
int arr[10];
for (int i = 0; i < 10; i++) {
    arr[i] = i;  /* 正确使用索引 */
}

/* 规则 21.3: 禁止使用堆内存分配 */
/* 非合规： */
int *p = malloc(sizeof(int) * 100);  /* 禁止 */

/* 合规： */
int arr[100];  /* 使用栈分配或静态分配 */
