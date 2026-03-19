/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 273
 * Language: c
 * Block ID: 918f2ffa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ReLU: max(0, x)
void relu_forward(float* data, int n) {
    for (int i = 0; i < n; i++) {
        if (data[i] < 0) data[i] = 0;
    }
}

void relu_backward(const float* grad_output, const float* input,
                   float* grad_input, int n) {
    for (int i = 0; i < n; i++) {
        grad_input[i] = (input[i] > 0) ? grad_output[i] : 0;
    }
}

// Leaky ReLU: max(alpha*x, x)
void leaky_relu_forward(float* data, int n, float alpha) {
    for (int i = 0; i < n; i++) {
        data[i] = (data[i] > 0) ? data[i] : alpha * data[i];
    }
}
