# 嵌入式 IoT 开发指南

> 真实硬件平台实战：ESP32、树莓派 Pico、Raspberry Pi、工控系统

---

## 目录结构

| 目录 | 内容 | 适用硬件 |
|:-----|:-----|:---------|
| [ESP32](./ESP32/README.md) | ESP32-S3/C3 完整开发指南 | ESP32 DevKit |
| [RPi_Pico](./RPi_Pico/README.md) | RP2040 双核开发 | Raspberry Pi Pico |
| [RaspberryPi](./RaspberryPi/README.md) | 嵌入式 Linux 开发 | RPi 4/5/CM4 |
| [Industrial_Linux](./Industrial_Linux/README.md) | 工控 Linux 系统 | 工业 PC |
| [Memory_Constrained](./Memory_Constrained/README.md) | 内存受限系统设计 | 8/16位 MCU |
| [IoT_Protocols](./IoT_Protocols/README.md) | 轻量级协议实现 | 所有 IoT 平台 |

---

## 快速导航

### 按硬件平台

| 硬件 | RAM | Flash | 推荐内容 |
|:-----|:----|:------|:---------|
| **ESP32-S3** | 512KB | 4-16MB | [ESP32完整指南](./ESP32/01_ESP32_Complete_Guide.md) |
| **ESP32-C3** | 400KB | 4MB | [ESP32完整指南](./ESP32/01_ESP32_Complete_Guide.md) |
| **RPi Pico** | 264KB | 2MB+ | [Pico开发指南](./RPi_Pico/01_Pico_Complete_Guide.md) |
| **RPi 4** | 1-8GB | SD卡 | [嵌入式Linux](./RaspberryPi/01_RaspberryPi_Embedded_Linux.md) |
| **STM32F103** | 20KB | 64KB | [内存受限设计](./Memory_Constrained/01_Memory_Constrained_Design.md) |
| **Arduino Uno** | 2KB | 32KB | [内存受限设计](./Memory_Constrained/01_Memory_Constrained_Design.md) |

### 按应用场景

| 场景 | 推荐内容 |
|:-----|:---------|
| **WiFi/BLE 设备** | [ESP32指南](./ESP32/01_ESP32_Complete_Guide.md) |
| **实时控制** | [Pico指南](./RPi_Pico/01_Pico_Complete_Guide.md) |
| **工业网关** | [嵌入式Linux](./RaspberryPi/01_RaspberryPi_Embedded_Linux.md) |
| **传感器节点** | [内存受限设计](./Memory_Constrained/01_Memory_Constrained_Design.md) |
| **协议开发** | [IoT协议栈](./IoT_Protocols/01_IoT_Protocols_Implementation.md) |

---

## 内容统计

| 模块 | 文件数 | 代码行数 | 特色 |
|:-----|:------:|:--------:|:-----|
| ESP32 | 1 | 800+ | WiFi/BLE/低功耗完整代码 |
| RPi Pico | 1 | 900+ | PIO/双核/DMA实战 |
| Raspberry Pi | 1 | 1000+ | 驱动开发/实时补丁 |
| Memory Constrained | 1 | 700+ | <64KB系统优化 |
| IoT Protocols | 1 | 800+ | MQTT/CoAP/Modbus实现 |

**总计**: 5个文件，4,200+行可运行代码

---

## 硬件准备

### 基础套件

| 物品 | 数量 | 用途 |
|:-----|:----:|:-----|
| ESP32-DevKitC | 1 | WiFi/BLE开发 |
| Raspberry Pi Pico | 1 | 裸机/RTOS学习 |
| 树莓派 4B | 1 | Linux嵌入式 |
| ST-Link V2 | 1 | STM32调试 |
| USB-TTL | 2 | 串口调试 |
| 杜邦线 | 若干 | 连接外设 |
| 面包板 | 2 | 原型搭建 |

### 传感器模块

- DHT22/BME280 (温湿度)
- ADXL345 (加速度)
- HC-SR04 (超声波)
- MQ-135 (空气质量)

---

## 开发环境

### ESP32

```bash
# ESP-IDF v5.1+
git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git
./install.sh esp32s3
```

### RPi Pico

```bash
# Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
export PICO_SDK_PATH=~/pico/pico-sdk
```

### Raspberry Pi

```bash
# Buildroot
git clone git://git.buildroot.net/buildroot
make raspberrypi4_64_defconfig
make
```

---

## 实战项目

1. **温湿度监测站**
   - 硬件: ESP32 + DHT22
   - 协议: MQTT + WiFi
   - 云端: ThingsBoard

2. **电机控制器**
   - 硬件: RPi Pico + 电机驱动
   - 特性: PWM + PID + 编码器

3. **工业数据采集网关**
   - 硬件: 树莓派4 + RS485
   - 协议: Modbus RTU → MQTT
   - 数据库: InfluxDB

4. **超低功耗传感器节点**
   - 硬件: STM32L0 + LoRa
   - 功耗: 休眠 < 10μA
   - 电池寿命: > 5年

---

## 调试技巧

### 串口日志

```c
// ESP32
ESP_LOGI("TAG", "Value: %d", value);

// Pico
printf("Value: %d\n", value);

// 内存受限系统
mini_printf("V:%d\n", value);  // 精简版
```

### JTAG调试

- ESP32: OpenOCD + FT2232
- STM32: ST-Link + GDB
- Pico: Picoprobe

---

## 参考资源

- [ESP-IDF 文档](https://docs.espressif.com/)
- [Pico SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [Buildroot](https://buildroot.org/docs.html)
- [Zephyr RTOS](https://docs.zephyrproject.org/)

---

> **维护者**: C_Lang Knowledge Base Team
> **最后更新**: 2026-03-16
> **状态**: 实战内容持续更新


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
