/*
 * Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\README.md
 * Line: 71
 * Language: c
 * Block ID: 84812995
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C代码使用Zig编译的库
#include <stdio.h>

extern int add(int a, int b);
extern unsigned int fibonacci(unsigned int n);

int main() {
    printf("2 + 3 = %d\n", add(2, 3));
    printf("fib(10) = %u\n", fibonacci(10));
    return 0;
}
