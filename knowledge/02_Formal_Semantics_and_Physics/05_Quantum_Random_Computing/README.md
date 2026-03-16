# 量子与随机计算

## 概述

量子计算代表了计算范式的根本性转变，利用量子力学原理（叠加、纠缠、干涉）实现经典计算难以企及的计算能力。随机计算则为处理不确定性、概率算法和统计模拟提供了理论基础。本章节探讨量子计算基础、量子随机数生成，以及使用 C 语言进行量子模拟的方法。

---

## 量子计算基础

### 量子比特与态空间

```
经典比特 vs 量子比特：

经典比特: |0⟩ 或 |1⟩                    量子比特: α|0⟩ + β|1⟩
     0 ──┐                                    ┌───┐
         ├── 确定状态                         │ α │ ──┐
     1 ──┘                                    │ β │   ├── 叠加态
                                              └───┘   │
     状态空间: {0, 1}                         |α|² + |β|² = 1

布洛赫球面表示:
                    |0⟩
                     │
                     │  θ
                     │/ φ
    |−⟩ ────────────┼──────────── |+⟩
                     │
                     │
                    |1⟩
```

```c
/*
 * C 语言中的复数与量子态表示 (C99)
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>

// 量子振幅：复数
typedef double complex Amplitude;

// 单量子比特态：|ψ⟩ = α|0⟩ + β|1⟩
typedef struct {
    Amplitude alpha;  // |0⟩ 的振幅
    Amplitude beta;   // |1⟩ 的振幅
} Qubit;

// 计算基态
const Qubit ZERO  = { 1.0, 0.0 };  // |0⟩
const Qubit ONE   = { 0.0, 1.0 };  // |1⟩
const Qubit PLUS  = { 1.0/sqrt(2), 1.0/sqrt(2) };  // |+⟩
const Qubit MINUS = { 1.0/sqrt(2), -1.0/sqrt(2) }; // |−⟩

// 归一化检查
bool is_normalized(const Qubit *q)
{
    double norm = cabs(q->alpha) * cabs(q->alpha) +
                  cabs(q->beta) * cabs(q->beta);
    return fabs(norm - 1.0) < 1e-10;
}

// 归一化
void normalize(Qubit *q)
{
    double norm = sqrt(cabs(q->alpha) * cabs(q->alpha) +
                       cabs(q->beta) * cabs(q->beta));
    q->alpha /= norm;
    q->beta /= norm;
}

// 打印量子态
void print_qubit(const Qubit *q)
{
    printf("|ψ⟩ = (%.4f %+.4fi)|0⟩ + (%.4f %+.4fi)|1⟩\n",
           creal(q->alpha), cimag(q->alpha),
           creal(q->beta), cimag(q->beta));
    printf("    P(|0⟩) = %.4f, P(|1⟩) = %.4f\n",
           cabs(q->alpha) * cabs(q->alpha),
           cabs(q->beta) * cabs(q->beta));
}

// 布洛赫球面坐标
void bloch_coordinates(const Qubit *q, double *theta, double *phi)
{
    // |ψ⟩ = cos(θ/2)|0⟩ + e^(iφ)sin(θ/2)|1⟩
    *theta = 2 * acos(cabs(q->alpha));
    *phi = carg(q->beta) - carg(q->alpha);
}
```

### 量子门操作

```c
/*
 * 量子门：酉变换矩阵
 */

#include <string.h>

#define N_QUBITS 20
#define STATE_SIZE (1 << N_QUBITS)  // 2^n 状态空间

// 多量子比特态：振幅向量
typedef struct {
    int n_qubits;
    Amplitude amplitudes[STATE_SIZE];
} QuantumState;

// 单量子比特门：2x2 矩阵
typedef struct {
    Amplitude m[2][2];
} Gate1;

// 常用单量子比特门
const Gate1 GATE_I = {{{1, 0}, {0, 1}}};  // 恒等
const Gate1 GATE_X = {{{0, 1}, {1, 0}}};  // Pauli-X (NOT)
const Gate1 GATE_Z = {{{1, 0}, {0, -1}}}; // Pauli-Z
const Gate1 GATE_H = {                     // Hadamard
    {{1/sqrt(2), 1/sqrt(2)},
     {1/sqrt(2), -1/sqrt(2)}}
};

// 构造旋转门
Gate1 rotation_x(double theta)
{
    Gate1 rx = {{
        {cos(theta/2), -I*sin(theta/2)},
        {-I*sin(theta/2), cos(theta/2)}
    }};
    return rx;
}

Gate1 rotation_y(double theta)
{
    Gate1 ry = {{
        {cos(theta/2), -sin(theta/2)},
        {sin(theta/2), cos(theta/2)}
    }};
    return ry;
}

// 应用单量子比特门到特定量子比特
void apply_gate1(QuantumState *state, int target, const Gate1 *gate)
{
    int n = state->n_qubits;
    int stride = 1 << target;

    // 对所有基态应用门
    for (int i = 0; i < (1 << n); i += 2 * stride) {
        for (int j = 0; j < stride; j++) {
            int idx0 = i + j;
            int idx1 = idx0 + stride;

            Amplitude a0 = state->amplitudes[idx0];
            Amplitude a1 = state->amplitudes[idx1];

            state->amplitudes[idx0] = gate->m[0][0] * a0 + gate->m[0][1] * a1;
            state->amplitudes[idx1] = gate->m[1][0] * a0 + gate->m[1][1] * a1;
        }
    }
}

// CNOT 门（双量子比特）
void apply_cnot(QuantumState *state, int control, int target)
{
    int n = state->n_qubits;
    int c_stride = 1 << control;
    int t_stride = 1 << target;

    for (int i = 0; i < (1 << n); i++) {
        // 检查控制位是否为 1
        if ((i & c_stride) && !(i & t_stride)) {
            int flipped = i | t_stride;
            // 交换振幅
            Amplitude tmp = state->amplitudes[i];
            state->amplitudes[i] = state->amplitudes[flipped];
            state->amplitudes[flipped] = tmp;
        }
    }
}
```

### 量子测量

```c
/*
 * 量子测量：波函数坍缩
 */

#include <stdlib.h>
#include <time.h>

// 初始化随机数生成
void init_random(void)
{
    srand((unsigned)time(NULL));
}

// 测量单量子比特，返回 0 或 1
int measure_qubit(const QuantumState *state, int target)
{
    double prob0 = 0.0;
    int stride = 1 << target;

    // 计算测量结果为 0 的概率
    for (int i = 0; i < (1 << state->n_qubits); i++) {
        if (!(i & stride)) {
            prob0 += cabs(state->amplitudes[i]) *
                     cabs(state->amplitudes[i]);
        }
    }

    // 随机决定是否坍缩到 0
    double r = (double)rand() / RAND_MAX;
    return (r < prob0) ? 0 : 1;
}

// 测量并坍缩态
int measure_and_collapse(QuantumState *state, int target)
{
    double prob0 = 0.0;
    int stride = 1 << target;
    int n = state->n_qubits;

    // 计算概率
    for (int i = 0; i < (1 << n); i++) {
        if (!(i & stride)) {
            prob0 += cabs(state->amplitudes[i]) *
                     cabs(state->amplitudes[i]);
        }
    }

    double r = (double)rand() / RAND_MAX;
    int result = (r < prob0) ? 0 : 1;

    // 坍缩：将与结果不一致的振幅置零
    for (int i = 0; i < (1 << n); i++) {
        int bit = (i & stride) ? 1 : 0;
        if (bit != result) {
            state->amplitudes[i] = 0;
        }
    }

    // 重新归一化
    double norm = 0.0;
    for (int i = 0; i < (1 << n); i++) {
        norm += cabs(state->amplitudes[i]) * cabs(state->amplitudes[i]);
    }
    norm = sqrt(norm);
    for (int i = 0; i < (1 << n); i++) {
        state->amplitudes[i] /= norm;
    }

    return result;
}

// 期望值测量
double expectation_value(const QuantumState *state,
                         const Amplitude operator_matrix[2][2],
                         int target)
{
    // 计算 ⟨ψ|O|ψ⟩
    QuantumState temp = *state;
    apply_gate1(&temp, target, &(Gate1){{
        {operator_matrix[0][0], operator_matrix[0][1]},
        {operator_matrix[1][0], operator_matrix[1][1]}
    }});

    double expectation = 0.0;
    for (int i = 0; i < (1 << state->n_qubits); i++) {
        expectation += creal(conj(state->amplitudes[i]) *
                            temp.amplitudes[i]);
    }
    return expectation;
}
```

---

## 量子算法简介

### Deutsch-Jozsa 算法

```c
/*
 * Deutsch-Jozsa 算法：判断函数是常数还是平衡
 *
 * 经典：最坏需要 2^(n-1)+1 次查询
 * 量子：仅需 1 次查询！
 */

void deutsch_jozsa(QuantumState *state, int n)
{
    // 初始化：|0⟩^n ⊗ |1⟩
    memset(state->amplitudes, 0, sizeof(state->amplitudes));
    state->n_qubits = n + 1;
    state->amplitudes[1] = 1.0;  // |00...01⟩

    // 对所有量子比特应用 H
    for (int i = 0; i <= n; i++) {
        apply_gate1(state, i, &GATE_H);
    }

    // 应用黑盒函数 U_f（这里用示例）
    // 假设 f 是常数函数，不做任何操作
    // 或 f(x) = x_0 XOR x_1 XOR ...，需要相应地应用门

    // 对前 n 个量子比特应用 H
    for (int i = 0; i < n; i++) {
        apply_gate1(state, i, &GATE_H);
    }

    // 测量前 n 个量子比特
    // 如果都是 0，f 是常数；否则是平衡函数
    int is_constant = 1;
    for (int i = 0; i < n; i++) {
        if (measure_qubit(state, i) != 0) {
            is_constant = 0;
            break;
        }
    }

    printf("Function is %s\n", is_constant ? "constant" : "balanced");
}
```

### Grover 搜索算法

```c
/*
 * Grover 搜索：在未排序数据库中搜索
 * 经典：O(N) 次查询
 * 量子：O(√N) 次查询
 */

#include <math.h>

void grover_diffusion(QuantumState *state, int n)
{
    // 扩散算子：2|ψ⟩⟨ψ| - I

    // 对所有量子比特应用 H
    for (int i = 0; i < n; i++) {
        apply_gate1(state, i, &GATE_H);
    }

    // 条件相位移动
    for (int i = 0; i < (1 << n); i++) {
        if (i != 0) {
            state->amplitudes[i] *= -1;
        }
    }

    // 再次应用 H
    for (int i = 0; i < n; i++) {
        apply_gate1(state, i, &GATE_H);
    }
}

void grover_oracle(QuantumState *state, int n, int target)
{
    // 标记目标态：相位翻转
    state->amplitudes[target] *= -1;
}

int grover_search(int n, int (*oracle)(int))
{
    QuantumState state;
    state.n_qubits = n;

    // 初始化均匀叠加态
    double amp = 1.0 / sqrt(1 << n);
    for (int i = 0; i < (1 << n); i++) {
        state.amplitudes[i] = amp;
    }

    // 找到目标
    int target = -1;
    for (int i = 0; i < (1 << n); i++) {
        if (oracle(i)) {
            target = i;
            break;
        }
    }

    if (target < 0) return -1;  // 目标不存在

    // 迭代次数：π/4 * √N
    int iterations = (int)(M_PI / 4 * sqrt(1 << n));

    for (int iter = 0; iter < iterations; iter++) {
        grover_oracle(&state, n, target);
        grover_diffusion(&state, n);
    }

    // 测量
    double max_prob = 0;
    int result = 0;
    for (int i = 0; i < (1 << n); i++) {
        double prob = cabs(state.amplitudes[i]) *
                      cabs(state.amplitudes[i]);
        if (prob > max_prob) {
            max_prob = prob;
            result = i;
        }
    }

    return result;
}

// 示例：搜索函数 f(x) = (x == 42)
int example_oracle(int x)
{
    return x == 42;
}
```

---

## 量子随机数生成

### 量子随机数发生器 (QRNG)

```c
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
```

### 随机性提取与验证

```c
/*
 * 随机性质量检验
 */

#include <string.h>

// 统计比特频率
void bit_frequency_test(const uint8_t *data, size_t len,
                         double *freq_0, double *freq_1)
{
    int count_0 = 0, count_1 = 0;
    for (size_t i = 0; i < len; i++) {
        for (int b = 0; b < 8; b++) {
            if ((data[i] >> b) & 1) {
                count_1++;
            } else {
                count_0++;
            }
        }
    }
    int total = count_0 + count_1;
    *freq_0 = (double)count_0 / total;
    *freq_1 = (double)count_1 / total;
}

// 游程检验
int runs_test(const uint8_t *data, size_t len)
{
    int runs = 1;
    int prev_bit = (data[0] & 1);

    for (size_t i = 0; i < len; i++) {
        for (int b = (i == 0) ? 1 : 0; b < 8; b++) {
            int bit = (data[i] >> b) & 1;
            if (bit != prev_bit) {
                runs++;
                prev_bit = bit;
            }
        }
    }

    return runs;
}
```

---

## C 语言量子模拟

### 量子模拟器框架

```c
/*
 * 简单量子模拟器核心
 */

#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUBITS 20

typedef struct {
    int num_qubits;
    double complex amplitudes[1 << MAX_QUBITS];
} QSimulator;

// 初始化模拟器
void qs_init(QSimulator *qs, int n)
{
    qs->num_qubits = n;
    memset(qs->amplitudes, 0, sizeof(qs->amplitudes));
    qs->amplitudes[0] = 1.0;  // |0...0⟩
}

// 获取状态大小
static inline int qs_state_size(const QSimulator *qs)
{
    return 1 << qs->num_qubits;
}

// 应用单量子比特门
void qs_apply_u(QSimulator *qs, int target,
                double complex u00, double complex u01,
                double complex u10, double complex u11)
{
    int n = qs->num_qubits;
    int mask = 1 << target;

    for (int i = 0; i < (1 << n); i++) {
        if (!(i & mask)) {
            int j = i | mask;
            double complex ai = qs->amplitudes[i];
            double complex aj = qs->amplitudes[j];

            qs->amplitudes[i] = u00 * ai + u01 * aj;
            qs->amplitudes[j] = u10 * ai + u11 * aj;
        }
    }
}

// 预定义门
void qs_hadamard(QSimulator *qs, int target)
{
    double s = 1.0 / sqrt(2.0);
    qs_apply_u(qs, target, s, s, s, -s);
}

void qs_pauli_x(QSimulator *qs, int target)
{
    qs_apply_u(qs, target, 0, 1, 1, 0);
}

void qs_pauli_z(QSimulator *qs, int target)
{
    qs_apply_u(qs, target, 1, 0, 0, -1);
}

void qs_phase(QSimulator *qs, int target, double theta)
{
    qs_apply_u(qs, target, 1, 0, 0, cexp(I * theta));
}

// CNOT 门
void qs_cnot(QSimulator *qs, int control, int target)
{
    int n = qs->num_qubits;
    int c_mask = 1 << control;
    int t_mask = 1 << target;

    for (int i = 0; i < (1 << n); i++) {
        if ((i & c_mask) && !(i & t_mask)) {
            int j = i | t_mask;
            double complex tmp = qs->amplitudes[i];
            qs->amplitudes[i] = qs->amplitudes[j];
            qs->amplitudes[j] = tmp;
        }
    }
}

// Toffoli 门 (CCNOT)
void qs_toffoli(QSimulator *qs, int c1, int c2, int target)
{
    int n = qs->num_qubits;
    int m1 = 1 << c1;
    int m2 = 1 << c2;
    int mt = 1 << target;

    for (int i = 0; i < (1 << n); i++) {
        if ((i & m1) && (i & m2) && !(i & mt)) {
            int j = i | mt;
            double complex tmp = qs->amplitudes[i];
            qs->amplitudes[i] = qs->amplitudes[j];
            qs->amplitudes[j] = tmp;
        }
    }
}

// SWAP 门
void qs_swap(QSimulator *qs, int q1, int q2)
{
    qs_cnot(qs, q1, q2);
    qs_cnot(qs, q2, q1);
    qs_cnot(qs, q1, q2);
}

// 测量
int qs_measure(QSimulator *qs, int target)
{
    int n = qs->num_qubits;
    int mask = 1 << target;

    double prob0 = 0.0;
    for (int i = 0; i < (1 << n); i++) {
        if (!(i & mask)) {
            prob0 += cabs(qs->amplitudes[i]) * cabs(qs->amplitudes[i]);
        }
    }

    int result = ((double)rand() / RAND_MAX) < prob0 ? 0 : 1;

    // 坍缩
    for (int i = 0; i < (1 << n); i++) {
        int bit = (i & mask) ? 1 : 0;
        if (bit != result) {
            qs->amplitudes[i] = 0;
        }
    }

    // 归一化
    double norm = 0.0;
    for (int i = 0; i < (1 << n); i++) {
        norm += cabs(qs->amplitudes[i]) * cabs(qs->amplitudes[i]);
    }
    norm = sqrt(norm);
    for (int i = 0; i < (1 << n); i++) {
        qs->amplitudes[i] /= norm;
    }

    return result;
}

// 打印状态（用于调试）
void qs_print_state(const QSimulator *qs)
{
    printf("Quantum state:\n");
    for (int i = 0; i < (1 << qs->num_qubits); i++) {
        double prob = cabs(qs->amplitudes[i]) * cabs(qs->amplitudes[i]);
        if (prob > 1e-10) {
            printf("|%d⟩: %.4f %+.4fi (prob: %.4f)\n",
                   i, creal(qs->amplitudes[i]),
                   cimag(qs->amplitudes[i]), prob);
        }
    }
}
```

### Bell 态制备与验证

```c
/*
 * Bell 态 |Φ+⟩ = (|00⟩ + |11⟩) / √2
 */

void bell_state_demo(void)
{
    QSimulator qs;
    qs_init(&qs, 2);

    printf("Initial state: |00⟩\n");

    // 对第一个量子比特应用 H
    qs_hadamard(&qs, 0);
    printf("After H on q0:\n");
    qs_print_state(&qs);

    // CNOT(0, 1)
    qs_cnot(&qs, 0, 1);
    printf("After CNOT(0, 1):\n");
    qs_print_state(&qs);

    // 多次测量验证纠缠
    printf("\nMeasurement statistics:\n");
    int counts[4] = {0};

    for (int run = 0; run < 1000; run++) {
        QSimulator qscopy = qs;
        int m0 = qs_measure(&qscopy, 0);
        int m1 = qs_measure(&qscopy, 1);
        counts[m0 * 2 + m1]++;
    }

    printf("|00⟩: %d (expected ~50%%)\n", counts[0]);
    printf("|01⟩: %d (expected ~0%%)\n", counts[1]);
    printf("|10⟩: %d (expected ~0%%)\n", counts[2]);
    printf("|11⟩: %d (expected ~50%%)\n", counts[3]);
}
```

---

## 延伸阅读

- [量子计算与信息](https://en.wikipedia.org/wiki/Quantum_computing)
- [Qiskit 开源量子框架](https://qiskit.org/)
- Nielsen & Chuang, *Quantum Computation and Quantum Information*
- [NIST 量子随机数生成器](https://www.nist.gov/programs-projects/quantum-random-number-generation)
