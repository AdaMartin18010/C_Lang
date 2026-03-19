/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\02_Type_System_Matrix.md
 * Line: 243
 * Language: c
 * Block ID: d20281ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 违反严格别名规则
float x = 1.0f;
int i = *(int*)&x;  // UB!

// ✅ 使用联合体 (C99合法)
union FloatInt {
    float f;
    int i;
};
union FloatInt fi = { .f = 1.0f };
int i = fi.i;  // OK

// ✅ 使用memcpy (最安全)
float x = 1.0f;
int i;
memcpy(&i, &x, sizeof(i));
