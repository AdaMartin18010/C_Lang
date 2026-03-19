/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 4422
 * Language: c
 * Block ID: 2f94acf2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* main.c - 图像处理程序主入口 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"

void print_usage(const char* program) {
    printf("Usage: %s <input.ppm> <output.ppm> [options]\n", program);
    printf("Options:\n");
    printf("  -b <factor>  Brightness adjustment (0.0-2.0, default: 1.0)\n");
    printf("  -g           Convert to grayscale\n");
    printf("  -r <radius>  Gaussian blur radius (default: 0.0)\n");
    printf("  -v           Show version\n");
    printf("  -h           Show help\n");
}

int main(int argc, char** argv) {
    printf("Image Processor v%s\n", img_get_version());
    printf("Powered by Rust + C\n\n");

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    /* 解析选项 */
    ProcessOptions opts = {
        .brightness = 1.0f,
        .contrast = 1.0f,
        .blur_radius = 0.0f,
        .grayscale = 0
    };

    int input_idx = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            return 0;
        }
        if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            opts.brightness = atof(argv[++i]);
        }
        if (strcmp(argv[i], "-g") == 0) {
            opts.grayscale = 1;
        }
        if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            opts.blur_radius = atof(argv[++i]);
        }
    }

    /* 查找输入和输出文件 */
    if (argc < 3 || argv[argc-2][0] == '-' || argv[argc-1][0] == '-') {
        printf("Error: Missing input or output file\n");
        return 1;
    }

    const char* input_file = argv[argc-2];
    const char* output_file = argv[argc-1];

    /* 加载图像 */
    printf("Loading: %s\n", input_file);
    Image* image = NULL;
    int result = img_load(input_file, &image);

    if (result != 0) {
        printf("Error loading image: %s\n", img_get_error_message(result));
        return 1;
    }

    printf("Image: %zux%zu, %zu channels\n",
           image->width, image->height, image->channels);

    /* 应用处理 */
    printf("Processing...\n");

    Image* images[] = { image };
    result = img_batch_process(images, 1, &opts);

    if (result != 0) {
        printf("Error processing: %s\n", img_get_error_message(result));
        img_destroy(image);
        return 1;
    }

    /* 保存结果 */
    printf("Saving: %s\n", output_file);
    result = img_save(output_file, image);

    if (result != 0) {
        printf("Error saving image\n");
        img_destroy(image);
        return 1;
    }

    /* 清理 */
    img_destroy(image);

    printf("Done!\n");
    return 0;
}
