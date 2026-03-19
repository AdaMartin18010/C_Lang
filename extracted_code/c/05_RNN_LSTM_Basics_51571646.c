/*
 * Auto-generated from: 11_Machine_Learning_C\05_RNN_LSTM_Basics.md
 * Line: 420
 * Language: c
 * Block ID: 51571646
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
