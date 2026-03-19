/*
 * Auto-generated from: 11_Machine_Learning_C\05_RNN_LSTM_Basics.md
 * Line: 208
 * Language: c
 * Block ID: 3f71abcf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
