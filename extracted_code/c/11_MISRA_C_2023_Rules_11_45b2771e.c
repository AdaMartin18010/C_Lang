/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 139
 * Language: c
 * Block ID: 45b2771e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 指针转整数 */
int *p = &value;
uintptr_t addr = (int)p;  /* 错误：应该是(intptr_t)p */

/* ❌ 违反 - 不同对象指针转换 */
int *ip = &ival;
float *fp = (float *)ip;  /* 危险！类型混淆 */

/* ❌ 违反 - 指针转void*后丢失信息 */
void *vp = p;
int *ip2 = vp;  /* 隐式转换，警告 */

/* ❌ 违反 - 结构体指针随意转换 */
struct Header *h = (struct Header *)buffer;  /* 不安全 */
