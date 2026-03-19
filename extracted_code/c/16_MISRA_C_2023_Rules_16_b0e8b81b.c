/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\16_MISRA_C_2023_Rules_16.md
 * Line: 81
 * Language: c
 * Block ID: b0e8b81b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 类型不匹配 */
void process(int value);
float f = 3.14f;
process(f);  /* 隐式float转int */

/* ❌ 违反 - 指针类型不匹配 */
void use_buffer(char *buf);
int data[10];
use_buffer((char *)data);  /* 不安全 */

/* ❌ 违反 - 符号性不匹配 */
void set_size(unsigned int size);
int len = -1;
set_size(len);  /* 负值变正值 */
