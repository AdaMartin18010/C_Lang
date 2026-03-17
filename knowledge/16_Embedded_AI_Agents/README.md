# 嵌入式AI Agent系统

> **模块编号**: 16
> **创建日期**: 2026-03-18
> **状态**: 🆕 新建模块
> **覆盖领域**: TinyML、边缘LLM、AI Agent运行时、C语言实现

---

## 模块概述

本模块涵盖在资源受限的嵌入式系统中实现AI Agent所需的全部知识，包括：

- **TinyML基础**: 微控制器上的机器学习
- **边缘LLM部署**: llama.cpp等框架在嵌入式设备上的应用
- **AI Agent运行时**: ZeroClaw、OpenClaw等轻量级Agent系统
- **C语言实现**: 纯C/C++推理引擎和Agent框架

---

## 子模块导航

### 📘 01_TinyML_Fundamentals

- 01_Introduction_to_TinyML.md
- 02_TensorFlow_Lite_Micro.md
- 03_Neural_Network_Quantization.md
- 04_C_Implementation_Techniques.md

### 🧠 02_LLMs_on_Edge

- 01_llama_cpp_Embedded_Deployment.md
- 02_GGML_Optimization.md
- 03_Model_Quantization_Q4_Q8.md
- 04_Streaming_Inference.md

### 🤖 03_AI_Agent_Runtime

- 01_ZeroClaw_Overview.md
- 02_OpenClaw_Ecosystem.md
- 03_Agent_Architecture_Patterns.md
- 04_C_Rust_Interop.md

### 🛠️ 04_C_Implementation_Guide

- 01_From_PyTorch_to_C.md
- 02_Static_Memory_Inference.md
- 03_Operator_Kernels_Optimization.md
- 04_Embedded_NN_Frameworks.md

### 📊 05_Case_Studies

- 01_ESP32_TinyML_Voice_Recognition.md
- 02_RaspberryPi_llama_cpp.md
- 03_STM32_Anomaly_Detection.md
- 04_ZeroClaw_Edge_Deployment.md

---

## 关键概念速览

### TinyML 生态系统

```
┌─────────────────────────────────────────────────────────────┐
│                      TinyML Ecosystem                        │
├──────────────┬──────────────────┬───────────────────────────┤
│  Frameworks  │  Hardware        │  Tools                    │
├──────────────┼──────────────────┼───────────────────────────┤
│ TensorFlow   │ ESP32系列        │ Edge Impulse              │
│ Lite Micro   │ STM32系列        │ Arduino TinyML            │
│              │ nRF52/53系列     │ emlearn                   │
│              │ Raspberry Pi Pico│ microTVM                  │
│              │ ARM Cortex-M4/M7 │ ACETONE                   │
└──────────────┴──────────────────┴───────────────────────────┘
```

### 边缘LLM部署对比

| 方案 | 硬件要求 | 性能 | 适用场景 |
|:-----|:---------|:-----|:---------|
| **llama.cpp** | 4GB+ RAM | 中等 | 树莓派、边缘服务器 |
| **mlc-llm** | GPU/Vulkan | 高 | 移动端、NPU |
| **llamafile** | 4GB+ RAM | 中等 | 单文件部署 |
| **ONNX Runtime** | 因模型而异 | 高 | 通用推理 |

### AI Agent运行时对比

| 运行时 | 语言 | 二进制大小 | 内存占用 | 启动时间 |
|:-------|:-----|:-----------|:---------|:---------|
| **ZeroClaw** | Rust | 3.4 MB | <5 MB | <10 ms |
| **OpenClaw** | Node.js | ~28 MB | >1 GB | >500 ms |
| **PicoClaw** | Go | ~8 MB | <10 MB | <1 s |
| **自研C Agent** | C | 可定制 | 可定制 | 可定制 |

---

## 硬件平台支持矩阵

| 平台 | TinyML | 小LLM | Agent运行时 | 推荐框架 |
|:-----|:------:|:-----:|:------------|:---------|
| ESP32-S3 | ✅ | ⚠️ | ⚠️ | TFLM, emlearn |
| STM32H7 | ✅ | ❌ | ⚠️ | TFLM, ACETONE |
| Raspberry Pi 4 | ✅ | ✅ | ✅ | llama.cpp |
| Raspberry Pi 5 | ✅ | ✅ | ✅ | llama.cpp, ZeroClaw |
| Jetson Nano | ✅ | ✅ | ✅ | TensorRT, llama.cpp |
| i.MX 8M Plus | ✅ | ✅ | ✅ | NPU加速, TFLM |

---

## 相关模块

- 11_Machine_Learning_C/ - C语言机器学习基础
- 04_Industrial_Scenarios/14_Embedded_IoT/ - 嵌入式IoT场景

---

**最后更新**: 2026-03-18
**维护者**: C_Lang Embedded AI Team
