/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 407
 * Language: c
 * Block ID: c5d7c920
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// riscv_vector.h - RVV intrinsic头文件 (LLVM/Clang)
#include <riscv_vector.h>

// 设置向量长度
size_t vl = vsetvl_e32m8(len);  // 设置处理float32的向量长度，LMUL=8

// 向量类型
vfloat32m8_t - 8个向量寄存器组，float32元素
vint32m4_t   - 4个向量寄存器组，int32元素
vint16m2_t   - 2个向量寄存器组，int16元素
vint8m1_t    - 1个向量寄存器组，int8元素

// 命名约定: v{type}{lmul}
// type: float32, int32, int16, int8, uint32, etc.
// lmul: m1, m2, m4, m8 (寄存器组乘数)
