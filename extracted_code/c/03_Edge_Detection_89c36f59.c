/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\03_Edge_Detection.md
 * Line: 185
 * Language: c
 * Block ID: 89c36f59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ppm_io.c - PPM格式图像读写 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canny_edge_detector.h"

/* 跳过PPM文件中的注释 */
static void skip_comments(FILE *fp)
{
    int c;
    while ((c = fgetc(fp)) == '#') {
        while ((c = fgetc(fp)) != '\n' && c != EOF);
    }
    ungetc(c, fp);
}

/* 加载PPM图像(P6格式) */
Image* image_load_ppm(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "无法打开文件: %s\n", filename);
        return NULL;
    }

    /* 读取魔数 */
    char magic[3];
    if (fscanf(fp, "%2s", magic) != 1 || magic[0] != 'P' ||
        (magic[1] != '6' && magic[1] != '5')) {
        fprintf(stderr, "不支持的PPM格式: %s\n", magic);
        fclose(fp);
        return NULL;
    }

    /* 跳过空白和注释 */
    skip_comments(fp);

    /* 读取宽度、高度 */
    int width, height, maxval;
    if (fscanf(fp, "%d %d", &width, &height) != 2) {
        fprintf(stderr, "读取图像尺寸失败\n");
        fclose(fp);
        return NULL;
    }

    skip_comments(fp);

    /* 读取最大像素值 */
    if (fscanf(fp, "%d", &maxval) != 1 || maxval > 255) {
        fprintf(stderr, "不支持的像素值范围\n");
        fclose(fp);
        return NULL;
    }

    /* 跳到图像数据 */
    fgetc(fp);

    /* 创建图像 */
    Image *img = image_create(width, height);
    if (!img) {
        fclose(fp);
        return NULL;
    }

    /* 读取像素数据 */
    if (magic[1] == '6') {
        /* P6: RGB格式，转换为灰度 */
        uint8_t *rgb = malloc(width * height * 3);
        if (!rgb || fread(rgb, 3, width * height, fp) != (size_t)(width * height)) {
            fprintf(stderr, "读取RGB数据失败\n");
            free(rgb);
            image_free(img);
            fclose(fp);
            return NULL;
        }
        /* RGB转灰度 */
        for (int i = 0; i < width * height; i++) {
            img->data[i] = (uint8_t)(0.299 * rgb[i * 3] +
                                     0.587 * rgb[i * 3 + 1] +
                                     0.114 * rgb[i * 3 + 2]);
        }
        free(rgb);
    } else {
        /* P5: 灰度格式 */
        if (fread(img->data, 1, width * height, fp) != (size_t)(width * height)) {
            fprintf(stderr, "读取灰度数据失败\n");
            image_free(img);
            fclose(fp);
            return NULL;
        }
    }

    fclose(fp);
    return img;
}

/* 保存PPM图像 */
bool image_save_ppm(const Image *img, const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "无法创建文件: %s\n", filename);
        return false;
    }

    /* 写入P5格式(灰度) */
    fprintf(fp, "P5\n%d %d\n255\n", img->width, img->height);
    fwrite(img->data, 1, img->width * img->height, fp);

    fclose(fp);
    return true;
}

/* 创建图像 */
Image* image_create(int width, int height)
{
    Image *img = malloc(sizeof(Image));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->data = calloc((size_t)width * height, sizeof(uint8_t));

    if (!img->data) {
        free(img);
        return NULL;
    }
    return img;
}

/* 释放图像 */
void image_free(Image *img)
{
    if (img) {
        free(img->data);
        free(img);
    }
}
