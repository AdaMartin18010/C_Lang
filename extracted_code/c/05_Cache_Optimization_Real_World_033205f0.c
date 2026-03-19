/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 780
 * Language: c
 * Block ID: 033205f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始实现 - 多次遍历
void image_pipeline_bad(const uint8_t* input, uint8_t* output,
                        int width, int height) {
    // 分配中间缓冲区
    uint8_t* temp1 = malloc(width * height);
    uint8_t* temp2 = malloc(width * height);
    uint8_t* temp3 = malloc(width * height);

    // 每个步骤遍历整张图像
    gaussian_blur(input, temp1, width, height);    // 读取1x，写入1x
    edge_detect(temp1, temp2, width, height);      // 读取1x，写入1x
    sharpen(temp2, temp3, width, height);          // 读取1x，写入1x
    color_adjust(temp3, output, width, height);    // 读取1x，写入1x

    // 总内存流量: 4读 + 4写 = 8x图像大小

    free(temp1);
    free(temp2);
    free(temp3);
}

// 优化实现 - 分块处理，缓存驻留
void image_pipeline_optimized(const uint8_t* input, uint8_t* output,
                               int width, int height) {
    // 选择块大小使所有数据在L2缓存中
    const int TILE_SIZE = 256;  // 256x256 = 64KB (适合L2)

    // 小块中间缓冲区
    uint8_t tile1[TILE_SIZE * TILE_SIZE];
    uint8_t tile2[TILE_SIZE * TILE_SIZE];
    uint8_t tile3[TILE_SIZE * TILE_SIZE];

    for (int ty = 0; ty < height; ty += TILE_SIZE) {
        for (int tx = 0; tx < width; tx += TILE_SIZE) {
            int tile_h = (ty + TILE_SIZE < height) ? TILE_SIZE : height - ty;
            int tile_w = (tx + TILE_SIZE < width) ? TILE_SIZE : width - tx;

            // 处理一个块 (所有数据在缓存中)
            load_tile(input, tile1, tx, ty, tile_w, tile_h, width);
            gaussian_blur_tile(tile1, tile2, tile_w, tile_h);
            edge_detect_tile(tile2, tile3, tile_w, tile_h);
            sharpen_tile(tile3, tile1, tile_w, tile_h);
            color_adjust_tile(tile1, output, tx, ty, tile_w, tile_h, width);
        }
    }

    // 总内存流量: 1读 + 1写 = 2x图像大小
}

/* 性能对比 (4K图像 3840x2160):
 * 原始:    45 ms,  带宽: 12 GB/s
 * 优化:    12 ms,  带宽: 5 GB/s (更少数据移动)
 * 加速比:  3.75x
 */
