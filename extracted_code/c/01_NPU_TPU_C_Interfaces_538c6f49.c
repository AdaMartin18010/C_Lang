/*
 * Auto-generated from: 16_Embedded_AI_Agents\07_Hardware_Acceleration\01_NPU_TPU_C_Interfaces.md
 * Line: 91
 * Language: c
 * Block ID: 538c6f49
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ethos_u_driver.c
#include "ethos_u.h"
#include "pmu_ethosu.h"

// Ethos-U设备句柄
typedef struct {
    struct ethosu_driver drv;       // 底层驱动
    void* cmd_data;                  // 命令流地址
    void* weight_data;               // 权重地址
    void* scratch_data;              // 临时缓冲区
} ethos_u_ctx_t;

// 初始化Ethos-U
int ethos_u_init(ethos_u_ctx_t* ctx,
                void* cmd_base,
                void* weight_base,
                void* scratch_base,
                size_t scratch_size) {

    ctx->cmd_data = cmd_base;
    ctx->weight_data = weight_base;
    ctx->scratch_data = scratch_base;

    // 配置NPU
    struct ethosu_config cfg = {
        .queue_size = 1,
        .custom_dma = NULL,
    };

    // 初始化驱动
    int status = ethosu_init(&ctx->drv,
                            ETHOS_U_BASE_ADDR,  // 0x48102000
                            cmd_base,
                            weight_base,
                            scratch_base,
                            scratch_size,
                            &cfg);

    if (status != 0) {
        printf("Ethos-U init failed: %d\n", status);
        return -1;
    }

    printf("Ethos-U initialized successfully\n");
    printf("  Version: %d.%d.%d\n",
           ethosu_get_version(&ctx->drv) >> 16,
           (ethosu_get_version(&ctx->drv) >> 8) & 0xFF,
           ethosu_get_version(&ctx->drv) & 0xFF);

    return 0;
}

// 执行推理
int ethos_u_inference(ethos_u_ctx_t* ctx,
                     const void* input_data,
                     void* output_data,
                     size_t input_size,
                     size_t output_size) {

    // 准备输入数据 (DMA传输到NPU可访问内存)
    void* input_npu = ethosu_map_input(&ctx->drv, input_size);
    memcpy(input_npu, input_data, input_size);
    ethosu_flush_input(&ctx->drv, input_size);

    // 创建推理任务
    struct ethosu_inference_job job = {
        .input = input_npu,
        .output = ethosu_map_output(&ctx->drv, output_size),
        .custom_data = NULL,
    };

    // 提交到NPU执行
    int status = ethosu_schedule_inference(&ctx->drv, &job);
    if (status != 0) {
        return -1;
    }

    // 等待完成 (可配置为中断或轮询)
    #ifdef ETHOS_U_INTERRUPT_MODE
    // 等待中断信号
    ethosu_wait_for_completion(&ctx->drv, &job);
    #else
    // 轮询模式
    while (!ethosu_is_inference_done(&ctx->drv, &job)) {
        __WFI();  // 等待中断
    }
    #endif

    // 获取输出
    ethosu_invalidate_output(&ctx->drv, output_size);
    memcpy(output_data, job.output, output_size);

    return 0;
}

// 获取性能计数器
void ethos_u_get_perf_counters(ethos_u_ctx_t* ctx,
                              uint32_t* cycles,
                              uint32_t* mac_count,
                              uint32_t* dma_stalls) {
    struct ethosu_pmu_counters counters;
    ethosu_get_pmu_counters(&ctx->drv, &counters);

    *cycles = counters.cycle_count;
    *mac_count = counters.event_count[ETHOSU_PMU_NPU_ACTIVE];
    *dma_stalls = counters.event_count[ETHOSU_PMU_SRAM_ACCESS_STALL];
}

// 完整使用示例
void ethos_u_example(void) {
    // 从Vela编译器生成的文件加载
    extern const uint8_t model_cmd_data[];      // 命令流
    extern const uint8_t model_weight_data[];   // 权重

    ethos_u_ctx_t ctx;

    // 分配scratch内存 (通常256KB-512KB)
    static uint8_t scratch_buffer[512 * 1024] __attribute__((aligned(16)));

    // 初始化
    ethos_u_init(&ctx,
                (void*)model_cmd_data,
                (void*)model_weight_data,
                scratch_buffer,
                sizeof(scratch_buffer));

    // 输入数据
    int8_t input[224 * 224 * 3];
    int8_t output[1000];  // 分类结果

    // 准备输入 (量化等)
    prepare_input(input);

    // 执行推理
    uint32_t start = get_cycle_count();
    ethos_u_inference(&ctx, input, output, sizeof(input), sizeof(output));
    uint32_t cycles = get_cycle_count() - start;

    printf("Inference completed in %u cycles\n", cycles);
    printf("  Equivalent to %.2f ms @ 480MHz\n",
           cycles / 480000.0f);

    // 获取性能数据
    uint32_t npu_cycles, mac_ops, dma_stall;
    ethos_u_get_perf_counters(&ctx, &npu_cycles, &mac_ops, &dma_stall);

    printf("NPU MAC utilization: %.1f%%\n",
           100.0f * mac_ops / npu_cycles);
}
