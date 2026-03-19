/*
 * Auto-generated from: 03_System_Technology_Domains\03_Edge_Detection.md
 * Line: 540
 * Language: c
 * Block ID: d4286950
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自适应阈值计算
float compute_auto_threshold(const float *magnitude, int size, float ratio) {
    // 使用直方图确定高阈值
    int hist[256] = {0};
    for (int i = 0; i < size; i++) {
        int bin = (int)(magnitude[i] / 255.0f * 255);
        if (bin > 255) bin = 255;
        hist[bin]++;
    }

    // 找到ratio分位数
    int cumsum = 0;
    int threshold_idx = 0;
    for (int i = 255; i >= 0; i--) {
        cumsum += hist[i];
        if (cumsum > size * ratio) {
            threshold_idx = i;
            break;
        }
    }

    return threshold_idx * 255.0f / 256.0f;
}
