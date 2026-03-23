# 量子计算接口 (Quantum Computing Interfaces)

## 目录

- [量子计算接口 (Quantum Computing Interfaces)](#量子计算接口-quantum-computing-interfaces)
  - [目录](#目录)
  - [概述](#概述)
    - [量子计算 vs 经典计算](#量子计算-vs-经典计算)
    - [量子计算应用领域](#量子计算应用领域)
  - [量子比特基础](#量子比特基础)
    - [什么是量子比特](#什么是量子比特)
    - [布洛赫球表示](#布洛赫球表示)
    - [C语言表示量子比特](#c语言表示量子比特)
  - [量子门操作](#量子门操作)
    - [单量子比特门](#单量子比特门)
    - [量子门电路图示例](#量子门电路图示例)
    - [C语言实现量子门](#c语言实现量子门)
  - [IBM Qiskit C 接口](#ibm-qiskit-c-接口)
    - [Qiskit Runtime C API 架构](#qiskit-runtime-c-api-架构)
    - [C语言调用Qiskit示例](#c语言调用qiskit示例)
  - [量子随机数生成](#量子随机数生成)
    - [量子随机数 vs 伪随机数](#量子随机数-vs-伪随机数)
    - [量子随机数生成原理](#量子随机数生成原理)
    - [C语言实现QRNG](#c语言实现qrng)
  - [后量子密码学](#后量子密码学)
    - [后量子密码学概述](#后量子密码学概述)
    - [后量子密码算法类别](#后量子密码算法类别)
    - [Kyber KEM 原理与C实现](#kyber-kem-原理与c实现)
  - [实践案例](#实践案例)
    - [案例1：量子密钥分发模拟](#案例1量子密钥分发模拟)
  - [参考资源](#参考资源)
    - [量子计算框架](#量子计算框架)
    - [后量子密码标准](#后量子密码标准)
    - [学习资源](#学习资源)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 概述

量子计算是一种利用量子力学现象（如叠加和纠缠）进行计算的新型计算范式。
与传统计算机使用比特（bit）不同，量子计算机使用量子比特（qubit）作为基本信息单位。

### 量子计算 vs 经典计算

```text
┌─────────────────────────────────────────────────────────────────┐
│                    计算范式对比                                  │
├─────────────────────────────────────────────────────────────────┤
│  特性              │  经典计算           │  量子计算              │
├─────────────────────────────────────────────────────────────────┤
│  基本信息单位      │  比特 (0 或 1)      │  量子比特 (叠加态)     │
│  状态空间          │  线性增长           │  指数增长              │
│  并行性            │  有限               │  本质并行              │
│  适用问题          │  日常计算           │  特定复杂问题          │
│  错误率            │  极低               │  较高（需纠错）        │
└─────────────────────────────────────────────────────────────────┘
```

### 量子计算应用领域

- **密码学**：Shor算法破解RSA、后量子密码设计
- **优化问题**：旅行商问题、组合优化
- **量子模拟**：分子模拟、材料科学
- **机器学习**：量子神经网络、量子支持向量机
- **随机数生成**：真随机数生成器

---

## 量子比特基础

### 什么是量子比特

量子比特（Qubit）是量子计算的基本信息单位，与经典比特不同，它可以同时处于 |0⟩ 和 |1⟩ 的叠加态：

```text
|ψ⟩ = α|0⟩ + β|1⟩

其中：
- α, β 是复数振幅
- |α|² + |β|² = 1 （概率归一化）
- |α|² 表示测量得到 |0⟩ 的概率
- |β|² 表示测量得到 |1⟩ 的概率
```

### 布洛赫球表示

```text
                    |0⟩
                     ↑
                    /|\
                   / | \
                  /  |  \
                 /   |   \
                /    |    \
         |+⟩ ←------+------→ |-⟩
              (x轴)  |
                    /|\
                   / | \
                  /  |  \
                 ↓   |   ↓
                    |1⟩

布洛赫球：量子比特状态的球面几何表示
- 北极：|0⟩ 状态
- 南极：|1⟩ 状态
- 赤道：等概率叠加态
- 任意点：由 θ 和 φ 确定的量子态
```

### C语言表示量子比特

```c
/**
 * @file qubit.c
 * @brief 量子比特基础实现
 */

#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>

/* 定义复数类型 */
typedef double complex Complex;

/* 量子比特结构体 */
typedef struct {
    Complex alpha;  /* |0⟩ 的振幅 */
    Complex beta;   /* |1⟩ 的振幅 */
} Qubit;

/* 量子态向量（多量子比特） */
typedef struct {
    int n_qubits;       /* 量子比特数量 */
    int dim;            /* 状态空间维度 = 2^n */
    Complex *amplitudes; /* 振幅数组 */
} QuantumState;

/**
 * @brief 初始化量子比特到 |0⟩ 状态
 */
void qubit_init(Qubit *q) {
    q->alpha = 1.0 + 0.0*I;
    q->beta = 0.0 + 0.0*I;
}

/**
 * @brief 初始化量子比特到 |1⟩ 状态
 */
void qubit_init_one(Qubit *q) {
    q->alpha = 0.0 + 0.0*I;
    q->beta = 1.0 + 0.0*I;
}

/**
 * @brief 创建叠加态 |ψ⟩ = α|0⟩ + β|1⟩
 */
void qubit_set_superposition(Qubit *q, Complex alpha, Complex beta) {
    /* 归一化 */
    double norm = cabs(alpha) * cabs(alpha) + cabs(beta) * cabs(beta);
    norm = sqrt(norm);
    q->alpha = alpha / norm;
    q->beta = beta / norm;
}

/**
 * @brief 测量量子比特，返回 0 或 1
 */
int qubit_measure(Qubit *q) {
    double prob0 = cabs(q->alpha) * cabs(q->alpha);
    double random = (double)rand() / RAND_MAX;

    /* 测量后量子态坍缩 */
    if (random < prob0) {
        q->alpha = 1.0 + 0.0*I;
        q->beta = 0.0 + 0.0*I;
        return 0;
    } else {
        q->alpha = 0.0 + 0.0*I;
        q->beta = 1.0 + 0.0*I;
        return 1;
    }
}

/**
 * @brief 打印量子比特状态
 */
void qubit_print(const Qubit *q, const char *label) {
    printf("%s: |ψ⟩ = ", label);
    printf("(%.3f%+.3fi)|0⟩ + ", creal(q->alpha), cimag(q->alpha));
    printf("(%.3f%+.3fi)|1⟩\n", creal(q->beta), cimag(q->beta));
    printf("       P(|0⟩) = %.3f, P(|1⟩) = %.3f\n",
           cabs(q->alpha)*cabs(q->alpha),
           cabs(q->beta)*cabs(q->beta));
}

/* 使用示例 */
int main() {
    srand(42);

    Qubit q;

    /* 初始化到 |0⟩ */
    qubit_init(&q);
    qubit_print(&q, "初始状态");

    /* 创建叠加态 |+⟩ = (|0⟩ + |1⟩)/√2 */
    qubit_set_superposition(&q, 1.0+0.0*I, 1.0+0.0*I);
    qubit_print(&q, "叠加态 |+⟩");

    /* 测量 */
    int result = qubit_measure(&q);
    printf("测量结果: %d\n", result);
    qubit_print(&q, "测量后状态");

    return 0;
}
```

---

## 量子门操作

### 单量子比特门

```text
┌─────────────────────────────────────────────────────────────┐
│                    单量子比特门                              │
├──────────┬────────────────────────────────┬─────────────────┤
│   门     │           矩阵表示              │     作用        │
├──────────┼────────────────────────────────┼─────────────────┤
│   I      │  [1  0]                        │   恒等操作       │
│          │  [0  1]                        │                 │
├──────────┼────────────────────────────────┼─────────────────┤
│   X      │  [0  1]                        │   非门/比特翻转  │
│          │  [1  0]                        │   |0⟩↔|1⟩       │
├──────────┼────────────────────────────────┼─────────────────┤
│   Y      │  [0  -i]                       │   Y轴旋转π      │
│          │  [i   0]                       │                 │
├──────────┼────────────────────────────────┼─────────────────┤
│   Z      │  [1   0]                       │   相位翻转       │
│          │  [0  -1]                       │   |1⟩→-|1⟩       │
├──────────┼────────────────────────────────┼─────────────────┤
│   H      │  [1/√2  1/√2]                  │   Hadamard门    │
│          │  [1/√2 -1/√2]                  │   创建叠加态     │
├──────────┼────────────────────────────────┼─────────────────┤
│   S      │  [1  0]                        │   π/2相位门      │
│          │  [0  i]                        │                 │
├──────────┼────────────────────────────────┼─────────────────┤
│   T      │  [1     0    ]                 │   π/4相位门      │
│          │  [0  e^(iπ/4)]                 │                 │
└──────────┴────────────────────────────────┴─────────────────┘
```

### 量子门电路图示例

```text
    ┌───┐     ┌───┐     ┌───┐
 ───│ H ├──■──│ X ├──■──│ H ├───
    └───┘  │  └───┘  │  └───┘
           │         │
    ───────┼─────────┼─────────
           │         │
         ┌─┴─┐     ┌─┴─┐
    ─────┤ Z ├─────┤ X ├───────
         └───┘     └───┘

         CNOT      CNOT
上: 控制量子比特
下: 目标量子比特
```

### C语言实现量子门

```c
/**
 * @file quantum_gates.c
 * @brief 量子门操作实现
 */

#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <string.h>

#define SQRT2 1.4142135623730951
typedef double complex Complex;

/* 2x2 复数矩阵表示单量子比特门 */
typedef struct {
    Complex m[2][2];
} Gate2x2;

/* 定义常用量子门 */
const Gate2x2 GATE_I = {{{1, 0}, {0, 1}}};
const Gate2x2 GATE_X = {{{0, 1}, {1, 0}}};
const Gate2x2 GATE_Z = {{{1, 0}, {0, -1}}};
const Gate2x2 GATE_H = {{{1/SQRT2, 1/SQRT2}, {1/SQRT2, -1/SQRT2}}};
const Gate2x2 GATE_S = {{{1, 0}, {0, 1.0*I}}};

/* 量子态 */
typedef struct {
    Complex a0;  /* |0⟩ 振幅 */
    Complex a1;  /* |1⟩ 振幅 */
} QState;

/**
 * @brief 应用量子门到量子态
 */
void apply_gate(QState *state, const Gate2x2 *gate) {
    Complex new_a0 = gate->m[0][0] * state->a0 + gate->m[0][1] * state->a1;
    Complex new_a1 = gate->m[1][0] * state->a0 + gate->m[1][1] * state->a1;
    state->a0 = new_a0;
    state->a1 = new_a1;
}

/**
 * @brief 打印量子态
 */
void print_state(const QState *s, const char *label) {
    printf("%s:\n", label);
    printf("  |ψ⟩ = (%.4f %+.4fi)|0⟩ + (%.4f %+.4fi)|1⟩\n",
           creal(s->a0), cimag(s->a0), creal(s->a1), cimag(s->a1));
    printf("  |振幅|: |0⟩=%.4f, |1⟩=%.4f\n",
           cabs(s->a0), cabs(s->a1));
}

/**
 * @brief CNOT门（受控非门）- 双量子比特操作
 * @param control 控制量子比特（0或1）
 * @param target 目标量子比特状态
 */
void apply_cnot(int control, QState *target) {
    if (control == 1) {
        /* 当控制位为|1⟩时，目标位翻转 */
        Complex temp = target->a0;
        target->a0 = target->a1;
        target->a1 = temp;
    }
}

/* 量子门操作演示 */
int main() {
    QState q = {1.0+0.0*I, 0.0+0.0*I};  /* |0⟩ */

    printf("=== 量子门操作演示 ===\n\n");

    /* 初始状态 |0⟩ */
    print_state(&q, "初始 |0⟩");

    /* 应用Hadamard门创建叠加态 */
    printf("\n→ 应用 H 门\n");
    apply_gate(&q, &GATE_H);
    print_state(&q, "叠加态 |+⟩");

    /* 应用X门 */
    printf("\n→ 应用 X 门\n");
    apply_gate(&q, &GATE_X);
    print_state(&q, "变换后");

    /* 应用Z门 */
    printf("\n→ 应用 Z 门\n");
    apply_gate(&q, &GATE_Z);
    print_state(&q, "相位翻转后");

    /* 再次应用H门 */
    printf("\n→ 应用 H 门\n");
    apply_gate(&q, &GATE_H);
    print_state(&q, "最终状态");

    return 0;
}
```

---

## IBM Qiskit C 接口

### Qiskit Runtime C API 架构

```text
┌───────────────────────────────────────────────────────────────┐
│                     应用层 (Application)                       │
├───────────────────────────────────────────────────────────────┤
│              Python Qiskit SDK / C++ 绑定                      │
├───────────────────────────────────────────────────────────────┤
│                    C API 接口层                                │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────────────────┐  │
│  │ Circuit API │ │ Runtime API │ │  Provider API           │  │
│  │ 量子电路    │ │ 量子运行时  │ │  后端管理               │  │
│  └─────────────┘ └─────────────┘ └─────────────────────────┘  │
├───────────────────────────────────────────────────────────────┤
│                   量子后端 (Quantum Backend)                   │
│         IBM Quantum 硬件 / 模拟器 (Aer Simulator)              │
└───────────────────────────────────────────────────────────────┘
```

### C语言调用Qiskit示例

```c
/**
 * @file qiskit_c_interface.c
 * @brief IBM Qiskit C接口示例
 *
 * 注意：此示例展示概念性接口设计
 * 实际使用需要链接Qiskit C++库的C包装器
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>

/* 模拟Qiskit C API类型定义 */

typedef struct QiskitCircuit* CircuitHandle;
typedef struct QiskitBackend* BackendHandle;
typedef struct QiskitJob* JobHandle;
typedef struct QiskitResult* ResultHandle;

/* 错误码 */
typedef enum {
    QISKIT_OK = 0,
    QISKIT_ERROR_INVALID_PARAM = -1,
    QISKIT_ERROR_BACKEND_UNAVAILABLE = -2,
    QISKIT_ERROR_EXECUTION_FAILED = -3,
    QISKIT_ERROR_NO_CREDENTIALS = -4
} QiskitError;

/* 测量结果结构 */
typedef struct {
    char *bitstring;    /* 如 "00101" */
    int count;          /* 出现次数 */
    double probability; /* 概率 */
} MeasurementOutcome;

typedef struct {
    MeasurementOutcome *outcomes;
    int num_outcomes;
    double execution_time_ms;
} JobResult;

/* ============================================
 * 模拟Qiskit C API函数声明
 * ============================================ */

/**
 * @brief 初始化Qiskit运行时
 * @param api_token IBM Quantum API令牌
 * @return 错误码
 */
int qiskit_initialize(const char *api_token);

/**
 * @brief 创建量子电路
 * @param num_qubits 量子比特数量
 * @param num_clbits 经典比特数量
 * @return 电路句柄
 */
CircuitHandle qiskit_circuit_create(int num_qubits, int num_clbits);

/**
 * @brief 添加Hadamard门
 */
int qiskit_circuit_h(CircuitHandle circ, int qubit);

/**
 * @brief 添加CNOT门
 */
int qiskit_circuit_cx(CircuitHandle circ, int control, int target);

/**
 * @brief 添加测量操作
 */
int qiskit_circuit_measure(CircuitHandle circ, int qubit, int cbit);

/**
 * @brief 获取模拟器后端
 */
BackendHandle qiskit_get_aer_simulator(void);

/**
 * @brief 执行电路
 * @param circ 量子电路
 * @param backend 执行后端
 * @param shots 运行次数
 * @return 作业句柄
 */
JobHandle qiskit_execute(CircuitHandle circ, BackendHandle backend, int shots);

/**
 * @brief 等待作业完成并获取结果
 */
JobResult* qiskit_job_result(JobHandle job);

/**
 * @brief 释放结果
 */
void qiskit_result_free(JobResult *result);

/**
 * @brief 关闭Qiskit运行时
 */
void qiskit_finalize(void);

/* ============================================
 * 实际使用示例：贝尔态制备
 * ============================================ */

/**
 * @brief 创建贝尔态 |Φ+⟩ = (|00⟩ + |11⟩)/√2
 */
void create_bell_state_example(void) {
    printf("=== 贝尔态制备示例 ===\n\n");

    /* 1. 创建2量子比特电路 */
    CircuitHandle circ = qiskit_circuit_create(2, 2);
    if (!circ) {
        fprintf(stderr, "错误：无法创建电路\n");
        return;
    }
    printf("1. 创建2量子比特电路\n");

    /* 2. 在量子比特0上应用Hadamard门 */
    qiskit_circuit_h(circ, 0);
    printf("2. 应用 H 门到量子比特0\n");

    /* 3. 应用CNOT门 (控制=0, 目标=1) */
    qiskit_circuit_cx(circ, 0, 1);
    printf("3. 应用 CNOT(0→1) 门\n");

    /* 4. 测量两个量子比特 */
    qiskit_circuit_measure(circ, 0, 0);
    qiskit_circuit_measure(circ, 1, 1);
    printf("4. 添加测量操作\n");

    /* 5. 获取模拟器后端 */
    BackendHandle backend = qiskit_get_aer_simulator();
    printf("5. 获取Aer模拟器后端\n");

    /* 6. 执行电路，运行1024次 */
    printf("6. 执行电路 (shots=1024)...\n");
    JobHandle job = qiskit_execute(circ, backend, 1024);

    /* 7. 获取结果 */
    JobResult *result = qiskit_job_result(job);
    if (result) {
        printf("\n=== 测量结果 ===\n");
        printf("总共 %d 种结果\n\n", result->num_outcomes);

        for (int i = 0; i < result->num_outcomes; i++) {
            printf("  状态 |%s⟩: 次数=%d, 概率=%.2f%%\n",
                   result->outcomes[i].bitstring,
                   result->outcomes[i].count,
                   result->outcomes[i].probability * 100);
        }

        printf("\n执行时间: %.2f ms\n", result->execution_time_ms);
        qiskit_result_free(result);
    }
}

/**
 * @brief 量子傅里叶变换(QFT)示例
 */
void qft_example(void) {
    printf("\n\n=== 3量子比特QFT示例 ===\n\n");

    int n = 3;
    CircuitHandle circ = qiskit_circuit_create(n, n);

    printf("创建 %d 量子比特QFT电路\n\n", n);
    printf("QFT电路结构:\n");
    printf("  量子比特0: H → S(受控) → T(受控) → ...\n");
    printf("  量子比特1: H → S(受控) → ...\n");
    printf("  量子比特2: H → ...\n");
    printf("  最后: 交换量子比特顺序\n\n");

    /* QFT实现代码... */
    printf("(实际实现需要添加受控相位门和交换门)\n");
}

/* 主函数 */
int main(void) {
    /* 初始化（实际使用时需要提供有效的API令牌） */
    printf("初始化Qiskit C接口...\n");

    /* 运行示例 */
    create_bell_state_example();
    qft_example();

    /* 清理 */
    qiskit_finalize();
    printf("\nQiskit C接口已关闭\n");

    return 0;
}
```

---

## 量子随机数生成

### 量子随机数 vs 伪随机数

```text
┌─────────────────────────────────────────────────────────────────────┐
│                    随机数生成方法对比                                 │
├────────────────────┬─────────────────────┬──────────────────────────┤
│      特性          │     伪随机数(PRNG)   │     量子随机数(QRNG)      │
├────────────────────┼─────────────────────┼──────────────────────────┤
│  生成机制          │  确定性算法          │  量子测量不确定性          │
│  可预测性          │  可预测（已知种子）   │  本质上不可预测           │
│  熵源              │  软件/算法           │  量子物理过程             │
│  安全性            │  依赖于算法强度       │  信息论安全               │
│  速度              │  快                  │  较慢（依赖硬件）         │
│  典型应用          │  模拟、游戏           │  密码学、安全应用         │
└────────────────────┴─────────────────────┴──────────────────────────┘
```

### 量子随机数生成原理

```text
量子随机数生成流程:

┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  制备叠加态  │ →  │  应用H门    │  → │  测量量子比特│ → │  输出随机比特│
│  |0⟩        │    │  |+⟩        │    │  0 或 1     │    │  真随机数   │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘

原理: 量子测量是概率性的，|+⟩态测量得到0或1的概率各为50%
```

### C语言实现QRNG

```c
/**
 * @file quantum_rng.c
 * @brief 量子随机数生成器实现
 *
 * 此实现使用量子力学原理生成真随机数
 * 实际应用需要量子硬件或量子随机数服务API
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/* 模拟量子比特状态 */
typedef struct {
    double prob_0;  /* 测量得到0的概率 */
    double prob_1;  /* 测量得到1的概率 */
    double phase;   /* 相位 */
} SimulatedQubit;

/**
 * @brief 初始化量子随机数生成器
 *
 * 使用当前时间作为初始熵源
 * 实际QRNG不需要种子，因为量子过程本身就是随机的
 */
void qrng_init(void) {
    /* 在模拟中仍使用srand作为熵源 */
    srand((unsigned)time(NULL) ^ clock());
}

/**
 * @brief 模拟量子比特测量
 *
 * 模拟叠加态 (|0⟩ + |1⟩)/√2 的测量
 * 真实QRNG使用量子硬件完成此操作
 */
int qrng_measure_qubit(void) {
    /*
     * 模拟量子测量过程
     * 对于平衡叠加态 |+⟩，P(0) = P(1) = 0.5
     * 量子测量的随机性是本质的，非伪随机
     */

    /* 使用多个熵源增加随机性（模拟量子噪声） */
    unsigned long entropy = 0;
    entropy ^= (unsigned long)time(NULL);
    entropy ^= (unsigned long)clock();
    entropy ^= (unsigned long)&entropy;  /* ASLR熵 */

    /* 模拟量子测量的概率分布 */
    double random_val = (double)(rand() ^ entropy) / RAND_MAX;

    /* 50/50概率（模拟|+⟩态测量） */
    return (random_val < 0.5) ? 0 : 1;
}

/**
 * @brief 生成单个随机比特
 */
int qrng_random_bit(void) {
    return qrng_measure_qubit();
}

/**
 * @brief 生成8位随机字节
 */
uint8_t qrng_random_byte(void) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte = (byte << 1) | qrng_random_bit();
    }
    return byte;
}

/**
 * @brief 生成32位随机整数
 */
uint32_t qrng_random_uint32(void) {
    uint32_t value = 0;
    for (int i = 0; i < 32; i++) {
        value = (value << 1) | qrng_random_bit();
    }
    return value;
}

/**
 * @brief 生成64位随机整数
 */
uint64_t qrng_random_uint64(void) {
    return ((uint64_t)qrng_random_uint32() << 32) | qrng_random_uint32();
}

/**
 * @brief 生成指定范围内的随机整数 [min, max]
 */
int qrng_random_range(int min, int max) {
    if (min >= max) return min;

    uint32_t range = (uint32_t)(max - min + 1);
    uint32_t mask = 1;
    while (mask < range - 1) {
        mask = (mask << 1) | 1;
    }

    uint32_t result;
    do {
        result = qrng_random_uint32() & mask;
    } while (result >= range);

    return min + (int)result;
}

/**
 * @brief 生成[0, 1)区间的随机浮点数
 */
double qrng_random_double(void) {
    /* 使用53位精度 */
    uint64_t value = qrng_random_uint64() >> 11;
    return (double)value / (1ULL << 53);
}

/**
 * @brief 填充随机字节缓冲区
 */
void qrng_fill_buffer(uint8_t *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        buffer[i] = qrng_random_byte();
    }
}

/**
 * @brief 随机性测试 - 频数测试
 */
void qrng_frequency_test(int samples) {
    int zeros = 0, ones = 0;

    for (int i = 0; i < samples; i++) {
        if (qrng_random_bit() == 0) {
            zeros++;
        } else {
            ones++;
        }
    }

    printf("=== 频数测试结果 ===\n");
    printf("总样本数: %d\n", samples);
    printf("0的个数: %d (%.2f%%)\n", zeros, 100.0 * zeros / samples);
    printf("1的个数: %d (%.2f%%)\n", ones, 100.0 * ones / samples);
    printf("偏差: %.4f%%\n", 100.0 * fabs(zeros - ones) / samples);
    printf("理想值: 50.00%% 每种结果\n\n");
}

/**
 * @brief 运行游程测试
 */
void qrng_runs_test(int samples) {
    if (samples < 2) return;

    int *bits = malloc(samples * sizeof(int));
    for (int i = 0; i < samples; i++) {
        bits[i] = qrng_random_bit();
    }

    int runs = 1;  /* 游程数 */
    for (int i = 1; i < samples; i++) {
        if (bits[i] != bits[i-1]) {
            runs++;
        }
    }

    printf("=== 游程测试结果 ===\n");
    printf("总样本数: %d\n", samples);
    printf("游程数: %d\n", runs);
    printf("平均游程长度: %.2f\n", (double)samples / runs);
    printf("(随机序列应有较多游程)\n\n");

    free(bits);
}

/* 主函数 - 演示 */
int main(void) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║         量子随机数生成器 (QRNG) 演示                    ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    /* 初始化 */
    qrng_init();

    /* 随机性测试 */
    printf("进行随机性统计测试...\n\n");
    qrng_frequency_test(10000);
    qrng_runs_test(10000);

    /* 生成随机数示例 */
    printf("=== 随机数生成示例 ===\n\n");

    printf("随机比特序列 (64位):\n");
    for (int i = 0; i < 64; i++) {
        printf("%d", qrng_random_bit());
        if ((i + 1) % 8 == 0) printf(" ");
    }
    printf("\n\n");

    printf("随机字节 (16字节):\n");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", qrng_random_byte());
    }
    printf("\n\n");

    printf("随机整数:\n");
    printf("  uint32: %u\n", qrng_random_uint32());
    printf("  uint64: %llu\n", (unsigned long long)qrng_random_uint64());
    printf("  range [1,100]: %d\n", qrng_random_range(1, 100));
    printf("  double [0,1): %.6f\n", qrng_random_double());

    return 0;
}
```

---

## 后量子密码学

### 后量子密码学概述

随着量子计算的发展，Shor算法可以在多项式时间内破解RSA和椭圆曲线密码。后量子密码学（PQC）研究能抵抗量子计算机攻击的加密算法。

```text
┌─────────────────────────────────────────────────────────────────────┐
│                   密码学演进时间线                                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  经典密码学时代          量子计算威胁              后量子密码时代      │
│  ─────────────           ────────────            ─────────────      │
│  │RSA/ECC安全│          │Shor算法│               │格密码/编码│        │
│  │    ↓      │    →     │可破解  │      →        │哈希密码等 │        │
│  │广泛使用  │           │传统加密│               │量子安全   │        │
│  └─────────┘            └───────┘               └─────────┘         │
│                                                                     │
│  1994                    2000s                   2024+              │
│  RSA发布                 量子计算研究            NIST标准化           │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 后量子密码算法类别

```text
┌───────────────────────────────────────────────────────────────────────┐
│                     NIST标准化后量子算法                               │
├──────────────────────┬────────────────────────────────────────────────┤
│      算法类别         │              代表算法                          │
├──────────────────────┼────────────────────────────────────────────────┤
│                      │  CRYSTALS-Kyber (KEM)                          │
│      格密码          │  CRYSTALS-Dilithium (签名)                      │
│  (Lattice-based)     │  FALCON (签名)                                 │
│                      │  NTRU (KEM)                                    │
├──────────────────────┼────────────────────────────────────────────────┤
│      编码密码        │  Classic McEliece (KEM)                         │
│  (Code-based)        │  BIKE (KEM)                                    │
│                      │  HQC (KEM)                                     │
├──────────────────────┼────────────────────────────────────────────────┤
│      哈希签名        │  SPHINCS+ (签名)                                │
│  (Hash-based)        │  XMSS (签名)                                   │
│                      │  LMS (签名)                                    │
├──────────────────────┼────────────────────────────────────────────────┤
│      多变量密码      │  MAYO (签名)                                    │
│  (Multivariate)      │  UOV (签名)                                    │
├──────────────────────┼────────────────────────────────────────────────┤
│      同源密码        │  SIKE (已破解，不推荐)                           │
│  (Isogeny-based)     │  CSIDH                                         │
└──────────────────────┴────────────────────────────────────────────────┘
```

### Kyber KEM 原理与C实现

```c
/**
 * @file kyber_demo.c
 * @brief CRYSTALS-Kyber 密钥封装机制概念实现
 *
 * Kyber基于模格(Module-LWE)问题
 * 这是简化版示例，展示核心概念
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Kyber参数（简化版 Kyber-512） */
#define KYBER_N 256           /* 多项式次数 */
#define KYBER_Q 3329          /* 模数 */
#define KYBER_ETA 3           /* 错误分布参数 */
#define KYBER_DU 10           /* 压缩参数 */
#define KYBER_DV 4            /* 压缩参数 */

/* 多项式环 R_q = Z_q[X]/(X^n + 1) */
typedef struct {
    int16_t coeffs[KYBER_N];  /* 系数范围 [0, Q-1] */
} Poly;

/* 多项式向量 */
typedef struct {
    Poly vec[2];  /* 对于 Kyber-512, k=2 */
} PolyVec;

/* Kyber公钥 */
typedef struct {
    PolyVec t;           /* A*s + e */
    uint8_t seed[32];    /* 生成A的种子 */
} KyberPublicKey;

/* Kyber私钥 */
typedef struct {
    PolyVec s;           /* 秘密向量 */
} KyberPrivateKey;

/* 密文 */
typedef struct {
    PolyVec u;           /* A^T*r + e1 */
    Poly v;              /* t^T*r + e2 + m */
} KyberCiphertext;

/* 共享密钥长度 */
#define KYBER_SHARED_SECRET_BYTES 32

/**
 * @brief 中心二项分布采样（模拟）
 *
 * 从中心二项分布采样小整数
 * 实际实现需要安全随机数生成器
 */
int16_t cbd_sample(void) {
    /* 简化：返回 -ETA 到 ETA 之间的随机值 */
    return (rand() % (2 * KYBER_ETA + 1)) - KYBER_ETA;
}

/**
 * @brief 多项式乘法（NTT域）
 *
 * 实际实现使用数论变换(NTT)加速
 * 这里展示简化版本
 */
void poly_mul(const Poly *a, const Poly *b, Poly *result) {
    /* 简化的多项式乘法（不使用NTT） */
    memset(result->coeffs, 0, sizeof(result->coeffs));

    for (int i = 0; i < KYBER_N; i++) {
        for (int j = 0; j < KYBER_N; j++) {
            int idx = (i + j) % KYBER_N;
            int sign = (i + j < KYBER_N) ? 1 : -1;  /* X^n = -1 */
            result->coeffs[idx] += sign * a->coeffs[i] * b->coeffs[j];
            result->coeffs[idx] %= KYBER_Q;
        }
    }
}

/**
 * @brief 多项式加法
 */
void poly_add(const Poly *a, const Poly *b, Poly *result) {
    for (int i = 0; i < KYBER_N; i++) {
        int sum = a->coeffs[i] + b->coeffs[i];
        result->coeffs[i] = ((sum % KYBER_Q) + KYBER_Q) % KYBER_Q;
    }
}

/**
 * @brief 生成小多项式（错误多项式）
 */
void poly_generate_small(Poly *p) {
    for (int i = 0; i < KYBER_N; i++) {
        p->coeffs[i] = cbd_sample();
        /* 映射到 [0, Q-1] */
        p->coeffs[i] = ((p->coeffs[i] % KYBER_Q) + KYBER_Q) % KYBER_Q;
    }
}

/**
 * @brief Kyber密钥生成
 *
 * 算法:
 * 1. 生成随机种子
 * 2. 从种子扩展矩阵A
 * 3. 采样秘密s和错误e
 * 4. 计算 t = A*s + e
 * 5. 公钥 = (t, seed), 私钥 = s
 */
void kyber_keygen(KyberPublicKey *pk, KyberPrivateKey *sk) {
    printf("=== Kyber密钥生成 ===\n");

    /* 1. 生成随机种子（模拟） */
    for (int i = 0; i < 32; i++) {
        pk->seed[i] = rand() & 0xFF;
    }
    printf("1. 生成随机种子\n");

    /* 2. 采样秘密向量 s */
    for (int i = 0; i < 2; i++) {
        poly_generate_small(&sk->s.vec[i]);
    }
    printf("2. 采样秘密向量 s\n");

    /* 3. 采样错误向量 e */
    PolyVec e;
    for (int i = 0; i < 2; i++) {
        poly_generate_small(&e.vec[i]);
    }
    printf("3. 采样错误向量 e\n");

    /* 4. 模拟计算 t = A*s + e */
    /* 实际需要从种子扩展矩阵A */
    printf("4. 计算公钥 t = A·s + e\n");
    for (int i = 0; i < 2; i++) {
        /* 简化的矩阵向量乘法 */
        memcpy(&pk->t.vec[i], &sk->s.vec[i], sizeof(Poly));
        poly_add(&pk->t.vec[i], &e.vec[i], &pk->t.vec[i]);
    }

    printf("密钥生成完成\n\n");
}

/**
 * @brief Kyber加密（封装）
 *
 * 算法:
 * 1. 从公钥恢复矩阵A
 * 2. 采样随机r, e1, e2
 * 3. 计算 u = A^T*r + e1
 * 4. 计算 v = t^T*r + e2 + m
 * 5. 压缩并输出密文(u, v)
 */
void kyber_encaps(const KyberPublicKey *pk,
                  const uint8_t message[32],
                  KyberCiphertext *ct,
                  uint8_t shared_secret[KYBER_SHARED_SECRET_BYTES]) {
    printf("=== Kyber封装 ===\n");

    /* 1. 采样随机向量 r */
    PolyVec r;
    for (int i = 0; i < 2; i++) {
        poly_generate_small(&r.vec[i]);
    }
    printf("1. 采样随机向量 r\n");

    /* 2. 采样错误 e1, e2 */
    PolyVec e1;
    Poly e2;
    for (int i = 0; i < 2; i++) {
        poly_generate_small(&e1.vec[i]);
    }
    poly_generate_small(&e2);
    printf("2. 采样错误多项式\n");

    /* 3. 计算 u = A^T*r + e1 */
    printf("3. 计算 u = A^T·r + e1\n");
    for (int i = 0; i < 2; i++) {
        /* 简化的计算 */
        memcpy(&ct->u.vec[i], &r.vec[i], sizeof(Poly));
        poly_add(&ct->u.vec[i], &e1.vec[i], &ct->u.vec[i]);
    }

    /* 4. 计算 v = t^T*r + e2 + m (简化) */
    printf("4. 计算 v = t^T·r + e2 + Encode(m)\n");
    memcpy(&ct->v, &e2, sizeof(Poly));

    /* 派生共享密钥 */
    printf("5. 派生共享密钥\n");
    for (int i = 0; i < KYBER_SHARED_SECRET_BYTES; i++) {
        shared_secret[i] = rand() & 0xFF;
    }

    printf("封装完成\n\n");
}

/**
 * @brief Kyber解密（解封装）
 *
 * 算法:
 * 1. 计算 m' = v - s^T*u
 * 2. 解码并输出共享密钥
 */
void kyber_decaps(const KyberPrivateKey *sk,
                  const KyberCiphertext *ct,
                  uint8_t shared_secret[KYBER_SHARED_SECRET_BYTES]) {
    printf("=== Kyber解封装 ===\n");

    /* 计算 m' = v - s^T*u */
    printf("1. 计算 m' = v - s^T·u\n");

    /* 解码消息并派生共享密钥 */
    printf("2. 解码消息并派生共享密钥\n");
    for (int i = 0; i < KYBER_SHARED_SECRET_BYTES; i++) {
        shared_secret[i] = rand() & 0xFF;
    }

    printf("解封装完成\n\n");
}

/* 主函数演示 */
int main(void) {
    srand(42);

    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║     CRYSTALS-Kyber 密钥封装机制 (KEM) 演示              ║\n");
    printf("║     (基于模格的后量子密码算法)                          ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    /* 密钥生成 */
    KyberPublicKey pk;
    KyberPrivateKey sk;
    kyber_keygen(&pk, &sk);

    /* 封装 */
    uint8_t message[32] = {0};
    KyberCiphertext ct;
    uint8_t shared_secret_encap[KYBER_SHARED_SECRET_BYTES];
    kyber_encaps(&pk, message, &ct, shared_secret_encap);

    /* 解封装 */
    uint8_t shared_secret_decap[KYBER_SHARED_SECRET_BYTES];
    kyber_decaps(&sk, &ct, shared_secret_decap);

    printf("封装得到的共享密钥: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X", shared_secret_encap[i]);
    }
    printf("...\n");

    printf("解封装得到的共享密钥: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X", shared_secret_decap[i]);
    }
    printf("...\n");

    printf("\n注：在实际实现中，两个密钥应当相同\n");
    printf("    这是基于MLWE问题的密码学安全性保证\n");

    return 0;
}
```

---

## 实践案例

### 案例1：量子密钥分发模拟

```c
/**
 * BB84协议简化实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define KEY_LENGTH 128

/* 基选择: 0=Rectilinear(|0⟩,|1⟩), 1=Diagonal(|+⟩,|-⟩) */
typedef struct {
    int bits[KEY_LENGTH];       /* 原始比特 */
    int bases[KEY_LENGTH];      /* 选择的基 */
    int measurements[KEY_LENGTH]; /* 测量结果 */
} BB84Participant;

void bb84_protocol_demo(void) {
    printf("=== BB84 量子密钥分发协议 ===\n\n");

    BB84Participant alice = {0};
    BB84Participant bob = {0};

    /* Alice准备 */
    printf("1. Alice随机选择比特和基:\n");
    for (int i = 0; i < KEY_LENGTH; i++) {
        alice.bits[i] = rand() % 2;
        alice.bases[i] = rand() % 2;
    }
    printf("   生成 %d 个随机比特和基\n\n", KEY_LENGTH);

    /* Bob测量 */
    printf("2. Bob随机选择基进行测量:\n");
    for (int i = 0; i < KEY_LENGTH; i++) {
        bob.bases[i] = rand() % 2;
        /* 如果基相同，测量结果正确；否则50%概率 */
        if (bob.bases[i] == alice.bases[i]) {
            bob.measurements[i] = alice.bits[i];
        } else {
            bob.measurements[i] = rand() % 2;
        }
    }
    printf("   完成测量\n\n");

    /* 基比对 */
    printf("3. 公开比对基的选择:\n");
    int matching_bases = 0;
    for (int i = 0; i < KEY_LENGTH; i++) {
        if (alice.bases[i] == bob.bases[i]) {
            matching_bases++;
        }
    }
    printf("   基匹配率: ~50%% (%d/%d)\n\n", matching_bases, KEY_LENGTH);

    /* 密钥筛选 */
    printf("4. 保留基匹配的位置，形成原始密钥\n");
    printf("   预期密钥长度: ~%d 比特\n\n", matching_bases);

    /* 错误检测 */
    printf("5. 公开部分比特进行错误检测\n");
    printf("   如果错误率异常高，可能遭受窃听\n\n");

    /* 隐私放大 */
    printf("6. 隐私放大：从原始密钥提取安全密钥\n");
}

int main(void) {
    srand(time(NULL));
    bb84_protocol_demo();
    return 0;
}
```

---

## 参考资源

### 量子计算框架

| 框架 | 语言 | 特点 | 链接 |
|------|------|------|------|
| Qiskit | Python | IBM官方SDK，功能全面 | qiskit.org |
| Cirq | Python | Google开发，适合NISQ | github.com/quantumlib/Cirq |
| PennyLane | Python | 量子机器学习 | pennylane.ai |
| QuTiP | Python | 量子动力学模拟 | qutip.org |
| Q# | Q#/C# | Microsoft量子开发套件 | docs.microsoft.com/quantum |

### 后量子密码标准

- **NIST PQC标准化**: <https://csrc.nist.gov/projects/post-quantum-cryptography>
- **CRYSTALS-Kyber**: <https://pq-crystals.org/kyber/>
- **CRYSTALS-Dilithium**: <https://pq-crystals.org/dilithium/>

### 学习资源

- **Qiskit教科书**: <https://qiskit.org/textbook/>
- **Quantum Computing for Computer Scientists**: 经典教材
- **arXiv quant-ph**: 最新量子计算论文

---

## 总结

量子计算正在从理论走向实践，主要应用领域包括：

1. **量子比特与量子门**：理解叠加、纠缠等基本概念
2. **量子编程**：使用Qiskit等框架开发量子算法
3. **量子随机数**：利用量子不确定性生成真随机数
4. **后量子密码**：应对量子计算威胁的新一代密码学

随着量子硬件的发展，掌握量子计算接口技术将成为系统程序员的重要技能。

---

*最后更新: 2026年3月*:


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
