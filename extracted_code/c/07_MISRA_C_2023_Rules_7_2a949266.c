/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 326
 * Language: c
 * Block ID: 2a949266
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 未检查的用户输入 */
void process(int index)
{
    int arr[10];
    arr[index] = 42;  /* 可能越界 */
}

/* ❌ 违反 - 字符串操作越界 */
char buf[10];
strcpy(buf, "Hello, World!");  /* 缓冲区溢出 */

/* ❌ 违反 - 格式化字符串 */
char buf[10];
int x = 123456789;
sprintf(buf, "%d", x);  /* 可能溢出 */

/* ❌ 违反 - memcpy越界 */
char dest[5];
char src[10] = "123456789";
memcpy(dest, src, 10);  /* 越界写入 */
