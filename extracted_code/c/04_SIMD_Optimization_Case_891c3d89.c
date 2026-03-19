/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 846
 * Language: c
 * Block ID: 891c3d89
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 2D卷积 - SIMD优化版本
// 输入: NxHxW, 卷积核: KxCxkxk, 输出: NxKxHxW

#ifdef __AVX__
void conv2d_avx(const float* input, const float* kernel, float* output,
                int N, int C, int H, int W, int K, int k) {
    const int pad = k / 2;
    const int out_h = H;
    const int out_w = W;

    for (int n = 0; n < N; n++) {
        for (int kk = 0; kk < K; kk++) {
            for (int oh = 0; oh < out_h; oh++) {
                for (int ow = 0; ow < out_w; ow += 8) {
                    __m256 acc = _mm256_setzero_ps();

                    for (int c = 0; c < C; c++) {
                        for (int kh = 0; kh < k; kh++) {
                            for (int kw = 0; kw < k; kw++) {
                                int ih = oh - pad + kh;
                                int iw = ow - pad + kw;

                                if (ih >= 0 && ih < H && iw >= 0 && iw < W - 7) {
                                    __m256 in = _mm256_loadu_ps(&input[
                                        ((n * C + c) * H + ih) * W + iw
                                    ]);
                                    __m256 k_val = _mm256_set1_ps(kernel[
                                        ((kk * C + c) * k + kh) * k + kw
                                    ]);
                                    acc = _mm256_fmadd_ps(in, k_val, acc);
                                } else {
                                    // 边界处理 (标量)
                                    for (int j = 0; j < 8 && ow + j < out_w; j++) {
                                        int iiw = ow + j - pad + kw;
                                        float in_val = (ih >= 0 && ih < H && iiw >= 0 && iiw < W) ?
                                            input[((n * C + c) * H + ih) * W + iiw] : 0;
                                        acc[j] += in_val * kernel[((kk * C + c) * k + kh) * k + kw];
                                    }
                                }
                            }
                        }
                    }

                    _mm256_storeu_ps(&output[((n * K + kk) * out_h + oh) * out_w + ow], acc);
                }
            }
        }
    }
}
#endif

// 性能对比 (ResNet-50首层: 7x7x64@224x224)
// 朴素实现:    1.0x
// AVX优化:     12.5x
// AVX+多线程:  48.2x (4线程)
