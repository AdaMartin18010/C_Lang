/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 2503
 * Language: c
 * Block ID: bb9457cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// image_memory.c
#include <emscripten/emscripten.h>
#include <stdlib.h>

// 图像缓冲区管理
#define MAX_IMAGE_CACHE 10

typedef struct {
    unsigned char* data;
    int width;
    int height;
    int channels;
    int ref_count;
    unsigned int last_used;
} ImageBuffer;

static ImageBuffer image_cache[MAX_IMAGE_CACHE];
static unsigned int frame_counter = 0;

// 智能图像分配
EMSCRIPTEN_KEEPALIVE
ImageBuffer* image_allocate(int width, int height, int channels) {
    size_t size = width * height * channels;

    // 查找可重用的缓冲区
    for (int i = 0; i < MAX_IMAGE_CACHE; i++) {
        if (image_cache[i].ref_count == 0 &&
            image_cache[i].width * image_cache[i].height * image_cache[i].channels >= size) {
            // 重用此缓冲区
            image_cache[i].ref_count = 1;
            image_cache[i].width = width;
            image_cache[i].height = height;
            image_cache[i].channels = channels;
            image_cache[i].last_used = frame_counter;
            return &image_cache[i];
        }
    }

    // 分配新缓冲区
    for (int i = 0; i < MAX_IMAGE_CACHE; i++) {
        if (image_cache[i].data == NULL) {
            image_cache[i].data = (unsigned char*)malloc(size);
            if (!image_cache[i].data) return NULL;

            image_cache[i].width = width;
            image_cache[i].height = height;
            image_cache[i].channels = channels;
            image_cache[i].ref_count = 1;
            image_cache[i].last_used = frame_counter;
            return &image_cache[i];
        }
    }

    // 缓存满，清理最旧的
    int oldest = 0;
    for (int i = 1; i < MAX_IMAGE_CACHE; i++) {
        if (image_cache[i].ref_count == 0 &&
            image_cache[i].last_used < image_cache[oldest].last_used) {
            oldest = i;
        }
    }

    if (image_cache[oldest].ref_count == 0) {
        free(image_cache[oldest].data);
        image_cache[oldest].data = (unsigned char*)malloc(size);
        if (!image_cache[oldest].data) return NULL;

        image_cache[oldest].width = width;
        image_cache[oldest].height = height;
        image_cache[oldest].channels = channels;
        image_cache[oldest].ref_count = 1;
        image_cache[oldest].last_used = frame_counter;
        return &image_cache[oldest];
    }

    return NULL;
}

EMSCRIPTEN_KEEPALIVE
void image_retain(ImageBuffer* img) {
    if (img) img->ref_count++;
}

EMSCRIPTEN_KEEPALIVE
void image_release(ImageBuffer* img) {
    if (img && --img->ref_count == 0) {
        // 不立即释放，保留在缓存中
        img->last_used = frame_counter;
    }
}

EMSCRIPTEN_KEEPALIVE
void image_cache_update() {
    frame_counter++;

    // 可选：清理长期未使用的缓冲区
    for (int i = 0; i < MAX_IMAGE_CACHE; i++) {
        if (image_cache[i].ref_count == 0 &&
            image_cache[i].data &&
            frame_counter - image_cache[i].last_used > 60) {
            free(image_cache[i].data);
            image_cache[i].data = NULL;
        }
    }
}
