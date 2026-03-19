/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 278
 * Language: c
 * Block ID: 2eaf94dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 字符串化运算符 # */
#define LOG(msg) printf(#msg "\n")
LOG(Hello);  /* 展开为 printf("Hello" "\n") */

/* ❌ 违反 - 连接运算符 ## */
#define CONCAT(a, b) a ## b
int xy = 10;
CONCAT(x, y);  /* 展开为 xy，即10 */

/* ❌ 违反 - 组合使用 */
#define MAKE_VAR(type, name) type var_##name
MAKE_VAR(int, count);  /* int var_count */
