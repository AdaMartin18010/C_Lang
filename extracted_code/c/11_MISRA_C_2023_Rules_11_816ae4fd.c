/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 158
 * Language: c
 * Block ID: 816ae4fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用uintptr_t */
#include <stdint.h>
int *p = &value;
uintptr_t addr = (uintptr_t)p;  /* 正确 */

/* ✅ 合规 - 使用union（类型双关）*/
union Converter {
    float f;
    int32_t i;
};

union Converter c;
c.f = 3.14f;
int32_t bits = c.i;  /* 安全 */

/* ✅ 合规 - memcpy（严格别名安全）*/
float f = 3.14f;
int32_t i;
memcpy(&i, &f, sizeof(i));  /* 安全，无类型混淆 */

/* ✅ 合规 - 字符指针访问字节 */
void *data = get_data();
unsigned char *bytes = (unsigned char *)data;  /* char*允许访问任何对象 */
/* 可以安全访问bytes[0], bytes[1]等 */
