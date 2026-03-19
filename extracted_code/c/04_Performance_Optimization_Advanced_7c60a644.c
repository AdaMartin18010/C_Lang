/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 3765
 * Language: c
 * Block ID: 7c60a644
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file vectorized_algorithms.c
 * @brief 算法级向量化优化
 *
 * 向量化算法设计原则：
 * - 数据布局：结构体数组(AoS) -> 数组结构体(SoA)
 * - 减少分支：分支预测失败代价高
 * - 合并循环：提高数据重用
 * - 分块处理：适应缓存层次
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>

/**
 * @brief 数据布局转换：AoS -> SoA
 *
 * AoS (Array of Structs): {x,y,z},{x,y,z}... - 不利于SIMD
 * SoA (Struct of Arrays): xxxx...,yyyy...,zzzz... - 适合SIMD
 */

typedef struct {
    float x, y, z;
} Vec3_AoS;

typedef struct {
    float *x, *y, *z;
} Vec3_SoA;

/* AoS向量加法 */
void add_aos(Vec3_AoS *dst, const Vec3_AoS *a, const Vec3_AoS *b, int n) {
    for (int i = 0; i < n; i++) {
        dst[i].x = a[i].x + b[i].x;
        dst[i].y = a[i].y + b[i].y;
        dst[i].z = a[i].z + b[i].z;
    }
}

/* SoA向量加法 - 可向量化 */
void add_soa(Vec3_SoA *dst, const Vec3_SoA *a, const Vec3_SoA *b, int n) {
    /* x分量 - 可向量化 */
    for (int i = 0; i < n; i++) {
        dst->x[i] = a->x[i] + b->x[i];
    }
    /* y分量 - 可向量化 */
    for (int i = 0; i < n; i++) {
        dst->y[i] = a->y[i] + b->y[i];
    }
    /* z分量 - 可向量化 */
    for (int i = 0; i < n; i++) {
        dst->z[i] = a->z[i] + b->z[i];
    }
}

/* SoA + AVX向量化 */
void add_soa_avx(Vec3_SoA *dst, const Vec3_SoA *a, const Vec3_SoA *b, int n) {
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 ax = _mm256_loadu_ps(&a->x[i]);
        __m256 bx = _mm256_loadu_ps(&b->x[i]);
        _mm256_storeu_ps(&dst->x[i], _mm256_add_ps(ax, bx));

        __m256 ay = _mm256_loadu_ps(&a->y[i]);
        __m256 by = _mm256_loadu_ps(&b->y[i]);
        _mm256_storeu_ps(&dst->y[i], _mm256_add_ps(ay, by));

        __m256 az = _mm256_loadu_ps(&a->z[i]);
        __m256 bz = _mm256_loadu_ps(&b->z[i]);
        _mm256_storeu_ps(&dst->z[i], _mm256_add_ps(az, bz));
    }
    /* 剩余元素 */
    for (; i < n; i++) {
        dst->x[i] = a->x[i] + b->x[i];
        dst->y[i] = a->y[i] + b->y[i];
        dst->z[i] = a->z[i] + b->z[i];
    }
}

/**
 * @brief 向量化查找表
 *
 * 使用 gather 指令从非连续位置加载数据
 */
void lookup_table_simd(const float *table, const int *indices,
                       float *output, int n) {
    int i = 0;
    /* AVX2支持gather，但这里使用AVX模拟 */
    for (; i <= n - 8; i += 8) {
        /* 手动gather（无AVX2时） */
        for (int j = 0; j < 8; j++) {
            output[i + j] = table[indices[i + j]];
        }
    }
    for (; i < n; i++) {
        output[i] = table[indices[i]];
    }
}

/**
 * @brief 向量化前缀和（扫描）
 *
 * 使用Kogge-Stone算法进行SIMD前缀和
 */
void prefix_sum_scalar(float *dst, const float *src, int n) {
    dst[0] = src[0];
    for (int i = 1; i < n; i++) {
        dst[i] = dst[i-1] + src[i];
    }
}

/* SIMD前缀和 - 分块处理 */
void prefix_sum_simd(float *dst, const float *src, int n) {
    const int BLOCK = 1024;  /* 每块大小 */

    /* 块内扫描 */
    for (int block_start = 0; block_start < n; block_start += BLOCK) {
        int block_end = (block_start + BLOCK < n) ? block_start + BLOCK : n;

        /* 标量扫描块内 */
        dst[block_start] = src[block_start];
        for (int i = block_start + 1; i < block_end; i++) {
            dst[i] = dst[i-1] + src[i];
        }
    }

    /* 块间修正（需要二次扫描） */
    float correction = 0;
    for (int block_start = 0; block_start < n; block_start += BLOCK) {
        int block_end = (block_start + BLOCK < n) ? block_start + BLOCK : n;
        float block_sum = dst[block_end - 1];

        for (int i = block_start; i < block_end; i++) {
            dst[i] += correction;
        }
        correction += block_sum;
    }
}

/**
 * @brief 向量化归约操作
 *
 * 使用多层级归约最大化并行
 */
float sum_reduction_scalar(const float *arr, int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

float sum_reduction_simd(const float *arr, int n) {
    __m256 sum8 = _mm256_setzero_ps();
    int i = 0;

    /* 第一级：8路SIMD归约 */
    for (; i <= n - 8; i += 8) {
        __m256 v = _mm256_loadu_ps(&arr[i]);
        sum8 = _mm256_add_ps(sum8, v);
    }

    /* 第二级：水平归约到4元素 */
    __m128 low = _mm256_castps256_ps128(sum8);
    __m128 high = _mm256_extractf128_ps(sum8, 1);
    __m128 sum4 = _mm_add_ps(low, high);

    /* 第三级：水平归约到标量 */
    sum4 = _mm_hadd_ps(sum4, sum4);
    sum4 = _mm_hadd_ps(sum4, sum4);
    float sum = _mm_cvtss_f32(sum4);

    /* 剩余元素 */
    for (; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

/**
 * @brief 性能基准测试
 */
#define N (10 * 1024 * 1024)  /* 10M元素 */

void benchmark_vectorized_algorithms(void) {
    printf("=== Vectorized Algorithms Benchmark ===\n\n");

    /* 分配内存 */
    Vec3_AoS *aos_a = aligned_alloc(32, N * sizeof(Vec3_AoS));
    Vec3_AoS *aos_b = aligned_alloc(32, N * sizeof(Vec3_AoS));
    Vec3_AoS *aos_dst = aligned_alloc(32, N * sizeof(Vec3_AoS));

    Vec3_SoA soa_a = {
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float))
    };
    Vec3_SoA soa_b = {
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float))
    };
    Vec3_SoA soa_dst = {
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float))
    };

    /* 初始化 */
    for (int i = 0; i < N; i++) {
        aos_a[i].x = aos_a[i].y = aos_a[i].z = (float)i;
        aos_b[i].x = aos_b[i].y = aos_b[i].z = (float)(N - i);
        soa_a.x[i] = soa_a.y[i] = soa_a.z[i] = (float)i;
        soa_b.x[i] = soa_b.y[i] = soa_b.z[i] = (float)(N - i);
    }

    /* 测试AoS */
    clock_t start = clock();
    add_aos(aos_dst, aos_a, aos_b, N);
    clock_t aos_time = clock() - start;
    printf("AoS addition:        %ld ms\n", aos_time * 1000 / CLOCKS_PER_SEC);

    /* 测试SoA */
    start = clock();
    add_soa(&soa_dst, &soa_a, &soa_b, N);
    clock_t soa_time = clock() - start;
    printf("SoA addition:        %ld ms (%.2fx)\n",
           soa_time * 1000 / CLOCKS_PER_SEC, (double)aos_time / soa_time);

    /* 测试SoA+AVX */
    start = clock();
    add_soa_avx(&soa_dst, &soa_a, &soa_b, N);
    clock_t soa_avx_time = clock() - start;
    printf("SoA+AVX addition:    %ld ms (%.2fx)\n",
           soa_avx_time * 1000 / CLOCKS_PER_SEC, (double)aos_time / soa_avx_time);

    /* 测试归约 */
    float *arr = aligned_alloc(32, N * sizeof(float));
    for (int i = 0; i < N; i++) arr[i] = 1.0f;

    start = clock();
    volatile float sum1 = sum_reduction_scalar(arr, N);
    clock_t scalar_sum_time = clock() - start;
    printf("\nScalar sum:          %ld ms (sum=%.0f)\n",
           scalar_sum_time * 1000 / CLOCKS_PER_SEC, sum1);

    start = clock();
    volatile float sum2 = sum_reduction_simd(arr, N);
    clock_t simd_sum_time = clock() - start;
    printf("SIMD sum:            %ld ms (sum=%.0f, %.2fx)\n",
           simd_sum_time * 1000 / CLOCKS_PER_SEC, sum2,
           (double)scalar_sum_time / simd_sum_time);

    /* 清理 */
    free(aos_a); free(aos_b); free(aos_dst);
    free(soa_a.x); free(soa_a.y); free(soa_a.z);
    free(soa_b.x); free(soa_b.y); free(soa_b.z);
    free(soa_dst.x); free(soa_dst.y); free(soa_dst.z);
    free(arr);
}

int main(void) {
    benchmark_vectorized_algorithms();
    return 0;
}
