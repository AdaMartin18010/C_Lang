# TinyML基础介绍

> **文档状态**: 🆕 新建
> **创建日期**: 2026-03-18
> **分类**: TinyML / 嵌入式AI

---

## 什么是TinyML

**TinyML** (Tiny Machine Learning)是在超低功耗微控制器(MCU)上运行机器学习模型的技术和领域，通常指在功耗低于1mW的设备上进行推理。

### 传统ML vs TinyML

| 特性 | 传统ML (云端) | 边缘ML (边缘设备) | TinyML (微控制器) |
|:-----|:-------------|:-----------------|:-----------------|
| **功耗** | 100W - 10kW | 5W - 100W | <1mW |
| **内存** | GB - TB | MB - GB | KB - MB |
| **存储** | TB - PB | GB - TB | KB - MB |
| **延迟** | 100ms+ | 10-100ms | <10ms |
| **成本** | $$$ | $$ | $ |
| **隐私** | 数据上传 | 本地处理 | 完全本地 |

### TinyML应用领域

```
┌─────────────────────────────────────────────────────────────┐
│                    TinyML Applications                       │
├─────────────────┬─────────────────┬─────────────────────────┤
│   智能音频       │    计算机视觉    │      传感器分析          │
├─────────────────┼─────────────────┼─────────────────────────┤
│ • 关键词唤醒     │ • 物体检测       │ • 异常检测              │
│ • 语音命令识别   │ • 人脸识别       │ • 预测性维护            │
│ • 噪声抑制       │ • 姿态估计       │ • 活动识别              │
│ • 鸟鸣识别       │ • 手势识别       │ • 健康监测              │
├─────────────────┼─────────────────┼─────────────────────────┤
│   工业物联网     │    智能家居      │      可穿戴设备          │
├─────────────────┼─────────────────┼─────────────────────────┤
│ • 振动分析       │ • 智能恒温器     │ • 跌倒检测              │
│ • 质量检测       │ • 智能照明       │ • 步态分析              │
│ • 能耗优化       │ • 安防监控       │ • 心率监测              │
│ • 资产追踪       │ • 家电控制       │ • 睡眠监测              │
└─────────────────┴─────────────────┴─────────────────────────┘
```

---


---

## 📑 目录

- [TinyML基础介绍](#tinyml基础介绍)
  - [什么是TinyML](#什么是tinyml)
    - [传统ML vs TinyML](#传统ml-vs-tinyml)
    - [TinyML应用领域](#tinyml应用领域)
  - [📑 目录](#-目录)
  - [TinyML工作流程](#tinyml工作流程)
    - [标准开发流程](#标准开发流程)
    - [1. 数据收集](#1-数据收集)
    - [2. 模型训练 (Python端)](#2-模型训练-python端)
    - [3. 转换为C代码](#3-转换为c代码)
    - [4. 嵌入式推理 (C代码)](#4-嵌入式推理-c代码)
  - [TinyML关键技术](#tinyml关键技术)
    - [模型量化](#模型量化)
    - [模型优化技术](#模型优化技术)
  - [硬件平台选择](#硬件平台选择)
    - [主流TinyML硬件](#主流tinyml硬件)
    - [性能基准](#性能基准)
  - [开发工具](#开发工具)
    - [端到端开发平台](#端到端开发平台)
  - [最佳实践](#最佳实践)
    - [内存管理](#内存管理)
    - [功耗优化](#功耗优化)
  - [参考资源](#参考资源)


---

## TinyML工作流程

### 标准开发流程

```
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│  数据收集     │ → │  模型训练     │ → │  模型转换     │
│  (传感器)     │    │  (TensorFlow) │    │  (TFLite)    │
└──────────────┘    └──────────────┘    └──────┬───────┘
                                                │
                                                ▼
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│  部署推理     │ ← │  量化优化     │ ← │  模型优化     │
│  (C/C++)     │    │  (INT8)      │    │  (剪枝/蒸馏)  │
└──────────────┘    └──────────────┘    └──────────────┘
```

### 1. 数据收集

```c
// 示例: 从IMU传感器收集数据
#include "sensor.h"

#define SAMPLE_RATE_HZ 100
#define SAMPLE_DURATION_MS 1000
#define NUM_SAMPLES (SAMPLE_RATE_HZ * SAMPLE_DURATION_MS / 1000)

void collect_training_data(void) {
    float accel_data[NUM_SAMPLES][3];  // x, y, z
    uint32_t timestamp;

    printf("Starting data collection...\n");

    for (int i = 0; i < NUM_SAMPLES; i++) {
        // 读取加速度计
        sensor_read_accelerometer(&accel_data[i][0],
                                   &accel_data[i][1],
                                   &accel_data[i][2]);

        // 添加时间戳
        timestamp = get_timestamp_ms();

        // 存储或传输数据
        log_data(timestamp, accel_data[i]);

        delay_ms(1000 / SAMPLE_RATE_HZ);
    }

    printf("Collected %d samples\n", NUM_SAMPLES);
}
```

### 2. 模型训练 (Python端)

```python
# train_keyword_spotting.py
import tensorflow as tf
from tensorflow import keras
import numpy as np

# 模型参数
SAMPLE_RATE = 16000
FFT_SIZE = 512
NUM_MEL_BINS = 40
NUM_FRAMES = 49

# 构建轻量级CNN模型
def create_tiny_model():
    model = keras.Sequential([
        # 输入: 梅尔频谱图
        keras.layers.Input(shape=(NUM_FRAMES, NUM_MEL_BINS)),

        # 深度可分离卷积 (计算效率高)
        keras.layers.Reshape((NUM_FRAMES, NUM_MEL_BINS, 1)),
        keras.layers.DepthwiseConv2D(
            kernel_size=(3, 3),
            depth_multiplier=8,
            activation='relu'
        ),
        keras.layers.BatchNormalization(),
        keras.layers.MaxPooling2D(pool_size=(2, 2)),

        # 标准卷积
        keras.layers.Conv2D(64, (3, 3), activation='relu'),
        keras.layers.BatchNormalization(),
        keras.layers.MaxPooling2D(pool_size=(2, 2)),

        # 分类头
        keras.layers.Flatten(),
        keras.layers.Dense(128, activation='relu'),
        keras.layers.Dropout(0.5),
        keras.layers.Dense(4, activation='softmax')  # 4个关键词
    ])

    model.compile(
        optimizer='adam',
        loss='categorical_crossentropy',
        metrics=['accuracy']
    )

    return model

# 训练并保存
model = create_tiny_model()
model.fit(train_data, train_labels, epochs=50, batch_size=32)
model.save('keyword_model.h5')

# 转换为TFLite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.target_spec.supported_types = [tf.int8]  # 量化到INT8

tflite_model = converter.convert()
with open('keyword_model.tflite', 'wb') as f:
    f.write(tflite_model)

print(f"Model size: {len(tflite_model) / 1024:.2f} KB")
```

### 3. 转换为C代码

```bash
# 使用xxd将TFLite模型转换为C数组
xxd -i keyword_model.tflite > keyword_model_data.cc

# 或使用TensorFlow Lite Micro的转换工具
python convert_to_c_array.py --model keyword_model.tflite \
                             --output model_data.h
```

### 4. 嵌入式推理 (C代码)

```c
// main.c: TinyML推理示例
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "model_data.h"  // 生成的模型数组

// 内存配置
constexpr int kTensorArenaSize = 10 * 1024;  // 10KB张量区域
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

// 模型输入/输出维度
constexpr int kInputSize = 1960;   // 49帧 x 40梅尔频带
constexpr int kOutputSize = 4;      // 4个关键词类别

void setup_tflm() {
    // 加载模型
    const tflite::Model* model = tflite::GetModel(g_keyword_model_data);

    // 验证模型版本
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        MicroPrintf("Model version mismatch!");
        return;
    }

    // 配置操作解析器 (仅包含需要的操作)
    static tflite::MicroMutableOpResolver<5> resolver;
    resolver.AddDepthwiseConv2D();
    resolver.AddConv2D();
    resolver.AddMaxPool2D();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();

    // 创建解释器
    static tflite::MicroInterpreter interpreter(
        model, resolver, tensor_arena, kTensorArenaSize);

    TfLiteStatus allocate_status = interpreter.AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        MicroPrintf("AllocateTensors() failed");
        return;
    }

    MicroPrintf("TinyML model loaded successfully!");
}

int run_inference(const float* audio_features) {
    // 获取输入张量
    TfLiteTensor* input = interpreter.input(0);

    // 复制输入数据 (量化到INT8)
    for (int i = 0; i < kInputSize; i++) {
        input->data.int8[i] = static_cast<int8_t>(
            (audio_features[i] / input->params.scale) + input->params.zero_point
        );
    }

    // 运行推理
    TfLiteStatus invoke_status = interpreter.Invoke();
    if (invoke_status != kTfLiteOk) {
        MicroPrintf("Invoke failed");
        return -1;
    }

    // 获取输出
    TfLiteTensor* output = interpreter.output(0);

    // 找到最大概率的类别
    int max_index = 0;
    int8_t max_value = output->data.int8[0];
    for (int i = 1; i < kOutputSize; i++) {
        if (output->data.int8[i] > max_value) {
            max_value = output->data.int8[i];
            max_index = i;
        }
    }

    // 反量化得到概率
    float probability = (max_value - output->params.zero_point) * output->params.scale;

    if (probability > 0.8f) {
        return max_index;  // 检测到关键词
    }

    return -1;  // 无关键词
}

int main() {
    setup_tflm();

    while (1) {
        // 从麦克风获取音频特征
        float audio_features[kInputSize];
        extract_mel_spectrogram(audio_features);

        // 运行推理
        int keyword = run_inference(audio_features);

        if (keyword >= 0) {
            const char* keywords[] = {"Yes", "No", "Up", "Down"};
            printf("Detected: %s\n", keywords[keyword]);

            // 触发相应动作
            execute_command(keyword);
        }

        delay_ms(100);  // 100ms检测间隔
    }

    return 0;
}
```

---

## TinyML关键技术

### 模型量化

```python
# 后训练量化示例
import tensorflow as tf

def representative_dataset():
    for _ in range(100):
        data = np.random.randn(1, 49, 40).astype(np.float32)
        yield [data]

converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.representative_dataset = representative_dataset
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8

tflite_quant_model = converter.convert()
```

### 模型优化技术

| 技术 | 描述 | 效果 | 复杂度 |
|:-----|:-----|:-----|:-------|
| **权重量化** | FP32 → INT8 | 4x大小减少 | 低 |
| **剪枝** | 移除不重要权重 | 2-10x稀疏 | 中 |
| **知识蒸馏** | 小模型学习大模型 | 保持精度 | 高 |
| **NAS** | 神经网络架构搜索 | 最优结构 | 高 |
| **算子融合** | 合并相邻操作 | 减少内存访问 | 低 |

---

## 硬件平台选择

### 主流TinyML硬件

| MCU | 架构 | Flash | RAM | 时钟 | 功耗 | 价格 |
|:-----|:-----|:------|:----|:-----|:-----|:-----|
| **ESP32-S3** | Xtensa LX7 | 8MB | 512KB | 240MHz | 240mA | $3 |
| **STM32H7** | ARM Cortex-M7 | 2MB | 1MB | 480MHz | 300mA | $8 |
| **nRF5340** | ARM Cortex-M33 | 1MB | 512KB | 128MHz | 100mA | $5 |
| **RP2040** | ARM Cortex-M0+ | 2MB | 264KB | 133MHz | 90mA | $1 |
| **Arduino Nano 33 BLE** | ARM Cortex-M4 | 1MB | 256KB | 64MHz | 80mA | $25 |

### 性能基准

在**关键词检测**任务上 (模型大小: 80KB):

| 平台 | 推理延迟 | 功耗 | 能效 |
|:-----|:---------|:-----|:-----|
| ESP32-S3 | 12ms | 120mW | 优秀 |
| STM32H7 | 8ms | 150mW | 优秀 |
| nRF5340 | 25ms | 50mW | 优秀 |
| RP2040 | 45ms | 30mW | 良好 |

---

## 开发工具

### 端到端开发平台

1. **Edge Impulse**
   - 可视化工作流
   - 自动特征提取
   - 一键部署到多种平台

2. **Arduino TinyML**
   - 与Arduino生态集成
   - 适合初学者
   - 大量教程

3. **emlearn**
   - 纯C/C++实现
   - 零依赖
   - 可移植性强

4. **microTVM**
   - Apache TVM微控制器版本
   - 支持多种前端
   - 自动优化

---

## 最佳实践

### 内存管理

```c
// 使用静态分配避免堆碎片
static uint8_t tensor_arena[10 * 1024] __attribute__((aligned(16)));
static TfLiteTensor input_buffer;

// 避免动态内存
// ❌ 不推荐
float* buffer = malloc(size);

// ✅ 推荐
static float buffer[MAX_SIZE];
```

### 功耗优化

```c
// 推理后进入低功耗模式
void low_power_inference() {
    // 唤醒、推理、休眠循环
    while (1) {
        // 等待唤醒事件 (GPIO中断或定时器)
        enter_light_sleep_mode();

        // 快速推理
        int result = tflm_invoke();

        // 处理结果
        process_result(result);

        // 立即返回低功耗
    }
}
```

---

## 参考资源

- [TensorFlow Lite Micro](https://www.tensorflow.org/lite/microcontrollers)
- [Edge Impulse](https://www.edgeimpulse.com/)
- [TinyML Foundation](https://www.tinyml.org/)
- [Harvard TinyML Course](https://sites.google.com/g.harvard.edu/tinyml/home)

---

**下一章**: [02_TensorFlow_Lite_Micro.md](./02_TensorFlow_Lite_Micro.md)

**最后更新**: 2026-03-18
