/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 219
 * Language: c
 * Block ID: 95f597f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用union（类型双关）*/
union FloatInt {
    float f;
    uint32_t i;
};

uint32_t get_float_bits(float f)
{
    union FloatInt fi = {.f = f};
    return fi.i;
}

/* ✅ 合规 - memcpy */
int src = 0x12345678;
unsigned char bytes[4];
memcpy(bytes, &src, sizeof(bytes));  /* 安全 */

/* ✅ 合规 - char*访问字节 */
int value = 42;
unsigned char *bytes = (unsigned char *)&value;  /* char*允许 */
for (size_t i = 0; i < sizeof(value); i++) {
    printf("%02x ", bytes[i]);
}

/* ✅ 合规 - 不透明指针 */
typedef struct InternalData *Handle;
Handle create(void);
void use(Handle h);
void destroy(Handle h);
