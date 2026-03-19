/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 438
 * Language: c
 * Block ID: b53e91f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void convolve2d_scalar(float *input, float *output, float *kernel,
                       int width, int height, int ksize) {
    int pad = ksize / 2;
    for (int y = pad; y < height - pad; y++) {
        for (int x = pad; x < width - pad; x++) {
            float sum = 0;
            for (int ky = 0; ky < ksize; ky++) {
                for (int kx = 0; kx < ksize; kx++) {
                    float pixel = input[(y + ky - pad) * width + (x + kx - pad)];
                    float weight = kernel[ky * ksize + kx];
                    sum += pixel * weight;
                }
            }
            output[y * width + x] = sum;
        }
    }
}
