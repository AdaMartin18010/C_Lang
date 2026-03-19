/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 490
 * Language: c
 * Block ID: 180b8bac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// adas_deterministic_ai.c
#include "arm_math.h"
#include "arm_nnfunctions.h"

// 静态分配的缓冲区
static q7_t sensor_buffer[1920 * 1080 * 3];     // 摄像头输入
static q7_t feature_map_1[480 * 270 * 32];      // 特征图1
static q7_t feature_map_2[120 * 68 * 64];       // 特征图2
static q7_t detection_output[100 * 6];          // 检测结果 [x, y, w, h, class, score]

// 预计算的量化参数
static const int32_t conv1_quant_params[2] = {1565432345, -8};
static const int32_t conv2_quant_params[2] = {1234567890, -7};

// 确定性ADAS推理
int adas_deterministic_inference(const q7_t* camera_frame,
                                 adas_detection_t* detections,
                                 int max_detections) {
    uint32_t start_cycle = DWT_CYCCNT;

    // 1. 预处理 (固定时间: 2ms)
    preprocess_frame(camera_frame, sensor_buffer);

    // 2. Backbone网络 (固定层数，每层固定计算量)
    arm_convolve_s8(&conv1_params, &conv1_quant, &input_dims,
                    sensor_buffer, &filter1_dims, weights_conv1,
                    &bias1_dims, bias_conv1, &output1_dims, feature_map_1);

    arm_max_pool_s8(&pool1_params, &pool1_input_dims, feature_map_1,
                    &pool1_filter_dims, &pool1_output_dims, feature_map_1);

    arm_convolve_s8(&conv2_params, &conv2_quant, &conv2_input_dims,
                    feature_map_1, &filter2_dims, weights_conv2,
                    &bias2_dims, bias_conv2, &conv2_output_dims, feature_map_2);

    // 3. 检测头 (SSD/YOLO风格)
    ssd_detection_head(feature_map_2, detection_output);

    // 4. NMS (非极大值抑制) - 最坏情况有界
    int num_detections = nms_fixed(detection_output, max_detections);

    // 5. 复制结果
    memcpy(detections, detection_output, num_detections * sizeof(adas_detection_t));

    // 验证WCET
    uint32_t elapsed_cycles = DWT_CYCCNT - start_cycle;
    uint32_t elapsed_ms = elapsed_cycles / (SystemCoreClock / 1000);

    if (elapsed_ms > 30) {  // 超过WCET预算
        log_violation("WCET violation: %ums", elapsed_ms);
        return -1;
    }

    return num_detections;
}

// NMS - 最坏情况分析版本
int nms_fixed(const q7_t* raw_detections,
              adas_detection_t* output,
              int max_output,
              float iou_threshold) {

    // 假设最多100个候选框
    #define MAX_CANDIDATES 100

    int num_candidates = parse_detections(raw_detections, MAX_CANDIDATES);
    int num_keep = 0;

    // O(N^2)复杂度，但N有界(100)，所以执行时间有界
    for (int i = 0; i < num_candidates && num_keep < max_output; i++) {
        if (suppressed[i]) continue;

        // 保留这个检测
        copy_detection(&output[num_keep++], &candidates[i]);

        // 抑制重叠的框
        for (int j = i + 1; j < num_candidates; j++) {
            if (!suppressed[j]) {
                float iou = compute_iou(candidates[i], candidates[j]);
                if (iou > iou_threshold) {
                    suppressed[j] = 1;
                }
            }
        }
    }

    return num_keep;  // 最多max_output个
}
