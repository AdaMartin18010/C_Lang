/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 58
 * Language: c
 * Block ID: 57f3091c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C89 标准特性 */

/* 函数原型（强制） */
int add(int a, int b);  /* 正确 */
int sub();              /* 旧式声明，不推荐 */

/* const 关键字 */
const int MAX_SIZE = 100;
const char* msg = "Hello";

/* volatile 关键字 */
volatile int timer_counter;  /* 可能被硬件修改 */

/* 标准头文件 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
