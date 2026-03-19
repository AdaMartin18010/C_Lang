/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 4285
 * Language: c
 * Block ID: 964a38d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* image_processor.h - Rust图像处理库C接口 */
#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* 图像结构体 */
typedef struct {
    size_t width;
    size_t height;
    size_t channels;
    uint8_t* data;
} Image;

/* 处理选项 */
typedef struct {
    float brightness;
    float contrast;
    float blur_radius;
    int grayscale;
} ProcessOptions;

/* 核心处理函数 */
int img_adjust_brightness(Image* image, float factor);
int img_to_grayscale(Image* image);
int img_gaussian_blur(Image* image, float radius);
int img_batch_process(Image** images, size_t count, const ProcessOptions* options);

/* 工具函数 */
const char* img_get_version(void);
const char* img_get_error_message(int code);

/* C端辅助函数 */
Image* img_create(size_t width, size_t height, size_t channels);
void img_destroy(Image* image);
int img_load(const char* filename, Image** out_image);
int img_save(const char* filename, const Image* image);

#ifdef __cplusplus
}
#endif

#endif
