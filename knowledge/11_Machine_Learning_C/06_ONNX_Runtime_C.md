# ONNX Runtime C API

> **难度**: L4 | **预估学习时间**: 3-4小时
> 包含：ONNX模型加载、推理执行、性能优化

---

## 1. ONNX概述

**ONNX (Open Neural Network Exchange)** 是开放的深度学习模型标准，允许在不同框架间互操作。

### 1.1 为什么使用ONNX Runtime

| 优势 | 说明 |
|:-----|:-----|
| 跨框架 | PyTorch/TensorFlow → ONNX → C部署 |
| 高性能 | 多种执行提供者(CPU/GPU/TensorRT) |
| 轻量级 | 纯C API，适合嵌入式 |
| 优化 | 图优化、量化、剪枝 |

### 1.2 工作流程

```text
训练(PyTorch/TF) → 导出ONNX → 优化(ORT) → C部署
```

---

## 2. 环境搭建

### 2.1 安装ONNX Runtime

```bash
# 从源码构建（嵌入式优化）
git clone --recursive https://github.com/microsoft/onnxruntime.git
cd onnxruntime

# 构建C库（最小化版本）
./build.sh --config MinSizeRel --build_shared_lib \
           --minimal_build --disable_ml_ops \
           --skip_tests --parallel

# 或使用预编译版本
wget https://github.com/microsoft/onnxruntime/releases/download/v1.17.0/onnxruntime-linux-x64-1.17.0.tgz
tar -xzf onnxruntime-linux-x64-1.17.0.tgz
```

### 2.2 链接配置

```cmake
# CMakeLists.txt
find_package(onnxruntime REQUIRED)

add_executable(inference inference.c)
target_link_libraries(inference onnxruntime)

# 或手动链接
include_directories(/path/to/onnxruntime/include)
link_directories(/path/to/onnxruntime/lib)
target_link_libraries(inference onnxruntime.so)
```

---

## 3. C API基础

### 3.1 头文件与初始化

```c
#include <onnxruntime_c_api.h>
#include <stdio.h>
#include <stdlib.h>

// 全局API指针
const OrtApi* g_ort = NULL;

// 初始化
void onnx_init() {
    g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (!g_ort) {
        fprintf(stderr, "Failed to initialize ONNX Runtime\n");
        exit(1);
    }
}

// 错误处理宏
#define CHECK_STATUS(status) \
    if (status != NULL) { \
        const char* msg = g_ort->GetErrorMessage(status); \
        fprintf(stderr, "ONNX Error: %s\n", msg); \
        g_ort->ReleaseStatus(status); \
        exit(1); \
    }
```

### 3.2 环境与会话

```c
typedef struct {
    OrtEnv* env;
    OrtSessionOptions* session_options;
    OrtSession* session;
    OrtMemoryInfo* memory_info;
} ONNX_Runtime;

ONNX_Runtime* onnx_create_runtime(const char* model_path) {
    ONNX_Runtime* rt = calloc(1, sizeof(ONNX_Runtime));
    OrtStatus* status = NULL;

    // 创建环境
    status = g_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "inference", &rt->env);
    CHECK_STATUS(status);

    // 会话选项
    status = g_ort->CreateSessionOptions(&rt->session_options);
    CHECK_STATUS(status);

    // 设置线程数
    g_ort->SetIntraOpNumThreads(rt->session_options, 4);
    g_ort->SetInterOpNumThreads(rt->session_options, 4);

    // 图优化
    g_ort->SetSessionGraphOptimizationLevel(
        rt->session_options,
        ORT_ENABLE_ALL);

    // 创建会话
    status = g_ort->CreateSession(rt->env, model_path,
                                   rt->session_options, &rt->session);
    CHECK_STATUS(status);

    // CPU内存信息
    status = g_ort->CreateCpuMemoryInfo(
        OrtArenaAllocator, OrtMemTypeDefault, &rt->memory_info);
    CHECK_STATUS(status);

    return rt;
}

void onnx_destroy_runtime(ONNX_Runtime* rt) {
    if (!rt) return;
    g_ort->ReleaseMemoryInfo(rt->memory_info);
    g_ort->ReleaseSession(rt->session);
    g_ort->ReleaseSessionOptions(rt->session_options);
    g_ort->ReleaseEnv(rt->env);
    free(rt);
}
```

---

## 4. 模型推理

### 4.1 查询模型信息

```c
void print_model_info(ONNX_Runtime* rt) {
    OrtStatus* status = NULL;
    OrtAllocator* allocator = NULL;
    status = g_ort->GetAllocatorWithDefaultOptions(&allocator);
    CHECK_STATUS(status);

    // 输入数量
    size_t num_inputs;
    status = g_ort->SessionGetInputCount(rt->session, &num_inputs);
    CHECK_STATUS(status);
    printf("Number of inputs: %zu\n", num_inputs);

    // 输入信息
    for (size_t i = 0; i < num_inputs; i++) {
        char* name = NULL;
        status = g_ort->SessionGetInputName(rt->session, i, allocator, &name);
        CHECK_STATUS(status);

        OrtTypeInfo* type_info = NULL;
        status = g_ort->SessionGetInputTypeInfo(rt->session, i, &type_info);
        CHECK_STATUS(status);

        const OrtTensorTypeAndShapeInfo* tensor_info = NULL;
        status = g_ort->CastTypeInfoToTensorInfo(type_info, &tensor_info);
        CHECK_STATUS(status);

        ONNXTensorElementDataType type;
        status = g_ort->GetTensorElementType(tensor_info, &type);
        CHECK_STATUS(status);

        size_t num_dims;
        status = g_ort->GetDimensionsCount(tensor_info, &num_dims);
        CHECK_STATUS(status);

        int64_t* dims = malloc(num_dims * sizeof(int64_t));
        status = g_ort->GetDimensions(tensor_info, dims, num_dims);
        CHECK_STATUS(status);

        printf("  Input %zu: %s, type=%d, dims=[", i, name, type);
        for (size_t d = 0; d < num_dims; d++) {
            printf("%ld%s", dims[d], d < num_dims - 1 ? ", " : "");
        }
        printf("]\n");

        free(dims);
        allocator->Free(allocator, name);
        g_ort->ReleaseTypeInfo(type_info);
    }
}
```

### 4.2 创建输入张量

```c
OrtValue* create_input_tensor(ONNX_Runtime* rt, float* data,
                               int64_t* dims, size_t num_dims) {
    OrtStatus* status = NULL;
    OrtValue* tensor = NULL;

    size_t total_elements = 1;
    for (size_t i = 0; i < num_dims; i++) {
        total_elements *= dims[i];
    }

    status = g_ort->CreateTensorWithDataAsOrtValue(
        rt->memory_info,
        data,                                    // 数据指针
        total_elements * sizeof(float),         // 数据大小
        dims, num_dims,                         // 维度
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,    // 数据类型
        &tensor);
    CHECK_STATUS(status);

    return tensor;
}
```

### 4.3 执行推理

```c
typedef struct {
    float* data;
    int64_t* shape;
    size_t num_dims;
    size_t total_size;
} Tensor_Output;

Tensor_Output* onnx_inference(ONNX_Runtime* rt, OrtValue** inputs,
                               size_t num_inputs) {
    OrtStatus* status = NULL;

    // 输出数组
    OrtValue* outputs[1] = {NULL};

    // 运行会话
    const char* input_names[] = {"input"};  // 根据模型修改
    const char* output_names[] = {"output"};

    status = g_ort->Run(rt->session, NULL,
                        input_names, inputs, num_inputs,
                        output_names, 1,
                        outputs);
    CHECK_STATUS(status);

    // 获取输出数据
    float* output_data;
    status = g_ort->GetTensorMutableData(outputs[0], (void**)&output_data);
    CHECK_STATUS(status);

    // 获取输出形状
    OrtTensorTypeAndShapeInfo* info = NULL;
    status = g_ort->GetTensorTypeAndShape(outputs[0], &info);
    CHECK_STATUS(status);

    size_t num_dims;
    g_ort->GetDimensionsCount(info, &num_dims);
    int64_t* shape = malloc(num_dims * sizeof(int64_t));
    g_ort->GetDimensions(info, shape, num_dims);

    size_t total_size;
    g_ort->GetTensorShapeElementCount(info, &total_size);

    // 复制数据（因为outputs会被释放）
    float* data_copy = malloc(total_size * sizeof(float));
    memcpy(data_copy, output_data, total_size * sizeof(float));

    Tensor_Output* result = malloc(sizeof(Tensor_Output));
    result->data = data_copy;
    result->shape = shape;
    result->num_dims = num_dims;
    result->total_size = total_size;

    g_ort->ReleaseTensorTypeAndShapeInfo(info);
    g_ort->ReleaseValue(outputs[0]);

    return result;
}

void free_tensor_output(Tensor_Output* tensor) {
    if (tensor) {
        free(tensor->data);
        free(tensor->shape);
        free(tensor);
    }
}
```

---

## 5. 完整示例：MNIST推理

```c
#include "onnx_inference.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <model.onnx>\n", argv[0]);
        return 1;
    }

    // 初始化
    onnx_init();

    // 创建运行时
    ONNX_Runtime* rt = onnx_create_runtime(argv[1]);

    // 打印模型信息
    print_model_info(rt);

    // 准备输入 (1x1x28x28 MNIST图像)
    float input_data[1 * 1 * 28 * 28];
    // 填充数据 (例如从文件读取图像)
    for (int i = 0; i < 784; i++) {
        input_data[i] = 0.5f;  // 示例数据
    }

    int64_t input_dims[] = {1, 1, 28, 28};
    OrtValue* input_tensor = create_input_tensor(rt, input_data,
                                                   input_dims, 4);

    // 执行推理
    OrtValue* inputs[] = {input_tensor};
    Tensor_Output* output = onnx_inference(rt, inputs, 1);

    // 处理输出 (10类概率)
    printf("Output probabilities:\n");
    for (int i = 0; i < 10; i++) {
        printf("  Digit %d: %.4f\n", i, output->data[i]);
    }

    // 找出最大概率
    int max_idx = 0;
    float max_prob = output->data[0];
    for (int i = 1; i < 10; i++) {
        if (output->data[i] > max_prob) {
            max_prob = output->data[i];
            max_idx = i;
        }
    }
    printf("Predicted digit: %d (confidence: %.2f%%)\n",
           max_idx, max_prob * 100);

    // 清理
    free_tensor_output(output);
    g_ort->ReleaseValue(input_tensor);
    onnx_destroy_runtime(rt);

    return 0;
}
```

---

## 6. 性能优化

### 6.1 执行提供者

```c
// 启用TensorRT (NVIDIA GPU)
#ifdef USE_TENSORRT
OrtTensorRTProviderOptionsV2* tensorrt_options = NULL;
g_ort->CreateTensorRTProviderOptions(&tensorrt_options);
g_ort->SessionOptionsAppendExecutionProvider_TensorRT_V2(
    rt->session_options, tensorrt_options);
g_ort->ReleaseTensorRTProviderOptions(tensorrt_options);
#endif

// 启用OpenVINO (Intel)
#ifdef USE_OPENVINO
g_ort->SessionOptionsAppendExecutionProvider_OpenVINO(
    rt->session_options, "");
#endif

// 启用CoreML (Apple)
#ifdef USE_COREML
 OrtCoreMLProviderOptions coreml_options = {0};
 coreml_options.use_cpu_only = 0;
 coreml_options.enable_on_subgraph = 1;
 g_ort->SessionOptionsAppendExecutionProvider_CoreML(
     rt->session_options, &coreml_options);
#endif
```

### 6.2 量化模型

```python
# Python端量化
import onnx
from onnxruntime.quantization import quantize_dynamic, QuantType

model_fp32 = 'model.onnx'
model_quant = 'model.quant.onnx'

quantize_dynamic(model_fp32, model_quant, weight_type=QuantType.QUInt8)
```

### 6.3 动态形状

```c
// 处理动态批次大小
int64_t input_dims[] = {-1, 3, 224, 224};  // -1表示动态

// 使用符号维度
OrtSessionOptions* options;
g_ort->CreateSessionOptions(&options);
g_ort->AddFreeDimensionOverrideByName(options, "batch_size", 1);
```

---

## 7. 嵌入式部署

### 7.1 最小化构建

```bash
# 仅保留必要算子
python tools/ci_build/build.py \
    --build_dir build_embedded \
    --config MinSizeRel \
    --minimal_build \
    --include_ops_by_config required_ops.config \
    --disable_ml_ops \
    --skip_tests
```

### 7.2 内存池优化

```c
// 使用固定内存池避免动态分配
#define MAX_TENSOR_SIZE (1024 * 1024)  // 1MB
static float g_tensor_pool[MAX_TENSOR_SIZE];

OrtValue* create_tensor_from_pool(ONNX_Runtime* rt, int64_t* dims,
                                   size_t num_dims) {
    size_t total = 1;
    for (size_t i = 0; i < num_dims; i++) total *= dims[i];

    if (total > MAX_TENSOR_SIZE) {
        fprintf(stderr, "Tensor too large for pool\n");
        return NULL;
    }

    OrtValue* tensor = NULL;
    g_ort->CreateTensorWithDataAsOrtValue(
        rt->memory_info,
        g_tensor_pool,
        total * sizeof(float),
        dims, num_dims,
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
        &tensor);

    return tensor;
}
```

---

## 8. 参考文献

1. **ONNX Runtime Documentation**: <https://onnxruntime.ai/docs/get-started/with-c.html>
2. **ONNX Format Spec**: <https://github.com/onnx/onnx/blob/main/docs/IR.md>
3. **Microsoft Docs**: C API reference and examples

---

> **关联文档**: [神经网络基础](./01_Neural_Network_Basics.md) | [CNN基础](./04_CNN_Fundamentals.md) | [RNN与LSTM](./05_RNN_LSTM_Basics.md)
