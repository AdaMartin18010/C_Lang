/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\20_MISRA_C_2023_Rules_20.md
 * Line: 156
 * Language: c
 * Block ID: 76fd7514
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 使用线程安全版本 */
#include <string.h>

/* strtok_r是线程安全的，strtok不是 */
char *saveptr;
char *token = strtok_r(str, delim, &saveptr);

/* ✅ 使用本地时间函数 */
#include <time.h>

/* localtime_r是线程安全的 */
struct tm result;
localtime_r(&time_val, &result);

/* ✅ 使用可重入函数 */
#include <stdlib.h>

/* rand_r是线程安全的，rand不是 */
unsigned int seed = time(NULL);
int value = rand_r(&seed);
