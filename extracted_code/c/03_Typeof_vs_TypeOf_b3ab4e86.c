/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 200
 * Language: c
 * Block ID: b3ab4e86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 typeof_unqual 详解

void typeof_unqual_usage(void) {
    const int cx = 42;
    volatile int vy = 100;
    const volatile int cvz = 0;

    // typeof_unqual 移除所有限定符
    typeof_unqual(cx) x = 10;    // int (移除 const)
    typeof_unqual(vy) y = 20;    // int (移除 volatile)
    typeof_unqual(cvz) z = 30;   // int (移除 const volatile)

    // 现在可以修改
    x = 100;  // OK
    y = 200;  // OK
    z = 300;  // OK
}

// ========== 实际应用场景 ==========

// 场景 1: 创建临时变量而不继承限定符
#define TMP_VAR(val) \
    typeof_unqual(val) _tmp = (val)

void demo_tmp_var(void) {
    const int x = 42;
    TMP_VAR(x);  // _tmp 是 int，不是 const int
    _tmp = 100;  // OK，可以修改
}

// 场景 2: 泛型交换宏
#define SWAP(a, b) do { \
    typeof_unqual(a) _tmp = (a); \
    (a) = (b); \
    (b) = _tmp; \
} while(0)

void demo_swap(void) {
    const int x = 1, y = 2;
    int a = x, b = y;
    SWAP(a, b);  // 使用无限定符类型作为临时变量
}

// 场景 3: 获取底层类型
#define UNDERLYING_TYPE(T) typeof_unqual(T)

void demo_underlying(void) {
    const volatile double cvd = 3.14;
    UNDERLYING_TYPE(cvd) d = 2.718;  // double
}
