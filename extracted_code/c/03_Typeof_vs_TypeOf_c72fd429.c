/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 856
 * Language: c
 * Block ID: c72fd429
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 函数类型推导

// 函数声明
int add(int a, int b);
double compute(int x, double y);
void callback(void (*cb)(int));

void c23_function_typeof(void) {
    // 函数类型推导
    typeof(add) add_fn;         // int(int, int) 函数类型
    typeof(compute) comp_fn;    // double(int, double) 函数类型

    // 实际使用中退化为函数指针
    typeof(add) *add_ptr = add;     // 函数指针
    typeof(&add) add_ptr2 = add;    // 显式取地址

    // 函数指针推导
    int (*fp)(int, int) = add;
    typeof(fp) fp2;             // int (*)(int, int)
    typeof(*fp) fn;             // int(int, int) - 解引用得到函数类型

    // 复杂函数指针
    void (*signal_handler)(int);
    typeof(signal_handler) sh2; // void (*)(int)

    // 返回指针的函数
    int* get_ptr(void);
    typeof(get_ptr) gp;         // int*(void)
    typeof(&get_ptr) gp_ptr;    // int* (*)(void)

    // 接受函数指针的函数
    void register_cb(void (*cb)(int));
    typeof(register_cb) rc;     // void(void (*)(int))
}
