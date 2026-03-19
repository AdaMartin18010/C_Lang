/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\03_Edge_Detection.md
 * Line: 145
 * Language: c
 * Block ID: 755994dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* canny_edge_detector.h */
#ifndef CANNY_EDGE_DETECTOR_H
#define CANNY_EDGE_DETECTOR_H

#include <stdint.h>
#include <stdbool.h>

/* 图像数据结构 */
typedef struct {
    uint8_t *data;      /* 灰度图像数据 */
    int width;          /* 图像宽度 */
    int height;         /* 图像高度 */
} Image;

/* Canny检测参数 */
typedef struct {
    double sigma;       /* 高斯滤波标准差 */
    double low_thresh;  /* 低阈值 (0.0-1.0) */
    double high_thresh; /* 高阈值 (0.0-1.0) */
} CannyParams;

/* 默认参数 */
#define DEFAULT_SIGMA       1.0
#define DEFAULT_LOW_THRESH  0.05
#define DEFAULT_HIGH_THRESH 0.15
#define GAUSSIAN_KERNEL_SIZE 5

/* 函数声明 */
Image* image_create(int width, int height);
void image_free(Image *img);
Image* image_load_ppm(const char *filename);
bool image_save_ppm(const Image *img, const char *filename);
Image* canny_edge_detect(const Image *src, const CannyParams *params);

#endif /* CANNY_EDGE_DETECTOR_H */
