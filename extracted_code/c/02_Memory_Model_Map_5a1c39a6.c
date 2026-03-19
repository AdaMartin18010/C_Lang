/*
 * Auto-generated from: 06_Thinking_Representation\03_Mind_Maps\02_Memory_Model_Map.md
 * Line: 468
 * Language: c
 * Block ID: 5a1c39a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* memory_layout_probe.c - C内存布局探测程序 */
/* 编译: gcc -std=c17 -o memory_layout_probe memory_layout_probe.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdalign.h>

/* 全局变量 - 放在.data段 */
int global_initialized = 42;
const int global_const = 100;

/* 未初始化全局变量 - 放在.bss段 */
int global_uninitialized;
static int static_global;

/* 字符串常量 - 放在.rodata段 */
const char *hello = "Hello, World!";

/* 函数 - 放在.text段 */
void sample_function(void) {
    static int static_local = 0;  /* .data段 */
    static int static_local_uninit; /* .bss段 */
    int local_auto;               /* 栈 */

    (void)static_local;
    (void)static_local_uninit;
    (void)local_auto;
}

/* 打印内存区域宏 */
#define PRINT_REGION(name, addr) \
    printf("  %-20s: %p\n", name, (void *)(addr))

#define PRINT_REGION_RANGE(name, start, end) \
    printf("  %-20s: %p - %p (size: %zu bytes)\n", \
           name, (void *)(start), (void *)(end), \
           (size_t)((uintptr_t)(end) - (uintptr_t)(start)))

int main(int argc, char *argv[]) {
    (void)argc;

    /* 局部变量 - 栈 */
    int local_var = 123;
    char local_array[256];
    const int local_const = 456;

    /* 动态分配 - 堆 */
    int *heap_var = malloc(sizeof(int));
    char *heap_array = malloc(1024);

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              C Memory Layout Probe (C17 Standard)              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* 命令行参数和环境变量 */
    printf("[High Address] Command Line & Environment\n");
    PRINT_REGION("argv[0]", argv[0]);
    PRINT_REGION("argv", argv);
    PRINT_REGION("environ", getenv("PATH"));
    printf("\n");

    /* 栈区 */
    printf("[Stack Segment] (grows downward)\n");
    int stack_marker1;
    int stack_marker2;
    PRINT_REGION("main local_var", &local_var);
    PRINT_REGION("main local_array", local_array);
    PRINT_REGION("main local_const", &local_const);
    PRINT_REGION("stack_marker1", &stack_marker1);
    PRINT_REGION("stack_marker2", &stack_marker2);
    printf("  Stack direction: %s\n",
           (&stack_marker1 > &stack_marker2) ? "Downward" : "Upward");
    printf("\n");

    /* 堆区 */
    printf("[Heap Segment] (grows upward)\n");
    PRINT_REGION("heap_var", heap_var);
    PRINT_REGION("heap_array", heap_array);
    printf("\n");

    /* BSS段 */
    printf("[.bss Segment] (uninitialized data)\n");
    PRINT_REGION("global_uninitialized", &global_uninitialized);
    PRINT_REGION("static_global", &static_global);
    printf("  Value check (should be 0): global_uninitialized=%d, static_global=%d\n",
           global_uninitialized, static_global);
    printf("\n");

    /* Data段 */
    printf("[.data Segment] (initialized data)\n");
    PRINT_REGION("global_initialized", &global_initialized);
    printf("  Value: %d\n", global_initialized);
    printf("\n");

    /* 只读数据段 */
    printf("[.rodata Segment] (read-only data)\n");
    PRINT_REGION("global_const", &global_const);
    PRINT_REGION("hello string", hello);
    PRINT_REGION("hello pointer", &hello);
    printf("\n");

    /* 代码段 */
    printf("[.text Segment] (code)\n");
    PRINT_REGION("main function", main);
    PRINT_REGION("sample_function", sample_function);
    PRINT_REGION("printf function", printf);
    printf("\n");

    /* 函数指针测试 */
    printf("[Function Pointer Test]\n");
    void (*func_ptr)(void) = sample_function;
    PRINT_REGION("func_ptr", func_ptr);
    printf("\n");

    /* 内存区域顺序分析 */
    printf("[Memory Layout Summary]\n");

    uintptr_t text_start = (uintptr_t)main & ~0xFFF;
    uintptr_t stack_top = (uintptr_t)&stack_marker1;
    uintptr_t heap_start = (uintptr_t)heap_var;

    printf("  Text segment base:   %p\n", (void *)text_start);
    printf("  Heap start:          %p\n", (void *)heap_start);
    printf("  Stack top:           %p\n", (void *)stack_top);
    printf("\n");

    /* 对齐信息 */
    printf("[Alignment Information]\n");
    printf("  alignof(char)      = %2zu, sizeof = %zu\n",
           alignof(char), sizeof(char));
    printf("  alignof(short)     = %2zu, sizeof = %zu\n",
           alignof(short), sizeof(short));
    printf("  alignof(int)       = %2zu, sizeof = %zu\n",
           alignof(int), sizeof(int));
    printf("  alignof(long)      = %2zu, sizeof = %zu\n",
           alignof(long), sizeof(long));
    printf("  alignof(long long) = %2zu, sizeof = %zu\n",
           alignof(long long), sizeof(long long));
    printf("  alignof(float)     = %2zu, sizeof = %zu\n",
           alignof(float), sizeof(float));
    printf("  alignof(double)    = %2zu, sizeof = %zu\n",
           alignof(double), sizeof(double));
    printf("  alignof(void*)     = %2zu, sizeof = %zu\n",
           alignof(void*), sizeof(void*));
    printf("\n");

    /* 结构体布局演示 */
    printf("[Struct Layout Demo]\n");

    struct TestStruct {
        char a;
        int b;
        short c;
        char d;
    };

    printf("  struct { char a; int b; short c; char d; }\n");
    printf("  sizeof(TestStruct)  = %zu\n", sizeof(struct TestStruct));
    printf("  alignof(TestStruct) = %zu\n", alignof(struct TestStruct));

    struct TestStruct ts;
    printf("  Offset of a: %zu\n", (size_t)((char *)&ts.a - (char *)&ts));
    printf("  Offset of b: %zu\n", (size_t)((char *)&ts.b - (char *)&ts));
    printf("  Offset of c: %zu\n", (size_t)((char *)&ts.c - (char *)&ts));
    printf("  Offset of d: %zu\n", (size_t)((char *)&ts.d - (char *)&ts));
    printf("\n");

    /* 清理 */
    free(heap_var);
    free(heap_array);

    printf("Memory layout probe completed.\n");
    return 0;
}
