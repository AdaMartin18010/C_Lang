/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 214
 * Language: c
 * Block ID: 8e53bc20
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 未定义行为：同一变量在同一表达式中既读又写，且不是顺序点
int i = 0;
int a = i++ + i++;  // UB!
int b = ++i + ++i;  // UB!
int c = i++ * 2 + i++;  // UB!

// ❌ 函数参数求值顺序未指定
int result = f(i++, i++);  // 哪个先求值？未定义！

// ❌ 数组下标求值顺序未指定
arr[i++] = arr[i++];  // UB!

// ✅ 安全写法：使用顺序点（语句结束是顺序点）
int temp1 = i++;
int temp2 = i++;
int a = temp1 + temp2;
