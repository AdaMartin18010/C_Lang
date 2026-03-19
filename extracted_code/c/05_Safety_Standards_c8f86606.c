/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\05_Safety_Standards.md
 * Line: 264
 * Language: c
 * Block ID: c8f86606
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 违反规则：依赖求值顺序 (Rule 13.2)
int x = func1() + func2();  // 调用顺序未指定

// ❌ 违反规则：副作用 (Rule 13.3, 13.4, 13.5)
x = v[i++] + v[i++];  // 两次修改i，未定义行为
x = i++ + i++;        // 未定义行为
if (x++ && x++)       // 未定义行为

// ✅ 合规代码
int a = func1();
int b = func2();
int x = a + b;

int temp = v[i];
i++;
x = temp + v[i];
i++;
