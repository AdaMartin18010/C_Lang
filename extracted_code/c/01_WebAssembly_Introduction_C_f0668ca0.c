/*
 * Auto-generated from: 10_WebAssembly_C\01_WebAssembly_Introduction_C.md
 * Line: 2144
 * Language: c
 * Block ID: f0668ca0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// image_processor.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <string.h>

// 图像结构体
typedef struct {
    unsigned char* data;
    int width;
    int height;
    int channels;
} Image;

// 在 WASM 内存中分配图像
EMSCRIPTEN_KEEPALIVE
Image* create_image(int width, int height, int channels) {
    Image* img = (Image*)malloc(sizeof(Image));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->channels = channels;
    img->data = (unsigned char*)malloc(width * height * channels);

    if (!img->data) {
        free(img);
        return NULL;
    }

    return img;
}

// 释放图像
EMSCRIPTEN_KEEPALIVE
void destroy_image(Image* img) {
    if (img) {
        if (img->data) free(img->data);
        free(img);
    }
}

// 获取图像数据指针
EMSCRIPTEN_KEEPALIVE
unsigned char* get_image_data(Image* img) {
    return img ? img->data : NULL;
}

// 灰度转换
EMSCRIPTEN_KEEPALIVE
void grayscale(Image* src, Image* dst) {
    if (!src || !dst || src->width != dst->width || src->height != dst->height) return;

    int pixelCount = src->width * src->height;

    for (int i = 0; i < pixelCount; i++) {
        int idx = i * src->channels;
        unsigned char r = src->data[idx];
        unsigned char g = src->data[idx + 1];
        unsigned char b = src->data[idx + 2];

        // 加权灰度: 0.299*R + 0.587*G + 0.114*B
        unsigned char gray = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);

        dst->data[i] = gray;
    }
}

// 亮度调整
EMSCRIPTEN_KEEPALIVE
void adjust_brightness(Image* img, int brightness) {
    if (!img) return;

    int size = img->width * img->height * img->channels;

    for (int i = 0; i < size; i++) {
        int val = img->data[i] + brightness;
        img->data[i] = (unsigned char)(val < 0 ? 0 : (val > 255 ? 255 : val));
    }
}

// 对比度调整
EMSCRIPTEN_KEEPALIVE
void adjust_contrast(Image* img, float contrast) {
    if (!img) return;

    int size = img->width * img->height * img->channels;
    float factor = (259.0f * (contrast + 255.0f)) / (255.0f * (259.0f - contrast));

    for (int i = 0; i < size; i++) {
        float val = factor * (img->data[i] - 128) + 128;
        img->data[i] = (unsigned char)(val < 0 ? 0 : (val > 255 ? 255 : val));
    }
}

// 高斯模糊 (简化版 - 3x3 核)
EMSCRIPTEN_KEEPALIVE
void blur(Image* src, Image* dst) {
    if (!src || !dst || src->width != dst->width || src->height != dst->height) return;

    float kernel[3][3] = {
        {1.0f/16, 2.0f/16, 1.0f/16},
        {2.0f/16, 4.0f/16, 2.0f/16},
        {1.0f/16, 2.0f/16, 1.0f/16}
    };

    int w = src->width;
    int h = src->height;
    int c = src->channels;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            for (int ch = 0; ch < c; ch++) {
                float sum = 0;
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int idx = ((y + ky) * w + (x + kx)) * c + ch;
                        sum += src->data[idx] * kernel[ky + 1][kx + 1];
                    }
                }
                int dstIdx = (y * w + x) * c + ch;
                dst->data[dstIdx] = (unsigned char)sum;
            }
        }
    }
}

// 边缘检测 (Sobel 算子)
EMSCRIPTEN_KEEPALIVE
void sobel_edge_detect(Image* src, Image* dst) {
    if (!src || !dst || src->width != dst->width || src->height != dst->height) return;

    int w = src->width;
    int h = src->height;

    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int sumX = 0, sumY = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int idx = ((y + ky) * w + (x + kx)) * src->channels;
                    unsigned char gray = (src->data[idx] + src->data[idx + 1] + src->data[idx + 2]) / 3;
                    sumX += gray * gx[ky + 1][kx + 1];
                    sumY += gray * gy[ky + 1][kx + 1];
                }
            }

            int magnitude = (int)sqrt(sumX * sumX + sumY * sumY);
            if (magnitude > 255) magnitude = 255;

            int dstIdx = (y * w + x);
            dst->data[dstIdx] = (unsigned char)magnitude;
        }
    }
}

// 颜色反转
EMSCRIPTEN_KEEPALIVE
void invert(Image* img) {
    if (!img) return;

    int size = img->width * img->height * img->channels;
    for (int i = 0; i < size; i++) {
        img->data[i] = 255 - img->data[i];
    }
}

// 图像缩略图生成 (最近邻插值)
EMSCRIPTEN_KEEPALIVE
void resize_nearest(Image* src, Image* dst, int newWidth, int newHeight) {
    if (!src || !dst) return;

    float xRatio = (float)src->width / newWidth;
    float yRatio = (float)src->height / newHeight;
    int c = src->channels;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int srcX = (int)(x * xRatio);
            int srcY = (int)(y * yRatio);

            int srcIdx = (srcY * src->width + srcX) * c;
            int dstIdx = (y * newWidth + x) * c;

            for (int ch = 0; ch < c; ch++) {
                dst->data[dstIdx + ch] = src->data[srcIdx + ch];
            }
        }
    }
}

// 获取图像尺寸信息
EMSCRIPTEN_KEEPALIVE
int get_image_width(Image* img) { return img ? img->width : 0; }
EMSCRIPTEN_KEEPALIVE
int get_image_height(Image* img) { return img ? img->height : 0; }
EMSCRIPTEN_KEEPALIVE
int get_image_channels(Image* img) { return img ? img->channels : 0; }
