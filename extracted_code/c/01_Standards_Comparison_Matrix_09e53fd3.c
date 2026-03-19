/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 1979
 * Language: c
 * Block ID: 09e53fd3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 信号处理 - 利用C99的复数支持和VLA */
#include <complex.h>
#include <math.h>

void fft(double complex* buf, int n) {
    if (n <= 1) return;

    /* 使用VLA进行原地重排 */
    double complex even[n/2];
    double complex odd[n/2];

    for (int i = 0; i < n/2; i++) {
        even[i] = buf[2*i];
        odd[i] = buf[2*i + 1];
    }

    fft(even, n/2);
    fft(odd, n/2);

    for (int k = 0; k < n/2; k++) {
        double complex t = cexp(-2.0*I*M_PI*k/n) * odd[k];
        buf[k] = even[k] + t;
        buf[k + n/2] = even[k] - t;
    }
}
