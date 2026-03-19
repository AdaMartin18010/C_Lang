/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 166
 * Language: c
 * Block ID: 271d9c5a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 整数常量类型明确 */

/* ❌ 违反 [待官方确认] */
/* 隐式类型可能导致意外 */
long val = 2147483648;  /* 可能溢出32位int */

/* ✅ 合规 [待官方确认] */
long val = 2147483648L;  /* 明确long类型 */
uint32_t uval = 0xFFFFFFFFU;  /* 明确无符号 */

/* [C23相关] - _BitInt常量 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* _BitInt(17) bit_val = 123; */  /* [待确认] */
#endif
