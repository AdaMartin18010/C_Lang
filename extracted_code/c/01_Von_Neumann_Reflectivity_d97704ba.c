/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\01_Von_Neumann_Reflectivity.md
 * Line: 238
 * Language: c
 * Block ID: d97704ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * s-m-n定理的C语言类比：
 *
 * 原始函数：int f(int a, int b, int c)
 *
 * 部分应用（固化a, b）：
 * int g(int c) { return f(10, 20, c); }
 *
 * g就是s^2_1(f, 10, 20)
 */

// 函数指针实现部分应用
typedef int (*unary_func)(int);
typedef int (*ternary_func)(int, int, int);

// 闭包结构
typedef struct {
    ternary_func f;
    int a, b;
} PartialApplication;

int apply_partial(void *closure, int c) {
    PartialApplication *pa = closure;
    return pa->f(pa->a, pa->b, c);
}

// s-m-n构造器
unary_func s_2_1(ternary_func f, int a, int b) {
    PartialApplication *pa = malloc(sizeof(PartialApplication));
    pa->f = f;
    pa->a = a;
    pa->b = b;

    // 返回带有闭包的函数
    return (unary_func)apply_partial;
}

// 使用示例
int original_func(int x, int y, int z) {
    return x + y + z;
}

void test_smn(void) {
    // s^2_1(original_func, 10, 20)
    unary_func specialized = s_2_1(original_func, 10, 20);

    // specialized(z) = original_func(10, 20, z)
    printf("s(f,10,20)(5) = %d\n", specialized(5));  // 35
}
