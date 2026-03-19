/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 361
 * Language: c
 * Block ID: 4fef5e4d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用union */
union FloatInt {
    float f;
    int32_t i;
};

int32_t safe_type_pun(float f)
{
    union FloatInt fi = {.f = f};
    return fi.i;
}

/* ✅ 合规 - memcpy */
void safe_copy(void *dest, const void *src, size_t n)
{
    memcpy(dest, src, n);  /* 安全 */
}
