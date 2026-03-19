/*
 * Auto-generated from: 11_Machine_Learning_C\05_RNN_LSTM_Basics.md
 * Line: 344
 * Language: c
 * Block ID: 886e167b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
