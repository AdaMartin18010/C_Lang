/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_EDAC_Memory.md
 * Line: 291
 * Language: c
 * Block ID: ef73045e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 内存Scrubbing - 后台错误清除
// ============================================================================

#define SCRUB_INTERVAL_MS       100     // Scrub间隔
#define SCRUB_BATCH_SIZE        64      // 每次处理的字数

typedef struct {
    EDACMemory *mem;
    size_t scrub_position;
    uint32_t last_scrub_time;
    bool scrub_in_progress;
    uint32_t errors_corrected;
} Scrubber;

// 初始化Scrubber
void scrubber_init(Scrubber *scrub, EDACMemory *mem) {
    scrub->mem = mem;
    scrub->scrub_position = 0;
    scrub->last_scrub_time = 0;
    scrub->scrub_in_progress = false;
    scrub->errors_corrected = 0;
}

// 执行一次Scrub周期
void scrubber_step(Scrubber *scrub, uint32_t current_time_ms) {
    if (current_time_ms - scrub->last_scrub_time < SCRUB_INTERVAL_MS) {
        return;
    }

    scrub->last_scrub_time = current_time_ms;
    scrub->scrub_in_progress = true;

    // 处理一批内存
    size_t end_pos = scrub->scrub_position + SCRUB_BATCH_SIZE;
    if (end_pos > scrub->mem->size) {
        end_pos = scrub->mem->size;
    }

    for (size_t addr = scrub->scrub_position; addr < end_pos; addr++) {
        uint64_t data;
        EDACResult result = edac_read(scrub->mem, addr, &data);

        if (result == EDAC_SINGLE_ERROR_CORRECTED) {
            scrub->errors_corrected++;
        } else if (result == EDAC_DOUBLE_ERROR_DETECTED) {
            // 记录不可纠正错误
            log_uncorrectable_error(addr);
        }
    }

    // 更新位置
    scrub->scrub_position = end_pos;
    if (scrub->scrub_position >= scrub->mem->size) {
        scrub->scrub_position = 0;  // 循环
    }

    scrub->scrub_in_progress = false;
}

// 紧急Scrub (在空闲时执行更多)
void scrubber_emergency(Scrubber *scrub) {
    // 处理整个内存
    for (size_t addr = 0; addr < scrub->mem->size; addr++) {
        uint64_t data;
        edac_read(scrub->mem, addr, &data);
    }
    scrub->scrub_position = 0;
}
