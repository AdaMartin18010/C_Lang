/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 4338
 * Language: c
 * Block ID: 1813e3de
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* io.c - C端图像I/O实现 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"

/* 简化的PPM图像加载 */
Image* img_create(size_t width, size_t height, size_t channels) {
    Image* img = malloc(sizeof(Image));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->channels = channels;
    img->data = calloc(width * height * channels, 1);

    if (!img->data) {
        free(img);
        return NULL;
    }

    return img;
}

void img_destroy(Image* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

int img_load(const char* filename, Image** out_image) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return -1;

    /* 简化的P6 PPM格式加载 */
    char magic[3];
    if (fscanf(fp, "%2s", magic) != 1 || strcmp(magic, "P6") != 0) {
        fclose(fp);
        return -1;
    }

    int width, height, maxval;
    if (fscanf(fp, "%d %d %d", &width, &height, &maxval) != 3) {
        fclose(fp);
        return -1;
    }

    fgetc(fp);  /* 消耗换行符 */

    Image* img = img_create(width, height, 3);
    if (!img) {
        fclose(fp);
        return -2;
    }

    if (fread(img->data, 1, width * height * 3, fp) != (size_t)(width * height * 3)) {
        img_destroy(img);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    *out_image = img;
    return 0;
}

int img_save(const char* filename, const Image* image) {
    if (!image || !image->data) return -1;

    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;

    fprintf(fp, "P6\n%zu %zu\n255\n", image->width, image->height);
    fwrite(image->data, 1, image->width * image->height * image->channels, fp);

    fclose(fp);
    return 0;
}
