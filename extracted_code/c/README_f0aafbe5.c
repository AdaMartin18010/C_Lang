/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\README.md
 * Line: 146
 * Language: c
 * Block ID: f0aafbe5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 不安全 */
if (a == b) { }

/* ✅ 使用ULP比较 */
bool almost_equal(float a, float b, int max_ulp) {
    union { float f; int32_t i; } ua = {a}, ub = {b};
    if (ua.i < 0) ua.i = 0x80000000 - ua.i;
    if (ub.i < 0) ub.i = 0x80000000 - ub.i;
    return abs(ua.i - ub.i) <= max_ulp;
}
