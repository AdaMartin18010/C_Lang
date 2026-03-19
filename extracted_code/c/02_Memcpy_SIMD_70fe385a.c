/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 81
 * Language: c
 * Block ID: 70fe385a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * x86 SIMD指令集演进：
 *
 * MMX (1997): 64-bit整数，复用FPU寄存器
 * SSE (1999): 128-bit浮点，独立XMM寄存器
 * SSE2 (2001): 128-bit整数，扩展到16个XMM
 * AVX (2011): 256-bit，YMM寄存器，三操作数
 * AVX2 (2013): 256-bit整数， gather指令
 * AVX-512 (2016+): 512-bit，ZMM寄存器，掩码操作
 */

// 特性检测
#include <cpuid.h>

typedef enum {
    SIMD_NONE,
    SIMD_SSE2,
    SIMD_AVX,
    SIMD_AVX2,
    SIMD_AVX512F
} SimdLevel;

SimdLevel detect_simd_level(void) {
    unsigned int eax, ebx, ecx, edx;

    // 检查AVX-512
    __cpuid_count(7, 0, eax, ebx, ecx, edx);
    if (ebx & (1 << 16)) return SIMD_AVX512F;  // AVX512F

    // 检查AVX2
    if (ebx & (1 << 5)) return SIMD_AVX2;

    // 检查AVX
    __cpuid(1, eax, ebx, ecx, edx);
    if (ecx & (1 << 28)) return SIMD_AVX;

    // 检查SSE2
    if (edx & (1 << 26)) return SIMD_SSE2;

    return SIMD_NONE;
}

// 函数指针表
typedef void* (*MemcpyFunc)(void*, const void*, size_t);

static MemcpyFunc memcpy_impl = NULL;

void init_memcpy(void) {
    switch (detect_simd_level()) {
        case SIMD_AVX512F:
            memcpy_impl = memcpy_avx512;
            break;
        case SIMD_AVX2:
            memcpy_impl = memcpy_avx2;
            break;
        case SIMD_AVX:
            memcpy_impl = memcpy_avx;
            break;
        case SIMD_SSE2:
            memcpy_impl = memcpy_sse2;
            break;
        default:
            memcpy_impl = memcpy_generic;
    }
}
