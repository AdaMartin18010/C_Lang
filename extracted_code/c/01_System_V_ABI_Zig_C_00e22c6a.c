/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 1357
 * Language: c
 * Block ID: 00e22c6a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: va_list 定义 (glibc x86-64)

// 在 <stdarg.h> 中定义
typedef struct {
    unsigned int gp_offset;      // 下一个 GP 参数偏移 (字节)
    unsigned int fp_offset;      // 下一个 FP 参数偏移 (字节)
    void* overflow_arg_area;     // 栈参数区指针
    void* reg_save_area;         // 寄存器保存区指针
} va_list[1];

// 变参函数示例
int sum_variadic(int count, ...) {
    va_list args;
    va_start(args, count);

    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += va_arg(args, int);
    }

    va_end(args);
    return sum;
}

// 使用
int result = sum_variadic(5, 1, 2, 3, 4, 5);  // result = 15
