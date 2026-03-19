/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 710
 * Language: c
 * Block ID: 8bae33af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 避免危险的隐式转换 */

/* ❌ 违反 [待官方确认] - 有符号/无符号混合 */
int a = -1;
unsigned int b = 1;
if (a < b) {  /* false！a被转换为无符号大数 */
    /* ... */
}

/* ❌ 违反 [待官方确认] - 浮点转整数 */
float f = 3.7f;
int i = f;  /* 隐式截断 */

/* ✅ 合规 [待官方确认] */
/* 统一类型 */
int a = -1;
int b = 1;
if (a < b) {  /* true */
    /* ... */
}

/* ✅ 合规 [待官方确认] - 显式转换 */
float f = 3.7f;
int i = (int)f;  /* 明确转换 */
