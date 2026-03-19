/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\02_Error_Correction_Coding.md
 * Line: 72
 * Language: c
 * Block ID: f66086c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// Reed-Solomon编码器/解码器 - DNA存储优化版本
// 基于GF(256)有限域
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// GF(256)参数
#define GF_SIZE         256
#define GF_POLY         0x11D   // 本原多项式: x^8 + x^4 + x^3 + x^2 + 1

// RS码参数 (可配置)
#define RS_N            255     // 码长
#define RS_K            223     // 信息符号数
#define RS_T            16      // 纠错能力 (t = (n-k)/2)
#define RS_2T           32      // 2t

// GF(256)运算表
typedef struct {
    uint8_t exp[GF_SIZE * 2];   // 指数表
    uint8_t log[GF_SIZE];       // 对数表
} GFTables;

// RS编解码器上下文
typedef struct {
    GFTables gf;
    uint8_t generator[RS_2T + 1];   // 生成多项式
    uint8_t n;                      // 实际码长
    uint8_t k;                      // 实际信息长
    uint8_t t;                      // 纠错能力
} RSContext;

// ============================================================================
// GF(256)有限域运算初始化
// ============================================================================

void gf_init(GFTables *gf) {
    // 构建指数表和对数表
    uint16_t x = 1;

    for (int i = 0; i < GF_SIZE - 1; i++) {
        gf->exp[i] = (uint8_t)x;
        gf->exp[i + GF_SIZE - 1] = (uint8_t)x;  // 重复一遍方便计算
        gf->log[x] = i;

        // x = x * 2 mod GF_POLY
        x <<= 1;
        if (x & GF_SIZE) {
            x ^= GF_POLY;
        }
    }

    gf->exp[GF_SIZE * 2 - 1] = gf->exp[GF_SIZE - 1];
    gf->log[0] = 0;  // log(0)未定义，设为0
}

// GF加法 = XOR
static inline uint8_t gf_add(uint8_t a, uint8_t b) {
    return a ^ b;
}

// GF减法 = XOR
static inline uint8_t gf_sub(uint8_t a, uint8_t b) {
    return a ^ b;
}

// GF乘法
static inline uint8_t gf_mul(const GFTables *gf, uint8_t a, uint8_t b) {
    if (a == 0 || b == 0) return 0;
    return gf->exp[gf->log[a] + gf->log[b]];
}

// GF除法
static inline uint8_t gf_div(const GFTables *gf, uint8_t a, uint8_t b) {
    if (b == 0) return 0;  // 错误
    if (a == 0) return 0;
    return gf->exp[gf->log[a] - gf->log[b] + GF_SIZE - 1];
}

// GF幂
static inline uint8_t gf_pow(const GFTables *gf, uint8_t a, int8_t n) {
    if (a == 0) return 0;
    if (n == 0) return 1;

    int log_a = gf->log[a];
    int result_log = (log_a * n) % (GF_SIZE - 1);
    if (result_log < 0) result_log += GF_SIZE - 1;

    return gf->exp[result_log];
}

// ============================================================================
// RS生成多项式
// g(x) = (x - α^0)(x - α^1)...(x - α^(2t-1))
// ============================================================================

void rs_init_generator(RSContext *rs) {
    // 初始多项式为1
    memset(rs->generator, 0, sizeof(rs->generator));
    rs->generator[0] = 1;

    // 迭代乘以 (x - α^i)
    for (int i = 0; i < rs->t * 2; i++) {
        uint8_t root = rs->gf.exp[i];  // α^i

        // 从后往前更新多项式系数
        for (int j = rs->t * 2; j > 0; j--) {
            rs->generator[j] = gf_add(
                rs->generator[j],
                gf_mul(&rs->gf, rs->generator[j - 1], root)
            );
        }
    }
}

// ============================================================================
// RS编码
// ============================================================================

void rs_encode(RSContext *rs, const uint8_t *data, uint8_t *parity) {
    // 清零校验符号
    memset(parity, 0, rs->t * 2);

    // 多项式除法
    for (int i = 0; i < rs->k; i++) {
        uint8_t feedback = gf_add(data[i], parity[0]);

        if (feedback != 0) {
            for (int j = 1; j < rs->t * 2; j++) {
                parity[j - 1] = gf_sub(parity[j],
                                       gf_mul(&rs->gf, feedback, rs->generator[rs->t * 2 - j]));
            }
            parity[rs->t * 2 - 1] = gf_mul(&rs->gf, feedback, rs->generator[0]);
        } else {
            // 移位
            for (int j = 1; j < rs->t * 2; j++) {
                parity[j - 1] = parity[j];
            }
            parity[rs->t * 2 - 1] = 0;
        }
    }
}

// ============================================================================
// RS译码 - Berlekamp-Massey算法
// ============================================================================

// 综合征计算
void rs_compute_syndromes(RSContext *rs, const uint8_t *received,
                          uint8_t *syndromes) {
    for (int i = 0; i < rs->t * 2; i++) {
        syndromes[i] = 0;
        uint8_t x = rs->gf.exp[i];  // α^i

        for (int j = 0; j < rs->n; j++) {
            syndromes[i] = gf_add(syndromes[i],
                                  gf_mul(&rs->gf, received[j], x));
            x = gf_mul(&rs->gf, x, rs->gf.exp[i]);  // x *= α^i
        }
    }
}

// Berlekamp-Massey算法求错误位置多项式
int rs_berlekamp_massey(RSContext *rs, const uint8_t *syndromes,
                        uint8_t *error_locator) {
    uint8_t b[RS_2T + 1];   // 辅助多项式
    uint8_t c[RS_2T + 1];   // 错误位置多项式
    uint8_t t[RS_2T + 1];   // 临时多项式

    int L = 0;              // 当前错误位置多项式阶数
    int m = 1;              // 上次更新的位置
    uint8_t b0 = 1;         // b(x)的常数项

    // 初始化
    memset(c, 0, sizeof(c));
    memset(b, 0, sizeof(b));
    c[0] = 1;
    b[0] = 1;

    for (int n = 0; n < rs->t * 2; n++) {
        // 计算差异
        uint8_t delta = syndromes[n];
        for (int i = 1; i <= L; i++) {
            delta = gf_add(delta, gf_mul(&rs->gf, c[i], syndromes[n - i]));
        }

        if (delta == 0) {
            // 无需更新
            m++;
        } else if (2 * L <= n) {
            // 更新多项式
            memcpy(t, c, sizeof(c));
            uint8_t scale = gf_div(&rs->gf, delta, b0);

            for (int i = 0; i + m < rs->t * 2; i++) {
                c[i + m] = gf_sub(c[i + m], gf_mul(&rs->gf, scale, b[i]));
            }

            L = n + 1 - L;
            memcpy(b, t, sizeof(t));
            b0 = delta;
            m = 1;
        } else {
            // 更新但不改变L
            uint8_t scale = gf_div(&rs->gf, delta, b0);
            for (int i = 0; i + m < rs->t * 2; i++) {
                c[i + m] = gf_sub(c[i + m], gf_mul(&rs->gf, scale, b[i]));
            }
            m++;
        }
    }

    memcpy(error_locator, c, sizeof(c));
    return L;
}

// Chien搜索求错误位置
int rs_chien_search(RSContext *rs, const uint8_t *error_locator, int L,
                    uint8_t *error_positions) {
    int error_count = 0;

    for (int i = 0; i < rs->n; i++) {
        uint8_t x = rs->gf.exp[GF_SIZE - 1 - i];  // α^(-i) = α^(255-i)
        uint8_t sum = error_locator[0];
        uint8_t x_power = 1;

        for (int j = 1; j <= L; j++) {
            x_power = gf_mul(&rs->gf, x_power, x);
            sum = gf_add(sum, gf_mul(&rs->gf, error_locator[j], x_power));
        }

        if (sum == 0) {
            error_positions[error_count++] = i;
            if (error_count >= L) break;
        }
    }

    return error_count;
}

// Forney算法求错误值
void rs_forney(RSContext *rs, const uint8_t *syndromes,
               const uint8_t *error_locator, int L,
               const uint8_t *error_positions, int error_count,
               uint8_t *error_values) {
    // 计算错误评估多项式
    uint8_t omega[RS_2T + 1];
    memset(omega, 0, sizeof(omega));

    for (int i = 0; i < rs->t * 2; i++) {
        for (int j = 0; j <= L; j++) {
            if (i >= j) {
                omega[i] = gf_add(omega[i],
                                  gf_mul(&rs->gf, error_locator[j], syndromes[i - j]));
            }
        }
    }

    // 计算错误位置多项式的导数
    uint8_t error_locator_deriv[RS_2T];
    for (int i = 0; i < L; i++) {
        if (i % 2 == 0) {
            error_locator_deriv[i] = error_locator[i + 1];
        } else {
            error_locator_deriv[i] = 0;
        }
    }

    // 计算每个错误值
    for (int e = 0; e < error_count; e++) {
        uint8_t pos = error_positions[e];
        uint8_t x = rs->gf.exp[GF_SIZE - 1 - pos];

        // 评估omega(x^(-1))
        uint8_t omega_val = 0;
        uint8_t x_inv_power = 1;
        for (int i = 0; i < rs->t * 2; i++) {
            omega_val = gf_add(omega_val,
                               gf_mul(&rs->gf, omega[i], x_inv_power));
            x_inv_power = gf_mul(&rs->gf, x_inv_power, x);
        }

        // 评估error_locator'(x^(-1))
        uint8_t deriv_val = 0;
        x_inv_power = 1;
        for (int i = 0; i < L; i++) {
            deriv_val = gf_add(deriv_val,
                               gf_mul(&rs->gf, error_locator_deriv[i], x_inv_power));
            x_inv_power = gf_mul(&rs->gf, x_inv_power, x);
        }

        error_values[e] = gf_div(&rs->gf, omega_val, deriv_val);
    }
}

// 完整译码
int rs_decode(RSContext *rs, uint8_t *received, uint8_t *corrected) {
    memcpy(corrected, received, rs->n);

    // 1. 计算综合征
    uint8_t syndromes[RS_2T];
    rs_compute_syndromes(rs, received, syndromes);

    // 检查是否无错误
    bool all_zero = true;
    for (int i = 0; i < rs->t * 2; i++) {
        if (syndromes[i] != 0) {
            all_zero = false;
            break;
        }
    }

    if (all_zero) {
        return 0;  // 无错误
    }

    // 2. Berlekamp-Massey算法
    uint8_t error_locator[RS_2T + 1];
    int L = rs_berlekamp_massey(rs, syndromes, error_locator);

    // 3. Chien搜索
    uint8_t error_positions[RS_2T];
    int error_count = rs_chien_search(rs, error_locator, L, error_positions);

    if (error_count != L) {
        return -1;  // 解码失败，错误数超过纠错能力
    }

    // 4. Forney算法求错误值
    uint8_t error_values[RS_2T];
    rs_forney(rs, syndromes, error_locator, L,
              error_positions, error_count, error_values);

    // 5. 纠正错误
    for (int i = 0; i < error_count; i++) {
        corrected[error_positions[i]] = gf_add(corrected[error_positions[i]],
                                                error_values[i]);
    }

    return error_count;
}
