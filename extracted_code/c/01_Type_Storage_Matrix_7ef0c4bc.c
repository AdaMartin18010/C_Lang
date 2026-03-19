/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\01_Type_Storage_Matrix.md
 * Line: 56
 * Language: c
 * Block ID: 7ef0c4bc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// type_sizes.c - 打印各平台类型大小
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 平台检测宏
#if defined(__x86_64__) || defined(_M_X64)
    #define PLATFORM "x86-64"
#elif defined(__i386__) || defined(_M_IX86)
    #define PLATFORM "x86-32"
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define PLATFORM "ARM64"
#elif defined(__arm__) || defined(_M_ARM)
    #define PLATFORM "ARM32"
#else
    #define PLATFORM "Unknown"
#endif

// 编译器检测
#if defined(__GNUC__)
    #define COMPILER "GCC " __VERSION__
#elif defined(__clang__)
    #define COMPILER "Clang"
#elif defined(_MSC_VER)
    #define COMPILER "MSVC"
#else
    #define COMPILER "Unknown"
#endif

int main(void) {
    printf("========================================\n");
    printf("Platform: %s\n", PLATFORM);
    printf("Compiler: %s\n", COMPILER);
    printf("========================================\n\n");

    printf("=== 整数类型 ===\n");
    printf("sizeof(char)      = %zu bytes (%zu bits)\n",
           sizeof(char), sizeof(char) * 8);
    printf("sizeof(short)     = %zu bytes (%zu bits)\n",
           sizeof(short), sizeof(short) * 8);
    printf("sizeof(int)       = %zu bytes (%zu bits)\n",
           sizeof(int), sizeof(int) * 8);
    printf("sizeof(long)      = %zu bytes (%zu bits)\n",
           sizeof(long), sizeof(long) * 8);
    printf("sizeof(long long) = %zu bytes (%zu bits)\n",
           sizeof(long long), sizeof(long long) * 8);
    printf("sizeof(size_t)    = %zu bytes (%zu bits)\n",
           sizeof(size_t), sizeof(size_t) * 8);
    printf("sizeof(ptrdiff_t) = %zu bytes (%zu bits)\n",
           sizeof(ptrdiff_t), sizeof(ptrdiff_t) * 8);

    printf("\n=== 固定宽度整数类型 (stdint.h) ===\n");
    printf("sizeof(int8_t)    = %zu bytes\n", sizeof(int8_t));
    printf("sizeof(int16_t)   = %zu bytes\n", sizeof(int16_t));
    printf("sizeof(int32_t)   = %zu bytes\n", sizeof(int32_t));
    printf("sizeof(int64_t)   = %zu bytes\n", sizeof(int64_t));
    printf("sizeof(uintptr_t) = %zu bytes\n", sizeof(uintptr_t));
    printf("sizeof(intptr_t)  = %zu bytes\n", sizeof(intptr_t));

    printf("\n=== 浮点类型 ===\n");
    printf("sizeof(float)       = %zu bytes (%zu bits)\n",
           sizeof(float), sizeof(float) * 8);
    printf("sizeof(double)      = %zu bytes (%zu bits)\n",
           sizeof(double), sizeof(double) * 8);
    printf("sizeof(long double) = %zu bytes (%zu bits)\n",
           sizeof(long double), sizeof(long double) * 8);

    printf("\n=== 指针类型 ===\n");
    printf("sizeof(void*)       = %zu bytes\n", sizeof(void*));
    printf("sizeof(char*)       = %zu bytes\n", sizeof(char*));
    printf("sizeof(int*)        = %zu bytes\n", sizeof(int*));
    printf("sizeof(double*)     = %zu bytes\n", sizeof(double*));
    printf("sizeof(void(*)())   = %zu bytes\n", sizeof(void(*)(void)));

    printf("\n=== 其他类型 ===\n");
    printf("sizeof(bool)        = %zu bytes (stdbool.h)\n", sizeof(bool));
    printf("sizeof(wchar_t)     = %zu bytes\n", sizeof(wchar_t));

    return 0;
}
