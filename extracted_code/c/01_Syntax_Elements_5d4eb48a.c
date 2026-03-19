/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\01_Syntax_Elements.md
 * Line: 626
 * Language: c
 * Block ID: 5d4eb48a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ⚠️ 注意默认类型
double d = 3.14;    // double（默认）
float f = 3.14f;    // 显式float
long double ld = 3.14L;  // 显式long double

// ❌ 精度损失风险
float f2 = 3.141592653589793;  // 先转double，再转float，可能截断
