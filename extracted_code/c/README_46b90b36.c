/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\README.md
 * Line: 454
 * Language: c
 * Block ID: 46b90b36
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 基于量子叠加的真随机数生成
 *
 * 量子随机性源于波函数坍缩的固有不确定性
 */

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

// 使用量子态测量生成随机比特
int quantum_random_bit(void)
{
    // 创建 |+⟩ = H|0⟩ 态
    Qubit q = ZERO;
    apply_gate1(&(QuantumState){1, {q.alpha, q.beta}}, 0, &GATE_H);

    // 测量 |+⟩ 态：50% 概率得到 0 或 1
    // 理论上这是真随机
    return rand() % 2;  // 模拟（实际需量子硬件）
}

// 生成随机字节
uint8_t quantum_random_byte(void)
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte = (byte << 1) | quantum_random_bit();
    }
    return byte;
}

// 使用硬件 QRNG（如果可用）
uint64_t hardware_qrng(void)
{
    // 某些系统提供 /dev/qrng 或类似接口
    int fd = open("/dev/urandom", O_RDONLY);  // 熵池（部分量子）
    if (fd < 0) return 0;

    uint64_t value;
    ssize_t n = read(fd, &value, sizeof(value));
    close(fd);

    return (n == sizeof(value)) ? value : 0;
}

// 均匀分布随机浮点数 [0, 1)
double quantum_random_double(void)
{
    uint64_t bits = hardware_qrng();
    // 52 位尾数精度
    return (bits >> 12) / (double)(1ULL << 52);
}

// 正态分布（Box-Muller 变换）
double quantum_random_normal(void)
{
    static int has_spare = 0;
    static double spare;

    if (has_spare) {
        has_spare = 0;
        return spare;
    }

    double u1 = quantum_random_double();
    double u2 = quantum_random_double();

    // 避免 log(0)
    while (u1 <= 0) u1 = quantum_random_double();

    double mag = sqrt(-2.0 * log(u1));
    double z0 = mag * cos(2 * M_PI * u2);
    double z1 = mag * sin(2 * M_PI * u2);

    spare = z1;
    has_spare = 1;

    return z0;
}
