/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 1594
 * Language: c
 * Block ID: 8c0d5479
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 typeof 的限制与边界情况

// ========== 限制 1: 不能在类型定义中使用 ==========

// ❌ 错误: typeof 不能用于定义新类型名
typedef typeof(int) MyInt;  // 某些编译器可能允许，但非标准

// ✅ 替代方案: 使用 typeof 声明变量
void demo1(void) {
    typeof(int) x;  // OK
}

// ========== 限制 2: 不完全类型 ==========

struct Incomplete;  // 不完全类型

void demo2(void) {
    // ❌ 错误: 不能使用不完全类型
    // typeof(struct Incomplete) x;
}

// ========== 限制 3: VLA (变长数组) ==========

void demo3(int n) {
    int vla[n];
    // typeof(vla) 的行为是未定义或实现定义
    // 某些编译器可能推导出 int* 而不是 VLA 类型
}

// ========== 限制 4: 位域 ==========

struct BitField {
    int x : 4;
};

void demo4(void) {
    struct BitField bf;
    // typeof(bf.x) 推导为 int，但丢失了位域信息
    typeof(bf.x) normal_int;
    normal_int = 100;  // 可以使用完整的 int 范围
}

// ========== 限制 5: 复杂的限定符交互 ==========

void demo5(void) {
    // restrict 限定符的复杂性
    int* restrict rp;
    typeof(rp) rp2;  // int* restrict

    // typeof_unqual 不适用于 restrict
    typeof_unqual(rp) rp3;  // int* (restrict 被移除)

    // 多重限定符的处理可能令人困惑
    const int* volatile* cvpp;
    typeof(cvpp) cvpp2;         // const int* volatile*
    typeof_unqual(cvpp) cvpp3;  // const int** (只移除顶层 volatile)
}

// ========== 限制 6: 函数类型的退化 ==========

void demo6(void) {
    int fn(int);

    typeof(fn) f;    // 函数类型
    // f();  // 错误: 函数类型不能直接使用

    typeof(&fn) fp;  // 函数指针类型
    fp = fn;         // OK
}

// ========== 限制 7: 宏展开中的复杂性 ==========

#define TYPEOF_SWAP(a, b) do { \
    typeof(a) _tmp = (a); \
    (a) = (b); \
    (b) = _tmp; \
} while(0)

void demo7(void) {
    const int x = 1, y = 2;
    // TYPEOF_SWAP(x, y);  // 错误: _tmp 是 const int，不能赋值

    // 需要使用 typeof_unqual
}
