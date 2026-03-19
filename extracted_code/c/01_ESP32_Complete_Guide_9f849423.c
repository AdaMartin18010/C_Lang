/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 3950
 * Language: c
 * Block ID: 9f849423
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * performance_tips.c
 * ESP32 性能优化技巧
 */

/**
 * @brief IRAM_ATTR 修饰关键函数
 *
 * 将频繁调用的函数放在 IRAM (指令 RAM) 中,
 * 避免从 Flash 执行时的缓存未命中延迟
 */
static void IRAM_ATTR critical_function(void)
{
    /* 高频调用的代码 */
}

/**
 * @brief 使用 DMA 缓冲区对齐
 */
static void dma_buffer_alignment(void)
{
    /* DMA 缓冲区需要 4 字节对齐 */
    static DRAM_ATTR uint8_t dma_buffer[1024] __attribute__((aligned(4)));

    /* 或使用 heap_caps_malloc */
    uint8_t *aligned_buf = heap_caps_malloc(1024, MALLOC_CAP_DMA);

    /* 使用后释放 */
    free(aligned_buf);
}

/**
 * @brief 零拷贝技术
 */
static void zero_copy_technique(void)
{
    /* 使用 ringbuffer 避免数据复制 */
    RingbufHandle_t ringbuf = xRingbufferCreate(2048, RINGBUF_TYPE_BYTEBUF);

    /* 发送: 直接发送数据指针 */
    uint8_t data[100];
    xRingbufferSend(ringbuf, data, sizeof(data), portMAX_DELAY);

    /* 接收: 获取数据指针,无需复制 */
    size_t item_size;
    uint8_t *item = (uint8_t *)xRingbufferReceive(ringbuf, &item_size, portMAX_DELAY);

    /* 使用数据 */
    process_data(item, item_size);

    /* 释放 (不释放内存,只是归还 ringbuffer) */
    vRingbufferReturnItem(ringbuf, item);
}

/**
 * @brief 使用 PSRAM 扩展内存
 */
static void use_psram(void)
{
    /* 检查 PSRAM 是否可用 */
    if (esp_psram_is_initialized()) {
        /* 分配 PSRAM 内存 */
        uint8_t *psram_buf = heap_caps_malloc(1024 * 1024, MALLOC_CAP_SPIRAM);

        /* 大型缓冲区、图像数据等可以放在 PSRAM */
        ESP_LOGI(TAG, "使用 PSRAM: %p", psram_buf);

        free(psram_buf);
    }
}

/**
 * @brief 编译优化设置
 */
/* CMakeLists.txt 中添加:
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# 或针对特定文件优化
set_source_files_properties(fast_code.c PROPERTIES COMPILE_FLAGS "-O3")
*/
