/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 306
 * Language: c
 * Block ID: 3975f580
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 表达式值不得依赖子表达式求值顺序 */

/* ❌ 严重违反 [待官方确认] - 未定义行为 */
int i = 0;
int x = i++ + i++;  /* 未定义行为！ */

/* ❌ 违反 [待官方确认] */
int a = 5;
int b = (a = 3) + a;  /* a的值不确定是3还是5 */

/* ❌ 违反 [待官方确认] */
int arr[10] = {0};
int idx = 0;
int val = arr[idx++] + arr[idx++];  /* 未定义！ */

/* ✅ 合规 [待官方确认] - 分离表达式 */
int i = 0;
int temp1 = i++;
int temp2 = i++;
int x = temp1 + temp2;  /* x = 0 + 1 = 1，明确 */

/* ✅ 合规 [待官方确认] */
int a = 5;
a = 3;
int b = a + a;  /* b = 6，明确 */
