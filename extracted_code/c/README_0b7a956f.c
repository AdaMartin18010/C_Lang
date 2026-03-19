/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\README.md
 * Line: 206
 * Language: c
 * Block ID: 0b7a956f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// const 限定：只读语义
const int MAX_VALUE = 100;         // 常量值
const char *str = "hello";         // 指向常量的指针
char * const ptr = buffer;         // 常量指针
const char * const p = "fixed";    // 常量指针指向常量

// volatile 限定：易变语义
volatile int sensor_value;         // 可能被硬件改变
volatile uint32_t *hardware_reg;   // 内存映射寄存器

// restrict 限定 (C99)：指针别名优化提示
void vector_add(int n,
                double *restrict a,
                const double *restrict b,
                const double *restrict c)
{
    // 编译器可假设 a, b, c 指向不重叠内存
    for (int i = 0; i < n; i++) {
        a[i] = b[i] + c[i];
    }
}

// _Atomic 限定 (C11)：原子操作
#include <stdatomic.h>
_Atomic int counter = 0;

void increment(void) {
    atomic_fetch_add(&counter, 1);  // 线程安全自增
}

// 存储类说明符
static int internal_count;         // 内部链接
extern int shared_variable;        // 外部声明
thread_local int thread_data;      // 线程本地存储 (C11)
