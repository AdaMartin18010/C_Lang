/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 602
 * Language: c
 * Block ID: 7d15fc3c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 未指定行为：函数参数求值顺序
int result = f(i++, i++);  // 不确定哪个i++先执行

// ❌ 未指定行为：操作数求值顺序
int x = expr1() + expr2();  // 不确定哪个函数先调用

// ❌ 未定义行为：修改+访问无序列点
int a = i++ + ++i;

// ✅ 安全写法：明确顺序
int temp1 = i++;
int temp2 = i++;
int result = f(temp1, temp2);

// ✅ 安全写法：使用独立语句
int val1 = expr1();
int val2 = expr2();
int x = val1 + val2;
