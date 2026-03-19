/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 222
 * Language: c
 * Block ID: d536c186
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 分步计算 */
int sum1 = a + b;
int diff1 = c - d;
int product = sum1 * diff1;
int sum2 = e + f;
int diff2 = g - h;
result = product / (sum2 * diff2);

/* ✅ 合规 - 使用临时变量 */
float temp1 = calculate_temp1(a, b);
float temp2 = calculate_temp2(c, d);
result = combine(temp1, temp2);

/* ✅ 合规 - 简化条件 */
if (a) {
    x = b ? c : d;
} else {
    x = e;
}

/* ✅ 合规 - 分离副作用 */
int idx1 = i++;
int idx2 = i++;
int arg = j++;
arr[idx1] = arr[idx2] + func(arg);
