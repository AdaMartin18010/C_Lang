/*
 * Auto-generated from: 17_Ada_SPARK\06_Migration_from_C\01_C_to_Ada_Guide.md
 * Line: 483
 * Language: c
 * Block ID: f9e84393
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define DEFINE_STACK(TYPE, NAME, SIZE) \
    typedef struct { \
        TYPE data[SIZE]; \
        int top; \
    } NAME##_Stack; \
    void NAME##_push(NAME##_Stack* s, TYPE val) { \
        s->data[s->top++] = val; \
    }

DEFINE_STACK(int, Int, 100)
DEFINE_STACK(float, Float, 100)
