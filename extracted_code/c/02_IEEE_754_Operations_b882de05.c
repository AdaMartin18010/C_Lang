/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\02_IEEE_754_Operations.md
 * Line: 432
 * Language: c
 * Block ID: b882de05
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反严格别名 */
float f = 3.14f;
int32_t bits = *(int32_t *)&f;  /* 未定义行为！ */

/* ✅ 合规 - 使用union */
union float_bits {
    float f;
    int32_t i;
};

int32_t get_float_bits(float f)
{
    union float_bits fb = { .f = f };
    return fb.i;
}

/* ✅ C23 - 使用stdbit.h（如果可用）*/
#include <stdbit.h>  /* C23 */
int32_t bits = stdbitcopyf(f);
