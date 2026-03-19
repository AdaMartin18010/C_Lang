/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 2185
 * Language: c
 * Block ID: 28641f7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// RP2040 双核协作示例
// 核心0: 通信 + 控制
// 核心1: 实时数据处理
// ============================================================

#include "pico/multicore.h"
#include "hardware/irq.h"
#include "hardware/sync.h"

// 共享数据 (需要互斥保护)
typedef struct {
    int32_t sensor_value;
    uint32_t timestamp;
    bool data_ready;
} SharedData;

static SharedData shared_data;
static spin_lock_t *data_lock;

// 核心1入口
void core1_entry(void) {
    // 核心1初始化
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    while (1) {
        // 高频率采样
        int32_t sum = 0;
        for (int i = 0; i < 100; i++) {
            sum += adc_read();
            tight_loop_contents();
        }
        int32_t avg = sum / 100;

        // 写入共享数据 (加锁)
        uint32_t save = spin_lock_blocking(data_lock);
        shared_data.sensor_value = avg;
        shared_data.timestamp = time_us_32();
        shared_data.data_ready = true;
        spin_unlock(data_lock, save);

        // 通知核心0 (通过FIFO)
        multicore_fifo_push_blocking(0x01);
    }
}

// 核心0主程序
int main(void) {
    stdio_init_all();

    // 创建互斥锁
    data_lock = spin_lock_instance(spin_lock_claim_unused(true));

    // 启动核心1
    multicore_launch_core1(core1_entry);

    while (1) {
        // 等待核心1通知
        uint32_t msg = multicore_fifo_pop_blocking();

        if (msg == 0x01) {
            // 读取共享数据
            uint32_t save = spin_lock_blocking(data_lock);
            int32_t value = shared_data.sensor_value;
            uint32_t ts = shared_data.timestamp;
            shared_data.data_ready = false;
            spin_unlock(data_lock, save);

            // 处理并上传
            printf("Value: %ld at %u\n", value, ts);
        }
    }
}
