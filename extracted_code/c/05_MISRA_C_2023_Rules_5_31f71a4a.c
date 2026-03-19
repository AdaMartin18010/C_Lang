/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 97
 * Language: c
 * Block ID: 31f71a4a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 使用char存储小整数 */
char small_value = 200;  /* 如果char是有符号的，这是负数！ */

/* ❌ 违反 - 使用char进行算术运算 */
char count = 0;
for (char i = 0; i < 100; i++) {  /* 如果char是signed，可能溢出 */
    count++;
}

/* ❌ 违反 - 使用char存储布尔值 */
char is_ready = 1;  /* 应该使用bool */

/* ❌ 违反 - 使用char存储8位数据 */
void process_byte(char byte);  /* 应该明确uint8_t或int8_t */
