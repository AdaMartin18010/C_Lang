/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 1106
 * Language: c
 * Block ID: bbbd7c13
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 匿名结构体和联合体 (C11 标准)
#include <stdio.h>
#include <stdint.h>

// 匿名结构体成员
struct WithAnonymousStruct {
    int type;
    union {
        struct {
            int x;
            int y;
        };  // 匿名结构体 - 成员直接访问
        struct {
            float r;
            float theta;
        } polar;  // 命名结构体 - 通过 polar.r 访问
    };
};

void demo_anonymous() {
    struct WithAnonymousStruct p = {
        .type = 0,
        .x = 10,    // 直接访问匿名结构体成员
        .y = 20,
    };

    printf("x = %d, y = %d\n", p.x, p.y);  // 直接访问

    // 切换到极坐标视图
    p.polar.r = 22.36f;
    p.polar.theta = 1.107f;
}

// 匿名联合体 - 常用于类型双关
struct Variant {
    uint32_t type;
    union {
        int i;
        float f;
        void *p;
    };  // 匿名联合体
};

void demo_variant() {
    struct Variant v;
    v.type = 1;  // 假设 1 表示 float
    v.f = 3.14159f;  // 直接访问匿名联合体成员

    // 读取整数值 (类型双关)
    printf("float %f has int representation 0x%x\n", v.f, v.i);
}

// 嵌套匿名结构体
struct ComplexNested {
    int header;
    struct {
        struct {
            int deep_field;
        };
        int sibling;
    };
    int footer;
};

void demo_nested() {
    struct ComplexNested n = {
        .header = 1,
        .deep_field = 42,  // 直接访问深层字段
        .sibling = 100,
        .footer = 2,
    };
    printf("deep_field = %d\n", n.deep_field);
}
