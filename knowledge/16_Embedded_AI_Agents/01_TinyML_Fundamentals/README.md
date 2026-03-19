# TinyML基础

> **在资源受限的嵌入式设备上部署和运行机器学习模型的核心技术与方法论**

## 概述

TinyML（微型机器学习）是机器学习领域的一个新兴分支，专注于在微控制器（MCU）等极度资源受限的设备上运行ML模型。与传统云端AI或边缘计算不同，TinyML面向的设备通常只有几十KB的RAM和几百KB的Flash存储，CPU频率在几十到几百MHz之间，且往往依赖电池供电。这种极端的约束条件催生了模型压缩、量化、高效推理框架等一系列创新技术。

TinyML的核心价值在于将AI能力带到数据源头，实现真正的端侧智能。在物联网场景中，设备可以在不依赖网络连接的情况下实时处理传感器数据，做出决策并执行动作。这种本地化处理不仅降低了延迟和带宽成本，更重要的是保护了用户隐私——原始数据无需上传云端即可得到处理。对于C语言开发者而言，理解TinyML意味着能够将AI能力集成到传统的嵌入式系统中，创造出新一代智能设备。

从技术架构看，TinyML工作流包括：模型训练（通常在TensorFlow等框架中完成）、模型优化（量化、剪枝、蒸馏）、模型转换（转换为TFLite Micro等嵌入式格式）、以及嵌入式推理（使用C/C++实现的推理引擎）。每个环节都需要精心权衡精度与资源消耗，确保最终模型在目标硬件上高效运行。

## 核心概念

- **模型量化（Model Quantization）**：将32位浮点模型权重转换为8位整数，减少模型大小4倍并加速推理，同时通过量化感知训练最小化精度损失。

- **TensorFlow Lite for Microcontrollers**：专为MCU设计的轻量级ML框架，核心运行时仅需几十KB，支持在裸机或RTOS环境下运行预训练模型。

- **CMSIS-NN**：ARM提供的神经网络库，针对Cortex-M处理器优化，提供卷积、池化、全连接等算子的高效汇编实现。

- **关键词唤醒（Keyword Spotting）**：TinyML的典型应用，在语音流中实时检测特定唤醒词，功耗低至毫瓦级，适用于智能音箱和可穿戴设备。

- **视觉唤醒词（Visual Wake Words）**：图像识别任务，检测图像中是否存在特定物体（如人、车），常用于安防摄像头和智能门铃。

- **异常检测（Anomaly Detection）**：学习设备正常运行模式的特征，检测偏离正常模式的异常行为，广泛应用于工业预测性维护。

## 文件列表

| 文件 | 描述 |
|------|------|
| [01_Introduction_to_TinyML.md](./01_Introduction_to_TinyML.md) | TinyML入门介绍 |

## 学习路径

1. **理论基础**：理解神经网络基础，学习CNN、RNN等常用模型架构
2. **工具掌握**：熟练使用TensorFlow或PyTorch训练简单模型
3. **量化优化**：学习模型量化和压缩技术，掌握TFLite转换工具
4. **嵌入式部署**：在STM32或ESP32等开发板上部署TinyML模型
5. **C语言优化**：深入理解CMSIS-NN，编写高效推理代码

## 参考资源

- [TensorFlow Lite Micro](https://www.tensorflow.org/lite/microcontrollers)
- [CMSIS-NN Documentation](https://www.keil.com/pack/doc/CMSIS/NN/html/index.html)
- [TinyML Foundation](https://www.tinyml.org/)
- [TinyML Book by Pete Warden](https://www.oreilly.com/library/view/tinyml/9781492052043/)
- [Harvard Edge ML Course](https://sites.harvard.edu/edge/courses/)

[← 返回上级](../README.md)


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
