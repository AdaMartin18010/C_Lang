/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 458
 * Language: c
 * Block ID: 2fda95d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 掩码操作 (条件执行)
void vec_add_masked(const float* a, const float* b, float* c,
                    const uint8_t* mask, size_t n) {
    for (size_t i = 0; i < n; ) {
        size_t vl = vsetvl_e32m8(n - i);

        // 加载掩码 (转换为bool向量)
        vuint8m2_t mask8 = vle8_v_u8m2(&mask[i], vl);
        vbool4_t mask = vmsne_vx_u8m2_b4(mask8, 0, vl);

        // 加载数据
        vfloat32m8_t va = vle32_v_f32m8(&a[i], vl);
        vfloat32m8_t vb = vle32_v_f32m8(&b[i], vl);

        // 掩码加法
        vfloat32m8_t vc = vfadd_vv_f32m8_m(mask, va, va, vb, vl);

        vse32_v_f32m8(&c[i], vc, vl);
        i += vl;
    }
}

// 向量归约 (求和)
float vec_sum_rvv(const float* a, size_t n) {
    vfloat32m1_t vsum = vfmv_v_f_f32m1(0.0f, 1);  // 初始化为0

    for (size_t i = 0; i < n; ) {
        size_t vl = vsetvl_e32m8(n - i);
        vfloat32m8_t va = vle32_v_f32m8(&a[i], vl);

        // 部分归约到vsum
        vsum = vfredosum_vs_f32m8_f32m1(vsum, va, vsum, vl);

        i += vl;
    }

    // 提取标量结果
    return vfmv_f_s_f32m1_f32(vsum);
}

// 矩阵乘法 (RVV)
void matmul_rvv(const float* A, const float* B, float* C,
                int M, int N, int K) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; ) {
            size_t vl = vsetvl_e32m8(N - j);

            // 初始化累加器为0
            vfloat32m8_t acc = vfmv_v_f_f32m8(0.0f, vl);

            for (int k = 0; k < K; k++) {
                // 广播A[i][k]
                float a_val = A[i*K + k];
                vfloat32m8_t a_vec = vfmv_v_f_f32m8(a_val, vl);

                // 加载B[k][j:j+vl]
                vfloat32m8_t b_vec = vle32_v_f32m8(&B[k*N + j], vl);

                // FMA: acc += a_vec * b_vec
                acc = vfmacc_vv_f32m8(acc, a_vec, b_vec, vl);
            }

            // 存储结果C[i][j:j+vl]
            vse32_v_f32m8(&C[i*N + j], acc, vl);
            j += vl;
        }
    }
}
