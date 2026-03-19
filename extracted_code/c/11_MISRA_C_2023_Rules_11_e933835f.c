/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 313
 * Language: c
 * Block ID: e933835f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 有符号数位移 */
int flags = 0x80;
int bit = flags >> 31;  /* 实现定义的：算术移位 */

/* ❌ 违反 - 符号扩展问题 */
int8_t byte = 0x80;
int extended = byte << 8;  /* 先符号扩展为0xFFFFFF80，再移位 */

/* ❌ 违反 - 有符号数取反 */
int mask = ~0;  /* 实现定义的结果 */

/* ❌ 违反 - 混合符号 */
int a = -1;
unsigned int b = 1;
unsigned int r = a | b;  /* a先转换为unsigned，变为大正数 */
