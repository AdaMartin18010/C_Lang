# 实时AI: 确定性推理与WCET分析

> **文档状态**: 🆕 重要补充
> **创建日期**: 2026-03-18
> **适用领域**: 汽车、工业控制、航空电子

---

## 概述

在安全关键系统（如汽车ABS、飞行控制）中部署AI，不仅需要**准确性**，更需要**确定性**和**实时性**。传统深度学习推理存在执行时间波动，这对硬实时系统是不可接受的。

### 实时AI的挑战

| 挑战 | 传统AI | 实时AI要求 |
|:-----|:-------|:-----------|
| **执行时间** | 变化大 | 有界、可预测 |
| **内存分配** | 动态 | 完全静态 |
| **缓存行为** | 不确定 | 可预测 |
| **浮点运算** | 非确定性 | 确定性 |
| **最坏情况** | 难以分析 | WCET可计算 |

---


---

## 📑 目录

- [实时AI: 确定性推理与WCET分析](#实时ai-确定性推理与wcet分析)
  - [概述](#概述)
    - [实时AI的挑战](#实时ai的挑战)
  - [📑 目录](#-目录)
  - [确定性推理技术](#确定性推理技术)
    - [1. 静态内存分配](#1-静态内存分配)
    - [2. 无分支推理 (Branchless Inference)](#2-无分支推理-branchless-inference)
    - [3. 固定点运算 (Fixed-Point Arithmetic)](#3-固定点运算-fixed-point-arithmetic)
    - [4. 循环展开 (Loop Unrolling)](#4-循环展开-loop-unrolling)
  - [WCET (Worst Case Execution Time) 分析](#wcet-worst-case-execution-time-分析)
    - [1. 基本块计时](#1-基本块计时)
    - [2. 抽象解释 (Abstract Interpretation)](#2-抽象解释-abstract-interpretation)
    - [3. 缓存锁定 (Cache Locking)](#3-缓存锁定-cache-locking)
  - [实时调度集成](#实时调度集成)
    - [1. Rate Monotonic Scheduling (RMS)](#1-rate-monotonic-scheduling-rms)
    - [2. 时间触发架构 (TTA)](#2-时间触发架构-tta)
  - [实际案例: 汽车ADAS实时推理](#实际案例-汽车adas实时推理)
    - [系统架构](#系统架构)
    - [确定性推理实现](#确定性推理实现)
  - [验证与测试](#验证与测试)
    - [1. 执行时间边界测试](#1-执行时间边界测试)
    - [2. 形式化WCET分析工具](#2-形式化wcet分析工具)
  - [参考资源](#参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 确定性推理技术

### 1. 静态内存分配

```c
// ❌ 传统方式 - 动态内存，非确定性
float* buffer = malloc(layer_size * sizeof(float));
if (!buffer) return ERROR;
layer_compute(input, buffer);
free(buffer);

// ✅ 实时方式 - 静态内存池
typedef struct {
    float buffer_0[256 * 256];  // Layer 0 output
    float buffer_1[128 * 128];  // Layer 1 output
    float buffer_2[64 * 64];    // Layer 2 output
    // ... 所有层预分配
} inference_buffers_t;

static inference_buffers_t g_buffers;  // 全局静态分配

int deterministic_inference(const float* input, float* output) {
    // 无malloc/free，执行时间确定
    layer_0_compute(input, g_buffers.buffer_0);
    layer_1_compute(g_buffers.buffer_0, g_buffers.buffer_1);
    layer_2_compute(g_buffers.buffer_1, output);
    return 0;
}
```

### 2. 无分支推理 (Branchless Inference)

```c
// ❌ 条件分支导致执行时间不确定
void relu_branch(float* data, int size) {
    for (int i = 0; i < size; i++) {
        if (data[i] < 0) {       // 分支预测可能失败
            data[i] = 0;
        }
    }
}

// ✅ 无分支实现，恒定执行时间
void relu_branchless(float* data, int size) {
    for (int i = 0; i < size; i++) {
        // 使用条件移动，无分支
        data[i] = (data[i] > 0) ? data[i] : 0;
        // 或位运算技巧
        // int32_t mask = ~((int32_t)data[i] >> 31);
        // data[i] = data[i] * mask;
    }
}

// ✅ SIMD无分支实现 (ARM NEON)
#include "arm_neon.h"

void relu_neon(float32_t* data, int size) {
    float32x4_t zero = vdupq_n_f32(0.0f);

    int i = 0;
    for (; i <= size - 4; i += 4) {
        float32x4_t vec = vld1q_f32(&data[i]);
        vec = vmaxq_f32(vec, zero);  // 无分支最大值
        vst1q_f32(&data[i], vec);
    }

    // 剩余元素
    for (; i < size; i++) {
        data[i] = (data[i] > 0) ? data[i] : 0;
    }
}
```

### 3. 固定点运算 (Fixed-Point Arithmetic)

```c
// 定点数类型定义
typedef int16_t q15_t;   // Q15格式: 1.15 (1符号位, 15小数位)
typedef int32_t q31_t;   // Q31格式: 1.31
typedef int8_t q7_t;     // Q7格式: 1.7

// Q15乘法 ( saturating multiplication )
static inline q15_t q15_mul(q15_t a, q15_t b) {
    // (a * b) >> 15，带饱和
    int32_t temp = (int32_t)a * (int32_t)b;
    temp = (temp + (1 << 14)) >> 15;  // 四舍五入

    // 饱和处理
    if (temp > INT16_MAX) return INT16_MAX;
    if (temp < INT16_MIN) return INT16_MIN;
    return (q15_t)temp;
}

// Q15加法 ( saturating addition )
static inline q15_t q15_add(q15_t a, q15_t b) {
    int32_t temp = (int32_t)a + (int32_t)b;
    if (temp > INT16_MAX) return INT16_MAX;
    if (temp < INT16_MIN) return INT16_MIN;
    return (q15_t)temp;
}

// 定点数矩阵乘法 (确定性执行)
void mat_mul_fixed(const q15_t* a, const q15_t* b, q15_t* c,
                   int m, int n, int k) {
    // m x k @ k x n = m x n
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            q31_t sum = 0;  // 使用32位累加避免溢出
            for (int l = 0; l < k; l++) {
                sum += (q31_t)a[i * k + l] * (q31_t)b[l * n + j];
            }
            // 转换回Q15并饱和
            sum = (sum + (1 << 14)) >> 15;
            if (sum > INT16_MAX) sum = INT16_MAX;
            if (sum < INT16_MIN) sum = INT16_MIN;
            c[i * n + j] = (q15_t)sum;
        }
    }
}

// ReLU激活 (定点数)
void relu_fixed(q15_t* data, int size) {
    for (int i = 0; i < size; i++) {
        data[i] = (data[i] > 0) ? data[i] : 0;
    }
}
```

### 4. 循环展开 (Loop Unrolling)

```c
// 确定性卷积 (已展开循环以减少分支)
void conv2d_deterministic(const float* input,
                         const float* kernel,
                         float* output,
                         int in_h, int in_w,
                         int k_h, int k_w,
                         int out_h, int out_w) {

    for (int oh = 0; oh < out_h; oh++) {
        for (int ow = 0; ow < out_w; ow++) {
            float sum = 0.0f;

            // 手动展开3x3卷积核 (最常见情况)
            #if KERNEL_SIZE == 3
            const float* inp = &input[oh * in_w + ow];

            // Row 0
            sum += inp[0 * in_w + 0] * kernel[0 * 3 + 0];
            sum += inp[0 * in_w + 1] * kernel[0 * 3 + 1];
            sum += inp[0 * in_w + 2] * kernel[0 * 3 + 2];

            // Row 1
            sum += inp[1 * in_w + 0] * kernel[1 * 3 + 0];
            sum += inp[1 * in_w + 1] * kernel[1 * 3 + 1];
            sum += inp[1 * in_w + 2] * kernel[1 * 3 + 2];

            // Row 2
            sum += inp[2 * in_w + 0] * kernel[2 * 3 + 0];
            sum += inp[2 * in_w + 1] * kernel[2 * 3 + 1];
            sum += inp[2 * in_w + 2] * kernel[2 * 3 + 2];
            #else
            // 通用实现
            for (int kh = 0; kh < k_h; kh++) {
                for (int kw = 0; kw < k_w; kw++) {
                    sum += input[(oh + kh) * in_w + (ow + kw)] *
                           kernel[kh * k_w + kw];
                }
            }
            #endif

            output[oh * out_w + ow] = sum;
        }
    }
}
```

---

## WCET (Worst Case Execution Time) 分析

### 1. 基本块计时

```c
// 使用周期计数器测量基本块执行时间
#include <stdint.h>

// ARM DWT周期计数器 (Cortex-M3/M4/M7)
#define DWT_CYCCNT (*((volatile uint32_t*)0xE0001004))
#define DWT_CONTROL (*((volatile uint32_t*)0xE0001000))
#define SCB_DEMCR (*((volatile uint32_t*)0xE000EDFC))

void dwt_init(void) {
    SCB_DEMCR |= 0x01000000;  // Enable DWT
    DWT_CONTROL |= 1;         // Enable cycle counter
}

uint32_t dwt_get_cycles(void) {
    return DWT_CYCCNT;
}

typedef struct {
    uint32_t min_cycles;
    uint32_t max_cycles;
    uint32_t total_cycles;
    uint32_t call_count;
} timing_stats_t;

// 测量函数执行时间
typedef struct {
    uint32_t start;
    timing_stats_t* stats;
} timed_scope_t;

timed_scope_t timed_function_start(timing_stats_t* stats) {
    return (timed_scope_t){dwt_get_cycles(), stats};
}

void timed_function_end(timed_scope_t scope) {
    uint32_t end = dwt_get_cycles();
    uint32_t cycles = end - scope.start;

    timing_stats_t* s = scope.stats;
    if (cycles < s->min_cycles || s->call_count == 0) {
        s->min_cycles = cycles;
    }
    if (cycles > s->max_cycles) {
        s->max_cycles = cycles;
    }
    s->total_cycles += cycles;
    s->call_count++;
}

// 使用示例
static timing_stats_t conv_stats = {0};

void profiled_convolution(...) {
    timed_scope_t scope = timed_function_start(&conv_stats);

    // 执行卷积
    convolution_impl(...);

    timed_function_end(scope);
}
```

### 2. 抽象解释 (Abstract Interpretation)

使用静态分析工具（如aiT、Bound-T）分析WCET：

```c
// 编写"WCET友好"的代码

// ❌ 避免递归
int fib_recursive(int n) {
    if (n <= 1) return n;
    return fib_recursive(n-1) + fib_recursive(n-2);  // 无法分析
}

// ✅ 使用迭代
int fib_iterative(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {  // 有界循环
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

// ✅ 使用__builtin_expect提示分支概率 (GCC)
void process_data(int* data, int size) {
    for (int i = 0; i < size; i++) {
        // 提示: data[i] == 0 是罕见情况
        if (__builtin_expect(data[i] == 0, 0)) {
            handle_zero();  // WCET分析中视为冷路径
        } else {
            handle_nonzero();
        }
    }
}
```

### 3. 缓存锁定 (Cache Locking)

```c
// ARM Cortex-M7 支持指令缓存锁定

// 将关键函数锁定到缓存
void lock_critical_function_to_cache(void) {
    extern uint32_t critical_function_start;
    extern uint32_t critical_function_end;

    // 逐出缓存行
    SCB_InvalidateICache();

    // 执行函数以加载到缓存
    critical_function();

    // 锁定缓存 (特定于MCU实现)
    // 需要参考具体芯片手册
}

// 数据缓存锁定
void lock_data_to_cache(void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    uint32_t* end = (uint32_t*)((uint8_t*)data + size);

    // 访问每个缓存行以加载到缓存
    while (ptr < end) {
        volatile uint32_t dummy = *ptr;  // 强制加载
        ptr += 8;  // 32字节缓存行
    }

    // 锁定 (实现依赖于具体芯片)
}
```

---

## 实时调度集成

### 1. Rate Monotonic Scheduling (RMS)

```c
// 实时AI任务参数
typedef struct {
    uint32_t period_ms;      // 周期 (例如: 10ms)
    uint32_t wcet_ms;        // 最坏执行时间
    uint32_t deadline_ms;    // 截止时间 (通常等于周期)
    uint8_t priority;        // 优先级 (周期越短越高)
    void (*task_func)(void); // 任务函数
} rt_ai_task_t;

// AI推理任务 (例如: 物体检测)
void object_detection_task(void) {
    static uint32_t last_run = 0;
    uint32_t now = get_tick_ms();

    // 检查是否满足周期要求
    if (now - last_run >= 10) {  // 100Hz
        uint32_t start = dwt_get_cycles();

        // 执行确定性推理
        deterministic_inference(g_input, g_output);

        uint32_t elapsed_ms = (dwt_get_cycles() - start) / (SystemCoreClock / 1000);

        // 监控实际执行时间
        if (elapsed_ms > 5) {  // 如果超过50%的周期
            trigger_warning("Inference approaching deadline");
        }

        last_run = now;
    }
}

// 任务表
rt_ai_task_t g_ai_tasks[] = {
    {10, 5, 10, 1, object_detection_task},    // 高优先级
    {50, 10, 50, 2, lane_detection_task},     // 中优先级
    {100, 20, 100, 3, traffic_sign_task},     // 低优先级
};
```

### 2. 时间触发架构 (TTA)

```c
// 时间触发调度表
typedef struct {
    uint32_t offset_ms;  // 相对于周期的偏移
    void (*action)(void);
} schedule_entry_t;

// 主调度循环 (100Hz基础频率)
void time_triggered_scheduler(void) {
    static uint32_t tick = 0;

    // 每10ms执行一次
    wait_for_next_tick(10);

    switch (tick % 10) {  // 10个时隙
        case 0:  // 0ms
            read_sensors();
            break;
        case 1:  // 10ms
            preprocess_data();
            break;
        case 2:  // 20ms - AI推理
            deterministic_inference();
            break;
        case 3:  // 30ms
            postprocess_results();
            break;
        case 4:  // 40ms
            output_control();
            break;
        default:
            // 空闲时间或其他任务
            break;
    }

    tick++;
}
```

---

## 实际案例: 汽车ADAS实时推理

### 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│              ADAS Real-Time AI System                        │
├─────────────────────────────────────────────────────────────┤
│  Sensor Fusion (1ms)                                        │
│  ├─ Camera: 30fps → 33.3ms period                           │
│  ├─ Radar: 20Hz → 50ms period                               │
│  └─ LiDAR: 10Hz → 100ms period                              │
├─────────────────────────────────────────────────────────────┤
│  AI Inference Pipeline                                      │
│  ├─ Object Detection (30ms WCET, 33ms deadline)            │
│  ├─ Lane Detection (15ms WCET, 33ms deadline)              │
│  └─ Free Space (20ms WCET, 50ms deadline)                  │
├─────────────────────────────────────────────────────────────┤
│  Decision & Control (5ms WCET, 10ms deadline)              │
└─────────────────────────────────────────────────────────────┘
```

### 确定性推理实现

```c
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
```

---

## 验证与测试

### 1. 执行时间边界测试

```c
// 在多种条件下测试WCET
void wcet_validation_test(void) {
    uint32_t max_cycles = 0;

    // 测试输入范围边界
    q7_t test_inputs[][3] = {
        {0, 0, 0},           // 全零
        {127, 127, 127},     // 最大正值
        {-128, -128, -128},  // 最大负值
        {127, -128, 0},      // 混合
    };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1000; j++) {  // 多次运行
            uint32_t start = DWT_CYCCNT;
            deterministic_inference(test_inputs[i], output);
            uint32_t cycles = DWT_CYCCNT - start;

            if (cycles > max_cycles) {
                max_cycles = cycles;
            }
        }
    }

    printf("Measured WCET: %u cycles (%u us @ 480MHz)\n",
           max_cycles, max_cycles / 480);
}
```

### 2. 形式化WCET分析工具

```bash
# 使用aiT (AbsInt)进行WCET分析

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../readme.md](../../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```

学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]

```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/readme.md](../../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/readme.md](../../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/readme.md](../../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/readme.md](../../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

### 【总体层】知识体系架构
```

┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘

```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/readme.md) |

---

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
ait --cpu=Cortex-M7 \
    --clock=480MHz \
    --cache=on \
    --memory-model=detailed \
    deterministic_inference.o \
    --entry=deterministic_inference \
    --report=wcet_report.txt

# 输出示例:
# WCET: 14,352,000 cycles = 29.9ms @ 480MHz
# WCET / Deadline = 29.9ms / 33.3ms = 89.7% (满足要求)
```

---

## 参考资源

- [WCET Analysis Tools](https://www.absint.com/ait/)
- [Real-Time Systems by Jane W. S. Liu](https://www.cs.ucr.edu/~ratul/Courses/UCr-CS203-F20/pdfs/liu-2000.pdf)
- [ARM CMSIS-DSP for Real-Time](https://arm-software.github.io/CMSIS-DSP/latest/)
- [ISO 26262 Functional Safety](https://www.iso.org/standard/68383.html)

---

**上一章**: 04_C_Implementation_Guide/02_Static_Memory_Inference（待创建）
**下一章**: 07_Hardware_Acceleration/01_ARM_Ethos_U（待创建）

**最后更新**: 2026-03-18


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
