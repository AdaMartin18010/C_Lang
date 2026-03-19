/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\02_Undefined_Behavior.md
 * Line: 122
 * Language: c
 * Block ID: 81893e17
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 通过不同类型指针访问
float f = 3.14f;
int *ip = (int *)&f;
int bits = *ip;  // UB! 违反严格别名

// ✅ 使用union（C99有效类型规则）
union FloatInt {
    float f;
    int i;
};
union FloatInt fi = {.f = 3.14f};
int bits = fi.i;  // OK

// ✅ 使用memcpy
memcpy(&bits, &f, sizeof(bits));  // OK
