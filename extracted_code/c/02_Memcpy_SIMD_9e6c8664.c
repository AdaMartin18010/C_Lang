/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 593
 * Language: c
 * Block ID: 9e6c8664
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ARM NEON (128-bit SIMD)：
 * - 32个128-bit寄存器（V0-V31）
 * - 支持8/16/32/64-bit整数和浮点
 * - 加载/存储需要地址对齐
 */

#ifdef __ARM_NEON
#include <arm_neon.h>

void* memcpy_neon(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;

    // 头部处理
    while (n > 0 && ((uintptr_t)d & 15)) {
        *d++ = *s++;
        n--;
    }

    // 主体：NEON 128-bit
    while (n >= 64) {
        uint8x16_t v0 = vld1q_u8(s + 0);
        uint8x16_t v1 = vld1q_u8(s + 16);
        uint8x16_t v2 = vld1q_u8(s + 32);
        uint8x16_t v3 = vld1q_u8(s + 48);

        vst1q_u8(d + 0, v0);
        vst1q_u8(d + 16, v1);
        vst1q_u8(d + 32, v2);
        vst1q_u8(d + 48, v3);

        s += 64;
        d += 64;
        n -= 64;
    }

    // 剩余16字节块
    while (n >= 16) {
        vst1q_u8(d, vld1q_u8(s));
        s += 16;
        d += 16;
        n -= 16;
    }

    // 尾部
    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dst;
}

// ARM SVE (Scalable Vector Extensions)
#ifdef __ARM_FEATURE_SVE
#include <arm_sve.h>

void* memcpy_sve(void *dst, const void *src, size_t n) {
    // SVE支持可变向量长度
    // 代码可以适应128-bit到2048-bit

    svbool_t pg = svwhilelt_b8_u64(0, n);

    while (svptest_any(svptrue_b8(), pg)) {
        svuint8_t v = svld1_u8(pg, src);
        svst1_u8(pg, dst, v);

        src += svcntb();
        dst += svcntb();
        n -= svcntb();

        pg = svwhilelt_b8_u64(0, n);
    }

    return dst;
}
#endif

#endif  // __ARM_NEON
