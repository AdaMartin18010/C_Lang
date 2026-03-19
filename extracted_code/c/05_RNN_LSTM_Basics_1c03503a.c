/*
 * Auto-generated from: 11_Machine_Learning_C\05_RNN_LSTM_Basics.md
 * Line: 73
 * Language: c
 * Block ID: 1c03503a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
