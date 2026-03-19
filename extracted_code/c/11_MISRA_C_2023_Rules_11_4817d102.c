/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 206
 * Language: c
 * Block ID: 4817d102
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 过度复杂 */
result = (a + b) * (c - d) / ((e + f) * (g - h));

/* ❌ 违反 - 嵌套太深 */
x = a ? (b ? (c ? d : e) : f) : g;

/* ❌ 违反 - 函数调用太多 */
result = func1(func2(a), func3(func4(b)));

/* ❌ 违反 - 副作用混乱 */
arr[i++] = arr[i++] + func(j++);  /* 未定义行为！ */
