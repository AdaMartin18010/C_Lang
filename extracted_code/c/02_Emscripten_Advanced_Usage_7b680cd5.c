/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 2861
 * Language: c
 * Block ID: 7b680cd5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// image_processor.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <math.h>

// 完整图像处理库（见前文）
// ... (已在前面的章节中定义)

// 批量处理接口
EMSCRIPTEN_KEEPALIVE
void batch_process_images(
    unsigned char** images,
    int* widths,
    int* heights,
    int count,
    int operation
) {
    for (int i = 0; i < count; i++) {
        Image* img = create_image_from_data(images[i], widths[i], heights[i], 4);

        switch (operation) {
            case 0: grayscale(img); break;
            case 1: blur(img); break;
            case 2: invert(img); break;
            case 3: edge_detect(img); break;
        }

        destroy_image(img);
    }
}
