/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 202
 * Language: c
 * Block ID: f65a6176
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 对象指针混用 */
int arr[10];
float *f = (float *)arr;  /* 危险！ */

/* ❌ 违反 - 结构体指针混用 */
struct Header h;
struct Data *d = (struct Data *)&h;  /* 不安全 */

/* ❌ 违反 - 通过void*间接转换 */
int *ip = &x;
void *vp = ip;
float *fp = vp;  /* 未定义行为 */
