/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 363
 * Language: c
 * Block ID: 5aebc52b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 从用户态获取整数参数
int argint(int n, int *ip);

// 从用户态获取指针参数
int argaddr(int n, uint64 *ip);

// 从用户态获取字符串
int argstr(int n, char *buf, int max);

// 例子: sys_trace使用argint(0, &mask)
// a0寄存器存储第一个参数
