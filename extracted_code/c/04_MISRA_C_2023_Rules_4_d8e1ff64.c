/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 233
 * Language: c
 * Block ID: d8e1ff64
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 严重违反 - 重定义关键字 */
#define if(x) if((x) != 0)  /* 改变了if的行为！ */
#define while(x) while((x))  /* 危险！ */

/* ❌ 违反 - 宏名与标准库标识符冲突 */
#define printf my_printf  /* 替换标准库函数 */
#define NULL 0  /* 可能改变NULL的定义 */

/* ❌ 违反 - 宏名与类型名冲突 */
typedef int int32_t;
#define int32_t long  /* 与typedef冲突！ */
