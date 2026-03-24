
---

## 🔗 全面知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../readme.md](../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/readme.md](../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/readme.md](../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

### 【阶段层】学习定位

**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链

| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/readme.md](../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/readme.md](../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网

| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/readme.md](../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/readme.md](../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

### 【总体层】知识体系架构

```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择

| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/readme.md) |

---

# 梯度下降与优化算法 (Gradient Descent & Optimizers)

> **难度**: L4 | **预估学习时间**: 3-4小时
> 包含：BGD, SGD, Momentum, AdaGrad, RMSprop, Adam

---

## 1. 梯度下降基础

### 1.1 核心思想

梯度下降通过沿损失函数梯度的反方向更新参数，逐步最小化损失：

$$
\theta_{t+1} = \theta_t - \eta \nabla_\theta J(\theta)
$$

### 1.2 批量梯度下降 (BGD)

使用**全部训练数据**计算梯度：

```c
// BGD伪代码
for epoch in range(num_epochs):
    gradient = compute_gradient_full_dataset(X, y, theta)
    theta = theta - learning_rate * gradient
```

**特点**:

- 梯度估计准确
- 计算成本高（大数据集不可行）

### 1.3 随机梯度下降 (SGD)

每次使用**单个样本**更新：

```c
typedef struct {
    double* weights;
    double bias;
    double learning_rate;
} SGD_Optimizer;

// SGD更新
void sgd_update(SGD_Optimizer* opt, double* x, double y_true,
                double y_pred, int n_features) {
    double error = y_pred - y_true;

    for (int i = 0; i < n_features; i++) {
        double gradient = error * x[i];
        opt->weights[i] -= opt->learning_rate * gradient;
    }
    opt->bias -= opt->learning_rate * error;
}
```

**特点**:

- 计算快，内存友好
- 梯度噪声大，收敛不稳定

### 1.4 小批量梯度下降 (Mini-batch GD)

使用**小批量样本**（通常32-256个）平衡两者：

```c
#define BATCH_SIZE 32

void minibatch_update(Network* net, Matrix* X_batch, Matrix* y_batch) {
    Matrix* gradients = compute_gradients(net, X_batch, y_batch);

    // 平均梯度
    scale_matrix(gradients, 1.0 / BATCH_SIZE);

    // 更新参数
    for (int i = 0; i < net->n_layers; i++) {
        axpy_matrix(-net->learning_rate, gradients[i], net->weights[i]);
    }

    free_matrix(gradients);
}
```

---


---

## 📑 目录

- [梯度下降与优化算法 (Gradient Descent \& Optimizers)](#梯度下降与优化算法-gradient-descent--optimizers)
  - [1. 梯度下降基础](#1-梯度下降基础)
    - [1.1 核心思想](#11-核心思想)
    - [1.2 批量梯度下降 (BGD)](#12-批量梯度下降-bgd)
    - [1.3 随机梯度下降 (SGD)](#13-随机梯度下降-sgd)
    - [1.4 小批量梯度下降 (Mini-batch GD)](#14-小批量梯度下降-mini-batch-gd)
  - [📑 目录](#-目录)
  - [2. 动量方法](#2-动量方法)
    - [2.1 Momentum](#21-momentum)
    - [2.2 Nesterov加速梯度 (NAG)](#22-nesterov加速梯度-nag)
  - [3. 自适应学习率方法](#3-自适应学习率方法)
    - [3.1 AdaGrad](#31-adagrad)
    - [3.2 RMSprop](#32-rmsprop)
    - [3.3 Adam (Adaptive Moment Estimation)](#33-adam-adaptive-moment-estimation)
  - [4. 学习率调度](#4-学习率调度)
    - [4.1 衰减策略](#41-衰减策略)
    - [4.2 预热 (Warmup)](#42-预热-warmup)
  - [5. 优化器对比](#5-优化器对比)
  - [6. 完整实现示例](#6-完整实现示例)
  - [7. 参考文献](#7-参考文献)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 2. 动量方法

### 2.1 Momentum

累积历史梯度方向，加速收敛：

$$
v_t = \gamma v_{t-1} + \eta \nabla_\theta J(\theta) \\
\theta_{t+1} = \theta_t - v_t
$$

```c
typedef struct {
    double* velocity;
    double momentum;
    double learning_rate;
} Momentum_Optimizer;

void momentum_update(Momentum_Optimizer* opt, double* params,
                     double* gradients, int n_params) {
    for (int i = 0; i < n_params; i++) {
        // v = gamma * v + lr * grad
        opt->velocity[i] = opt->momentum * opt->velocity[i]
                         + opt->learning_rate * gradients[i];
        // theta = theta - v
        params[i] -= opt->velocity[i];
    }
}
```

**效果**: 在一致方向上加速，减少振荡。

### 2.2 Nesterov加速梯度 (NAG)

在动量基础上，先沿动量方向"展望"再计算梯度：

```c
void nesterov_update(Optimizer* opt, double* params,
                     double* gradients, int n_params) {
    for (int i = 0; i < n_params; i++) {
        double prev_velocity = opt->velocity[i];

        // 计算临时位置（展望）
        double lookahead = params[i] - opt->momentum * prev_velocity;

        // 在临时位置计算梯度
        double grad_at_lookahead = compute_gradient_at(lookahead);

        // 更新速度
        opt->velocity[i] = opt->momentum * prev_velocity
                         + opt->learning_rate * grad_at_lookahead;

        params[i] -= opt->velocity[i];
    }
}
```

---

## 3. 自适应学习率方法

### 3.1 AdaGrad

为每个参数维护独立的学习率，基于历史梯度平方和：

$$
\theta_{t+1,i} = \theta_{t,i} - \frac{\eta}{\sqrt{G_{t,ii} + \epsilon}} \cdot g_{t,i}
$$

```c
typedef struct {
    double* squared_grad_sum;  // G
    double epsilon;
    double learning_rate;
} AdaGrad_Optimizer;

void adagrad_update(AdaGrad_Optimizer* opt, double* params,
                    double* gradients, int n_params) {
    for (int i = 0; i < n_params; i++) {
        // 累积平方梯度
        opt->squared_grad_sum[i] += gradients[i] * gradients[i];

        // 自适应学习率更新
        double adapted_lr = opt->learning_rate /
                           (sqrt(opt->squared_grad_sum[i]) + opt->epsilon);

        params[i] -= adapted_lr * gradients[i];
    }
}
```

**问题**: 学习率单调递减，可能过早停止。

### 3.2 RMSprop

使用**指数移动平均**替代累积和，解决AdaGrad问题：

```c
typedef struct {
    double* squared_grad_avg;  // E[g^2]
    double decay_rate;         // rho, 通常0.9
    double epsilon;
    double learning_rate;
} RMSprop_Optimizer;

void rmsprop_update(RMSprop_Optimizer* opt, double* params,
                    double* gradients, int n_params) {
    for (int i = 0; i < n_params; i++) {
        // E[g^2] = rho * E[g^2] + (1-rho) * g^2
        opt->squared_grad_avg[i] = opt->decay_rate * opt->squared_grad_avg[i]
                                  + (1 - opt->decay_rate) * gradients[i] * gradients[i];

        double adapted_lr = opt->learning_rate /
                           (sqrt(opt->squared_grad_avg[i]) + opt->epsilon);

        params[i] -= adapted_lr * gradients[i];
    }
}
```

### 3.3 Adam (Adaptive Moment Estimation)

结合动量和RMSprop优点，维护一阶和二阶矩估计：

$$
m_t = \beta_1 m_{t-1} + (1-\beta_1) g_t \quad \text{(一阶矩/动量)} \\
v_t = \beta_2 v_{t-1} + (1-\beta_2) g_t^2 \quad \text{(二阶矩)} \\
\hat{m}_t = \frac{m_t}{1-\beta_1^t}, \quad \hat{v}_t = \frac{v_t}{1-\beta_2^t} \quad \text{(偏差修正)} \\
\theta_{t+1} = \theta_t - \frac{\eta}{\sqrt{\hat{v}_t} + \epsilon} \hat{m}_t
$$

```c
typedef struct {
    double* m;           // 一阶矩估计
    double* v;           // 二阶矩估计
    double beta1;        // 通常0.9
    double beta2;        // 通常0.999
    double epsilon;      // 通常1e-8
    double learning_rate;
    int t;               // 时间步
} Adam_Optimizer;

Adam_Optimizer* adam_create(int n_params, double lr) {
    Adam_Optimizer* opt = malloc(sizeof(Adam_Optimizer));
    opt->m = calloc(n_params, sizeof(double));
    opt->v = calloc(n_params, sizeof(double));
    opt->beta1 = 0.9;
    opt->beta2 = 0.999;
    opt->epsilon = 1e-8;
    opt->learning_rate = lr;
    opt->t = 0;
    return opt;
}

void adam_update(Adam_Optimizer* opt, double* params,
                 double* gradients, int n_params) {
    opt->t++;

    for (int i = 0; i < n_params; i++) {
        // 更新一阶和二阶矩
        opt->m[i] = opt->beta1 * opt->m[i] + (1 - opt->beta1) * gradients[i];
        opt->v[i] = opt->beta2 * opt->v[i] + (1 - opt->beta2) * gradients[i] * gradients[i];

        // 偏差修正
        double m_hat = opt->m[i] / (1 - pow(opt->beta1, opt->t));
        double v_hat = opt->v[i] / (1 - pow(opt->beta2, opt->t));

        // 参数更新
        params[i] -= opt->learning_rate * m_hat / (sqrt(v_hat) + opt->epsilon);
    }
}
```

---

## 4. 学习率调度

### 4.1 衰减策略

```c
double lr_schedule(int epoch, double initial_lr, int decay_epochs) {
    // 阶梯衰减
    if (epoch % decay_epochs == 0 && epoch > 0) {
        return initial_lr * 0.5;
    }

    // 指数衰减
    // return initial_lr * exp(-0.1 * epoch);

    // 余弦退火
    // return initial_lr * (1 + cos(M_PI * epoch / max_epochs)) / 2;

    return initial_lr;
}
```

### 4.2 预热 (Warmup)

训练初期从小学习率逐渐增大：

```c
double warmup_lr(int step, int warmup_steps, double target_lr) {
    if (step < warmup_steps) {
        return target_lr * ((double)step / warmup_steps);
    }
    return target_lr;
}
```

---

## 5. 优化器对比

| 优化器 | 内存需求 | 收敛速度 | 超参数敏感性 | 适用场景 |
|:-------|:---------|:---------|:-------------|:---------|
| SGD | 低 | 慢 | 低 | 大数据集，简单模型 |
| Momentum | 中 | 中 | 中 | 通用 |
| AdaGrad | 中 | 快(初期) | 低 | 稀疏梯度 |
| RMSprop | 中 | 快 | 中 | RNN，非平稳目标 |
| **Adam** | 中 | 快 | 低 | **通用首选** |
| AdamW | 中 | 快 | 低 | 需要权重衰减时 |

---

## 6. 完整实现示例

```c
// 简单的线性回归 + Adam优化器
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    double w, b;
    double m_w, v_w, m_b, v_b;
    double beta1, beta2, epsilon, lr;
    int t;
} Adam;

void adam_init(Adam* opt, double lr) {
    opt->w = opt->b = 0;
    opt->m_w = opt->v_w = opt->m_b = opt->v_b = 0;
    opt->beta1 = 0.9; opt->beta2 = 0.999;
    opt->epsilon = 1e-8; opt->lr = lr; opt->t = 0;
}

void adam_step(Adam* opt, double x, double y) {
    double pred = opt->w * x + opt->b;
    double error = pred - y;

    double grad_w = error * x;
    double grad_b = error;

    opt->t++;
    double lr = opt->lr * sqrt(1 - pow(opt->beta2, opt->t))
                     / (1 - pow(opt->beta1, opt->t));

    opt->m_w = opt->beta1 * opt->m_w + (1 - opt->beta1) * grad_w;
    opt->v_w = opt->beta2 * opt->v_w + (1 - opt->beta2) * grad_w * grad_w;
    opt->w -= lr * opt->m_w / (sqrt(opt->v_w) + opt->epsilon);

    opt->m_b = opt->beta1 * opt->m_b + (1 - opt->beta1) * grad_b;
    opt->v_b = opt->beta2 * opt->v_b + (1 - opt->beta2) * grad_b * grad_b;
    opt->b -= lr * opt->m_b / (sqrt(opt->v_b) + opt->epsilon);
}

int main() {
    // y = 2x + 1
    double X[] = {1, 2, 3, 4, 5};
    double y[] = {3, 5, 7, 9, 11};
    int n = 5;

    Adam opt;
    adam_init(&opt, 0.1);

    for (int epoch = 0; epoch < 1000; epoch++) {
        for (int i = 0; i < n; i++) {
            adam_step(&opt, X[i], y[i]);
        }
    }

    printf("w = %.4f, b = %.4f\n", opt.w, opt.b);
    // 输出: w ≈ 2.0, b ≈ 1.0

    return 0;
}
```

---

## 7. 参考文献

1. **Ruder, S.** (2016). "An overview of gradient descent optimization algorithms". arXiv:1609.04747.
2. **Kingma, D.P. & Ba, J.** (2015). "Adam: A method for stochastic optimization". ICLR.
3. **Duchi, J.** et al. (2011). "Adaptive subgradient methods for online learning and stochastic optimization". JMLR.
4. **Tieleman, T. & Hinton, G.** (2012). "Lecture 6.5 - RMSprop". Coursera.

---

> **关联文档**: [神经网络基础](./01_Neural_Network_Basics.md) | [矩阵运算](./02_Matrix_Operations_BLAS.md) | [CNN基础](./04_CNN_Fundamentals.md)


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
