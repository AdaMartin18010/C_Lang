/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 342
 * Language: c
 * Block ID: bfb2fe0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
