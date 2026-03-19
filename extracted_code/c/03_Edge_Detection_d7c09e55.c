/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\03_Edge_Detection.md
 * Line: 78
 * Language: c
 * Block ID: d7c09e55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 生成高斯滤波核 */
static void generate_gaussian_kernel(double *kernel, int size, double sigma)
{
    int half_size = size / 2;
    double sum = 0.0;

    for (int i = -half_size; i <= half_size; i++) {
        for (int j = -half_size; j <= half_size; j++) {
            double value = (1.0 / (2.0 * M_PI * sigma * sigma)) *
                          exp(-(i * i + j * j) / (2.0 * sigma * sigma));
            kernel[(i + half_size) * size + (j + half_size)] = value;
            sum += value;
        }
    }

    /* 归一化 */
    for (int i = 0; i < size * size; i++) {
        kernel[i] /= sum;
    }
}
