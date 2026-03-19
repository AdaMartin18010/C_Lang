/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\03_Edge_Detection.md
 * Line: 542
 * Language: c
 * Block ID: 2351c17c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* main.c - 示例程序 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "canny_edge_detector.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("用法: %s <输入PPM图像> [sigma] [low_thresh] [high_thresh]\n", argv[0]);
        printf("  sigma: 高斯滤波标准差 (默认: 1.0)\n");
        printf("  low_thresh: 低阈值 (默认: 0.05)\n");
        printf("  high_thresh: 高阈值 (默认: 0.15)\n");
        return 1;
    }

    /* 解析参数 */
    CannyParams params = {
        .sigma = (argc > 2) ? atof(argv[2]) : DEFAULT_SIGMA,
        .low_thresh = (argc > 3) ? atof(argv[3]) : DEFAULT_LOW_THRESH,
        .high_thresh = (argc > 4) ? atof(argv[4]) : DEFAULT_HIGH_THRESH
    };

    printf("Canny边缘检测参数:\n");
    printf("  Sigma: %.2f\n", params.sigma);
    printf("  低阈值: %.3f\n", params.low_thresh);
    printf("  高阈值: %.3f\n", params.high_thresh);

    /* 加载图像 */
    printf("\n加载图像: %s\n", argv[1]);
    clock_t start = clock();

    Image *src = image_load_ppm(argv[1]);
    if (!src) {
        fprintf(stderr, "图像加载失败\n");
        return 1;
    }

    printf("图像尺寸: %d x %d\n", src->width, src->height);

    /* 执行Canny边缘检测 */
    printf("\n执行Canny边缘检测...\n");
    Image *edges = canny_edge_detect(src, &params);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    if (!edges) {
        fprintf(stderr, "边缘检测失败\n");
        image_free(src);
        return 1;
    }

    /* 保存结果 */
    const char *output_name = "edges_output.ppm";
    if (image_save_ppm(edges, output_name)) {
        printf("边缘图像已保存: %s\n", output_name);
    }

    printf("处理时间: %.3f秒\n", elapsed);
    printf("处理速度: %.2f MP/s\n",
           (src->width * src->height / 1000000.0) / elapsed);

    /* 清理 */
    image_free(src);
    image_free(edges);

    return 0;
}
