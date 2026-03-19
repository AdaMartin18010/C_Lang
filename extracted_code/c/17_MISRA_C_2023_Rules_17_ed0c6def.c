/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\17_MISRA_C_2023_Rules_17.md
 * Line: 183
 * Language: c
 * Block ID: ed0c6def
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 带类型标签的联合 */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} ValueType;

typedef struct {
    ValueType type;
    union {
        int i;
        float f;
        const char *s;
    } data;
} TypedValue;

void set_int(TypedValue *v, int value)
{
    v->type = TYPE_INT;
    v->data.i = value;
}

void set_float(TypedValue *v, float value)
{
    v->type = TYPE_FLOAT;
    v->data.f = value;
}

int get_int(const TypedValue *v)
{
    if (v->type != TYPE_INT) {
        /* 错误处理 */
        return 0;
    }
    return v->data.i;
}

/* ✅ 合规 - 类型双关（读取当前写入的类型）*/
union FloatInt {
    float f;
    uint32_t i;
};

uint32_t float_to_bits(float f)
{
    union FloatInt fi;
    fi.f = f;  /* 写入f */
    return fi.i;  /* 读取i - 允许 */
}

float bits_to_float(uint32_t i)
{
    union FloatInt fi;
    fi.i = i;  /* 写入i */
    return fi.f;  /* 读取f - 允许 */
}
