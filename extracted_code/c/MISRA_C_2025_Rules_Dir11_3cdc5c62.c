/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 170
 * Language: c
 * Block ID: 3cdc5c62
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 指针转换限制 */

/* ❌ 违反 [待官方确认] - 对象指针类型混用 */
int arr[10];
float *fp = (float *)arr;  /* 危险！ */

/* ✅ 合规 [待官方确认] - char*可访问任何对象 */
void *data = get_data();
unsigned char *bytes = (unsigned char *)data;  /* 允许 */

/* ✅ 合规 [待官方确认] - 使用union类型双关 */
union FloatInt {
    float f;
    uint32_t i;
};
union FloatInt fi = {.f = 3.14f};
uint32_t bits = fi.i;  /* 安全 */

/* ✅ 合规 [待官方确认] - memcpy */
int src = 0x12345678;
unsigned char bytes[4];
memcpy(bytes, &src, sizeof(bytes));  /* 安全 */
