# 工业 Linux (Industrial Linux)

> **描述**: 工业环境下Linux嵌入式系统开发指南，涵盖实时Linux(PREEMPT_RT/Xenomai)、EtherCAT主站、软PLC运行时、OPC UA服务器、工业协议栈等关键技术

## 概述

工业Linux是专门为工业控制场景优化的Linux系统，结合了Linux的开放性和工业应用所需的实时性、可靠性。本目录提供工业Linux系统开发的完整知识体系，从实时内核配置到工业协议实现。

现代工业自动化系统越来越倾向于使用Linux作为基础平台，利用其丰富的软件生态和强大的网络功能。理解如何在Linux上实现实时控制、工业通信和数据采集，是工业软件开发人员的核心技能。

## 核心内容

- **实时Linux技术**：PREEMPT_RT补丁原理与配置、Xenomai双内核架构、实时任务调度
- **EtherCAT主站开发**：IgH EtherCAT Master库使用、从站配置、过程数据交换
- **软PLC实现**：PLC运行时架构、IEC 61131-3运行时、扫描周期管理
- **OPC UA服务器**：open62541库使用、地址空间建模、安全通信
- **工业协议栈**：Modbus RTU/TCP主从站、CANopen协议、MQTT物联网通信
- **数据采集系统**：高性能数据采集架构、环形缓冲区、实时数据库
- **边缘计算框架**：边缘节点设计、规则引擎、数据预处理
- **系统可靠性**：看门狗机制、进程监控、OTA远程升级

## 文件列表

| 文件 | 描述 |
|------|------|
| `01_Industrial_Linux_System.md` | 工业Linux系统开发完整指南 |
| `preempt_rt_task.c` | PREEMPT_RT实时任务示例 |
| `xenomai_task.c` | Xenomai硬实时编程示例 |
| `ethercat_master.c` | EtherCAT主站开发示例 |
| `soft_plc_runtime.c` | 软PLC运行时代码 |
| `opcua_server.c` | OPC UA服务器实现 |

## 学习路径

1. **基础阶段**：掌握嵌入式Linux基本开发，理解设备树、驱动开发
2. **实时阶段**：学习PREEMPT_RT配置，编写实时应用程序
3. **协议阶段**：实现Modbus、CANopen等工业通信协议
4. **集成阶段**：整合实时控制、通信、数据采集功能

## 应用场景

- 工业PLC控制器开发
- 运动控制系统
- 工业数据采集网关
- 边缘计算节点
- SCADA系统

---

[← 返回 14_Embedded_IoT 目录](../README.md)
