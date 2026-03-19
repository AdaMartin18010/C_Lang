/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 2108
 * Language: c
 * Block ID: d67ab8ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * PQC硬件加速器集成
 */

/* 加速器接口 */
typedef struct {
    int (*init)(void);
    int (*ntt)(const int16_t *in, int16_t *out, size_t n);
    int (*intt)(const int16_t *in, int16_t *out, size_t n);
    int (*poly_mul)(const int16_t *a, const int16_t *b, int16_t *r);
    int (*hash)(const uint8_t *in, size_t len, uint8_t *out);
    int (*rng)(uint8_t *out, size_t len);
} pqc_accelerator_t;

/* 检查加速器可用性 */
bool is_pqc_accelerator_available(void) {
    /* 检查CPUID或设备树 */
    return check_accelerator_id() == PQC_ACCEL_ID;
}

/* 使用加速器的ML-KEM */
int ml_kem_keygen_accel(ml_kem_keypair_t *kp, pqc_accelerator_t *accel) {
    /* 使用加速器进行NTT运算 */
    /* 密钥生成中的主要计算：
     * 1. 扩展矩阵A（使用加速器哈希）
     * 2. NTT转换（使用加速器NTT）
     * 3. 多项式乘法（使用加速器）
     */

    /* 生成随机种子 */
    uint8_t d[32], z[32];
    accel->rng(d, 32);
    accel->rng(z, 32);

    /* 扩展种子 */
    uint8_t expanded[64];
    accel->hash(d, 32, expanded);

    /* 使用加速器进行NTT和多项式运算 */
    /* ... */

    return 0;
}

/* FPGA加速器驱动 */
#ifdef HAS_FPGA_ACCEL

#define FPGA_PQC_BASE   0x40000000
#define FPGA_PQC_NTT    (FPGA_PQC_BASE + 0x00)
#define FPGA_PQC_INTT   (FPGA_PQC_BASE + 0x04)
#define FPGA_PQC_POLY   (FPGA_PQC_BASE + 0x08)
#define FPGA_PQC_STATUS (FPGA_PQC_BASE + 0x0C)

void fpga_ntt(int16_t *data) {
    volatile uint32_t *ctrl = (uint32_t *)FPGA_PQC_NTT;
    volatile int16_t *dma = (int16_t *)0x40010000;

    /* 写入数据 */
    for (int i = 0; i < 256; i++) {
        dma[i] = data[i];
    }

    /* 启动运算 */
    *ctrl = 1;

    /* 等待完成 */
    while (*(volatile uint32_t *)FPGA_PQC_STATUS & 0x01);

    /* 读取结果 */
    for (int i = 0; i < 256; i++) {
        data[i] = dma[i];
    }
}

#endif /* HAS_FPGA_ACCEL */
