# 循环神经网络与LSTM (RNN & LSTM)

> **难度**: L5 | **预估学习时间**: 4-5小时
> 包含：RNN基础、LSTM门控机制、GRU、序列建模

---

## 1. 序列建模问题

传统神经网络假设输入独立同分布，但许多数据具有**序列结构**：

- 时间序列预测
- 自然语言处理
- 语音识别
- DNA序列分析

### 1.1 挑战

```text
问题1: 变长序列 → 需要处理不同长度输入
问题2: 长期依赖 → 远距离信息关联
问题3: 参数共享 → 跨时间步复用权重
```

---


---

## 📑 目录

- [循环神经网络与LSTM (RNN \& LSTM)](#循环神经网络与lstm-rnn--lstm)
  - [1. 序列建模问题](#1-序列建模问题)
    - [1.1 挑战](#11-挑战)
  - [📑 目录](#-目录)
  - [2. 循环神经网络 (RNN)](#2-循环神经网络-rnn)
    - [2.1 基本结构](#21-基本结构)
    - [2.2 C语言实现](#22-c语言实现)
    - [2.3 梯度消失问题](#23-梯度消失问题)
  - [3. LSTM (长短期记忆网络)](#3-lstm-长短期记忆网络)
    - [3.1 核心思想](#31-核心思想)
    - [3.2 LSTM公式](#32-lstm公式)
    - [3.3 C语言实现](#33-c语言实现)
  - [4. GRU (门控循环单元)](#4-gru-门控循环单元)
    - [4.1 简化结构](#41-简化结构)
    - [4.2 对比](#42-对比)
  - [5. 应用：字符级语言模型](#5-应用字符级语言模型)
  - [6. 双向RNN](#6-双向rnn)
  - [7. 参考文献](#7-参考文献)


---

## 2. 循环神经网络 (RNN)

### 2.1 基本结构

RNN在每个时间步接收输入和前一时间步的隐藏状态：

$$
h_t = \tanh(W_{hh} h_{t-1} + W_{xh} x_t + b_h) \\
y_t = W_{hy} h_t + b_y
$$

```text
       x₁ → [RNN Cell] → h₁ → [RNN Cell] → h₂ → ...
              ↑              ↑
             h₀             h₁
```

### 2.2 C语言实现

```c
typedef struct {
    int input_size;
    int hidden_size;
    int output_size;

    // 权重矩阵
    float* Wxh;  // [hidden_size × input_size]
    float* Whh;  // [hidden_size × hidden_size]
    float* Why;  // [output_size × hidden_size]
    float* bh;   // [hidden_size]
    float* by;   // [output_size]

    // 状态
    float* h;    // 当前隐藏状态 [hidden_size]
} RNN;

RNN* rnn_create(int input_size, int hidden_size, int output_size) {
    RNN* rnn = calloc(1, sizeof(RNN));
    rnn->input_size = input_size;
    rnn->hidden_size = hidden_size;
    rnn->output_size = output_size;

    rnn->Wxh = calloc(hidden_size * input_size, sizeof(float));
    rnn->Whh = calloc(hidden_size * hidden_size, sizeof(float));
    rnn->Why = calloc(output_size * hidden_size, sizeof(float));
    rnn->bh = calloc(hidden_size, sizeof(float));
    rnn->by = calloc(output_size, sizeof(float));
    rnn->h = calloc(hidden_size, sizeof(float));

    // Xavier初始化
    float scale = sqrtf(2.0f / (input_size + hidden_size));
    for (int i = 0; i < hidden_size * input_size; i++)
        rnn->Wxh[i] = ((float)rand() / RAND_MAX - 0.5f) * 2 * scale;

    scale = sqrtf(2.0f / (hidden_size + hidden_size));
    for (int i = 0; i < hidden_size * hidden_size; i++)
        rnn->Whh[i] = ((float)rand() / RAND_MAX - 0.5f) * 2 * scale;

    scale = sqrtf(2.0f / (hidden_size + output_size));
    for (int i = 0; i < output_size * hidden_size; i++)
        rnn->Why[i] = ((float)rand() / RAND_MAX - 0.5f) * 2 * scale;

    return rnn;
}

// 前向传播一步
void rnn_step(RNN* rnn, const float* x, float* y) {
    // h_new = tanh(Wxh @ x + Whh @ h + bh)
    float* h_new = calloc(rnn->hidden_size, sizeof(float));

    // Wxh @ x
    for (int i = 0; i < rnn->hidden_size; i++) {
        float sum = 0;
        for (int j = 0; j < rnn->input_size; j++) {
            sum += rnn->Wxh[i * rnn->input_size + j] * x[j];
        }
        h_new[i] = sum;
    }

    // Whh @ h
    for (int i = 0; i < rnn->hidden_size; i++) {
        float sum = 0;
        for (int j = 0; j < rnn->hidden_size; j++) {
            sum += rnn->Whh[i * rnn->hidden_size + j] * rnn->h[j];
        }
        h_new[i] += sum + rnn->bh[i];
    }

    // tanh激活
    for (int i = 0; i < rnn->hidden_size; i++) {
        rnn->h[i] = tanhf(h_new[i]);
    }

    free(h_new);

    // y = Why @ h + by
    for (int i = 0; i < rnn->output_size; i++) {
        float sum = rnn->by[i];
        for (int j = 0; j < rnn->hidden_size; j++) {
            sum += rnn->Why[i * rnn->hidden_size + j] * rnn->h[j];
        }
        y[i] = sum;
    }
}

// 序列前向传播
void rnn_forward(RNN* rnn, const float* sequence, int seq_len, float* outputs) {
    // 重置隐藏状态
    memset(rnn->h, 0, rnn->hidden_size * sizeof(float));

    for (int t = 0; t < seq_len; t++) {
        rnn_step(rnn, &sequence[t * rnn->input_size],
                 &outputs[t * rnn->output_size]);
    }
}
```

### 2.3 梯度消失问题

标准RNN的梯度随时间反向传播时呈指数衰减/增长：

```text
∂L/∂h₁ = ∂L/∂h_T × ∏(t=2 to T) diag(1 - tanh²(h_t)) × Whh

如果 ||Whh|| < 1: 梯度消失（无法学习长期依赖）
如果 ||Whh|| > 1: 梯度爆炸（数值不稳定）
```

---

## 3. LSTM (长短期记忆网络)

### 3.1 核心思想

LSTM通过**门控机制**控制信息流动：

- **遗忘门**: 决定丢弃哪些旧信息
- **输入门**: 决定存储哪些新信息
- **输出门**: 决定输出哪些信息
- **细胞状态**: 长期记忆的"传送带"

### 3.2 LSTM公式

$$
f_t = \sigma(W_f \cdot [h_{t-1}, x_t] + b_f) \quad \text{(遗忘门)} \\
i_t = \sigma(W_i \cdot [h_{t-1}, x_t] + b_i) \quad \text{(输入门)} \\
\tilde{C}_t = \tanh(W_C \cdot [h_{t-1}, x_t] + b_C) \quad \text{(候选状态)} \\
C_t = f_t \odot C_{t-1} + i_t \odot \tilde{C}_t \quad \text{(细胞状态更新)} \\
o_t = \sigma(W_o \cdot [h_{t-1}, x_t] + b_o) \quad \text{(输出门)} \\
h_t = o_t \odot \tanh(C_t) \quad \text{(隐藏状态)}
$$

### 3.3 C语言实现

```c
typedef struct {
    int input_size, hidden_size;

    // 权重: 每个门都有 [hidden_size × (input_size + hidden_size)]
    float* Wf, *Wi, *Wc, *Wo;
    float* bf, *bi, *bc, *bo;

    // 状态
    float* h;  // 隐藏状态
    float* c;  // 细胞状态

    // 缓存用于反向传播
    float** cache_f, **cache_i, **cache_c, **cache_o, **cache_h;
} LSTM;

LSTM* lstm_create(int input_size, int hidden_size) {
    LSTM* lstm = calloc(1, sizeof(LSTM));
    lstm->input_size = input_size;
    lstm->hidden_size = hidden_size;

    int gate_size = hidden_size * (input_size + hidden_size);
    lstm->Wf = calloc(gate_size, sizeof(float));
    lstm->Wi = calloc(gate_size, sizeof(float));
    lstm->Wc = calloc(gate_size, sizeof(float));
    lstm->Wo = calloc(gate_size, sizeof(float));

    lstm->bf = calloc(hidden_size, sizeof(float));
    lstm->bi = calloc(hidden_size, sizeof(float));
    lstm->bc = calloc(hidden_size, sizeof(float));
    lstm->bo = calloc(hidden_size, sizeof(float));

    lstm->h = calloc(hidden_size, sizeof(float));
    lstm->c = calloc(hidden_size, sizeof(float));

    // 初始化：遗忘门偏置设为1（有利于梯度流动）
    for (int i = 0; i < hidden_size; i++) lstm->bf[i] = 1.0f;

    return lstm;
}

// sigmoid和tanh
define SIGMOID(x) (1.0f / (1.0f + expf(-(x))))

void lstm_step(LSTM* lstm, const float* x, int t, int max_seq) {
    int h_size = lstm->hidden_size;
    int combined_size = lstm->input_size + h_size;
    float* combined = calloc(combined_size, sizeof(float));

    // [x_t, h_{t-1}]
    memcpy(combined, x, lstm->input_size * sizeof(float));
    memcpy(combined + lstm->input_size, lstm->h, h_size * sizeof(float));

    // 遗忘门
    float* f = calloc(h_size, sizeof(float));
    for (int i = 0; i < h_size; i++) {
        float sum = lstm->bf[i];
        for (int j = 0; j < combined_size; j++) {
            sum += lstm->Wf[i * combined_size + j] * combined[j];
        }
        f[i] = SIGMOID(sum);
    }

    // 输入门
    float* i_gate = calloc(h_size, sizeof(float));
    float* c_tilde = calloc(h_size, sizeof(float));
    for (int k = 0; k < h_size; k++) {
        float sum_i = lstm->bi[k];
        float sum_c = lstm->bc[k];
        for (int j = 0; j < combined_size; j++) {
            sum_i += lstm->Wi[k * combined_size + j] * combined[j];
            sum_c += lstm->Wc[k * combined_size + j] * combined[j];
        }
        i_gate[k] = SIGMOID(sum_i);
        c_tilde[k] = tanhf(sum_c);
    }

    // 细胞状态: c_t = f * c_{t-1} + i * c_tilde
    for (int k = 0; k < h_size; k++) {
        lstm->c[k] = f[k] * lstm->c[k] + i_gate[k] * c_tilde[k];
    }

    // 输出门
    float* o = calloc(h_size, sizeof(float));
    for (int k = 0; k < h_size; k++) {
        float sum = lstm->bo[k];
        for (int j = 0; j < combined_size; j++) {
            sum += lstm->Wo[k * combined_size + j] * combined[j];
        }
        o[k] = SIGMOID(sum);
    }

    // 隐藏状态: h_t = o * tanh(c_t)
    for (int k = 0; k < h_size; k++) {
        lstm->h[k] = o[k] * tanhf(lstm->c[k]);
    }

    // 缓存
    memcpy(lstm->cache_f[t], f, h_size * sizeof(float));
    memcpy(lstm->cache_i[t], i_gate, h_size * sizeof(float));
    memcpy(lstm->cache_c[t], c_tilde, h_size * sizeof(float));
    memcpy(lstm->cache_o[t], o, h_size * sizeof(float));
    memcpy(lstm->cache_h[t], lstm->h, h_size * sizeof(float));

    free(combined); free(f); free(i_gate); free(c_tilde); free(o);
}
```

---

## 4. GRU (门控循环单元)

### 4.1 简化结构

GRU将遗忘门和输入门合并为**更新门**：

$$
z_t = \sigma(W_z \cdot [h_{t-1}, x_t]) \quad \text{(更新门)} \\
r_t = \sigma(W_r \cdot [h_{t-1}, x_t]) \quad \text{(重置门)} \\
\tilde{h}_t = \tanh(W \cdot [r_t \odot h_{t-1}, x_t]) \quad \text{(候选状态)} \\
h_t = (1 - z_t) \odot h_{t-1} + z_t \odot \tilde{h}_t
$$

### 4.2 对比

| 特性 | LSTM | GRU | RNN |
|:-----|:-----|:----|:----|
| 参数数量 | 多 (4个门) | 中 (3个门) | 少 |
| 训练速度 | 慢 | 快 | 最快 |
| 长依赖能力 | 强 | 强 | 弱 |
| 数据量小 | 可能过拟合 | 推荐 | - |

---

## 5. 应用：字符级语言模型

```c
// 训练RNN生成文本
void train_language_model(RNN* rnn, const char* text, int len,
                          int vocab_size, int epochs) {
    float* inputs = calloc(len * vocab_size, sizeof(float));
    int* targets = calloc(len, sizeof(int));

    // one-hot编码
    for (int i = 0; i < len - 1; i++) {
        inputs[i * vocab_size + (text[i] & 0x7F)] = 1.0f;
        targets[i] = text[i + 1] & 0x7F;
    }

    for (int epoch = 0; epoch < epochs; epoch++) {
        float total_loss = 0;

        // 前向传播
        float* outputs = calloc((len - 1) * vocab_size, sizeof(float));
        rnn_forward(rnn, inputs, len - 1, outputs);

        // 计算交叉熵损失
        for (int t = 0; t < len - 1; t++) {
            // softmax
            softmax(&outputs[t * vocab_size], &outputs[t * vocab_size], vocab_size);
            total_loss -= logf(outputs[t * vocab_size + targets[t]] + 1e-8f);
        }

        // 反向传播 (BPTT)
        // ... 实现 truncated BPTT

        if (epoch % 100 == 0) {
            printf("Epoch %d, Loss: %.4f\n", epoch, total_loss / (len - 1));
        }

        free(outputs);
    }

    free(inputs); free(targets);
}

// 生成文本
void generate_text(RNN* rnn, int seed_char, int length, char* output) {
    memset(rnn->h, 0, rnn->hidden_size * sizeof(float));

    float x[128] = {0};
    x[seed_char] = 1.0f;

    for (int i = 0; i < length; i++) {
        float y[128];
        rnn_step(rnn, x, y);
        softmax(y, y, 128);

        // 采样下一个字符
        float r = (float)rand() / RAND_MAX;
        float cumsum = 0;
        int next_char = 0;
        for (int j = 0; j < 128; j++) {
            cumsum += y[j];
            if (cumsum >= r) {
                next_char = j;
                break;
            }
        }

        output[i] = (char)next_char;
        memset(x, 0, sizeof(x));
        x[next_char] = 1.0f;
    }
    output[length] = '\0';
}
```

---

## 6. 双向RNN

```c
void bidirectional_rnn_forward(RNN* forward_rnn, RNN* backward_rnn,
                               const float* sequence, int seq_len,
                               float* outputs) {
    int hidden_size = forward_rnn->hidden_size;
    float* forward_h = calloc(seq_len * hidden_size, sizeof(float));
    float* backward_h = calloc(seq_len * hidden_size, sizeof(float));

    // 前向RNN
    memset(forward_rnn->h, 0, hidden_size * sizeof(float));
    for (int t = 0; t < seq_len; t++) {
        rnn_step(forward_rnn, &sequence[t * forward_rnn->input_size], NULL);
        memcpy(&forward_h[t * hidden_size], forward_rnn->h, hidden_size * sizeof(float));
    }

    // 后向RNN
    memset(backward_rnn->h, 0, hidden_size * sizeof(float));
    for (int t = seq_len - 1; t >= 0; t--) {
        rnn_step(backward_rnn, &sequence[t * backward_rnn->input_size], NULL);
        memcpy(&backward_h[t * hidden_size], backward_rnn->h, hidden_size * sizeof(float));
    }

    // 拼接
    for (int t = 0; t < seq_len; t++) {
        memcpy(&outputs[t * 2 * hidden_size], &forward_h[t * hidden_size], hidden_size * sizeof(float));
        memcpy(&outputs[t * 2 * hidden_size + hidden_size], &backward_h[t * hidden_size], hidden_size * sizeof(float));
    }

    free(forward_h); free(backward_h);
}
```

---

## 7. 参考文献

1. **Hochreiter, S. & Schmidhuber, J.** (1997). "Long Short-Term Memory". *Neural Computation*.
2. **Cho, K.** et al. (2014). "Learning Phrase Representations using RNN Encoder-Decoder". *EMNLP*.
3. **Chung, J.** et al. (2014). "Empirical Evaluation of Gated Recurrent Neural Networks". *arXiv*.
4. **Karpathy, A.** (2015). "The Unreasonable Effectiveness of Recurrent Neural Networks". Blog post.

---

> **关联文档**: [神经网络基础](./01_Neural_Network_Basics.md) | [CNN基础](./04_CNN_Fundamentals.md) | [ONNX Runtime](./06_ONNX_Runtime_C.md)


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
