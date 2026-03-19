/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\02_Undefined_Behavior.md
 * Line: 142
 * Language: c
 * Block ID: 356a461f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 同一变量多次修改
int i = 0;
int a = i++ + i++;  // UB!
int b = ++i + ++i;  // UB!

// ❌ 修改和读取交错
int c = i++ * i;    // UB!

// ✅ 分离到不同语句
int temp1 = i++;
int temp2 = i++;
int a = temp1 + temp2;
