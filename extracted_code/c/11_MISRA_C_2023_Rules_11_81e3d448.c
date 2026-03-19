/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 278
 * Language: c
 * Block ID: 81e3d448
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 分离副作用 */
int temp1 = i++;
int temp2 = i++;
int x = temp1 + temp2;

/* ✅ 合规 - 明确调用顺序 */
int f1 = func1();
int f2 = func2();
int r = f1 + f2;

/* ✅ 合规 - 先完成所有副作用 */
int a_val = a++;
int b_val = b++;
int sum = a_val + b_val;
result = calculate(a_val, b_val, sum);

/* ✅ 合规 - 明确索引 */
int idx = i;
arr[idx] = i;
i++;  /* 单独增量 */
