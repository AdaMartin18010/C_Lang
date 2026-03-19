/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 240
 * Language: c
 * Block ID: c7676805
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 指针转整数（地址操作）*/
uintptr_t addr = (uintptr_t)&data;  /* 安全 */

/* ✅ 合规 - 指针转void* */
void *generic_ptr = (void *)&data;

/* ✅ 合规 - 指针类型转换（相关类型）*/
int arr[10];
int *p = arr;
const int *cp = (const int *)p;  /* 添加const是安全的 */

/* ✅ 合规 - 函数指针（相同签名）*/
int (*func_ptr)(int) = func;  /* 类型匹配 */
