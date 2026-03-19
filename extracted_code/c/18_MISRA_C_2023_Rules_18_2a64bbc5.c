/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 262
 * Language: c
 * Block ID: 2a64bbc5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 显式大小 */
int arr1[10];
char str[256];
uint8_t buffer[1024];

/* ✅ 合规 - 从初始化推导大小 */
int arr2[] = {1, 2, 3, 4, 5};  /* 大小为5 */
char msg[] = "hello";  /* 大小为6（含'\0'）*/

/* ✅ 合规 - 使用宏 */
#define MAX_ITEMS 100
struct Item items[MAX_ITEMS];

/* ✅ 合规 - 枚举作为大小 */
enum { NUM_SENSORS = 8 };
float sensor_data[NUM_SENSORS];

/* ✅ 合规 - 外部数组声明 */
extern int global_array[];  /* 外部定义确定大小 */
