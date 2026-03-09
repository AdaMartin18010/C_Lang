# 04 Industrial Scenarios - 工业场景

> **对应标准**: Automotive (ISO 26262), 5G (3GPP), Game Dev
> **完成度**: 80% | **预估学习时间**: 120-150小时

---

## 目录结构

### 01_Automotive_ECU - 汽车电子控制单元

汽车嵌入式系统开发。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
<!-- | [01_CAN_Bus_Protocol.md](./01_Automotive_ECU/01_CAN_Bus_Protocol.md) | CAN总线协议 | L4 | ISO 11898 | -->
<!-- | [02_Autosar_Architecture.md](./01_Automotive_ECU/02_Autosar_Architecture.md) | AUTOSAR架构 | L5 | AUTOSAR Classic | -->
<!-- | [03_Functional_Safety.md](./01_Automotive_ECU/03_Functional_Safety.md) | 功能安全 | L5 | ISO 26262 | -->
<!-- | [04_OBD_Diagnostics.md](./01_Automotive_ECU/04_OBD_Diagnostics.md) | OBD诊断 | L4 | ISO 15031, SAE J1979 | -->

**关联标准**: ISO 26262 (ASIL), AUTOSAR, CANopen

---

### 02_Avionics_Systems - 航空电子

航空嵌入式系统。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
<!-- | [01_ARINC_429.md](./02_Avionics_Systems/01_ARINC_429.md) | ARINC 429总线 | L5 | ARINC 429 Spec | -->
<!-- | [02_DO_178C.md](./02_Avionics_Systems/02_DO_178C.md) | 适航认证 | L5 | DO-178C | -->
<!-- | [03_Flight_Control.md](./02_Avionics_Systems/03_Flight_Control.md) | 飞行控制 | L5 | NASA Guidelines | -->

**关联标准**: DO-178C, ARINC 653, MIL-STD-1553

---

### 03_High_Frequency_Trading - 高频交易

超低延迟交易系统。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
| [01_DPDK_Network_Stack.md](./03_High_Frequency_Trading/01_DPDK_Network_Stack.md) | DPDK网络 | L5 | DPDK Documentation |
<!-- | [02_Lock_Free_Queues.md](./03_High_Frequency_Trading/02_Lock_Free_Queues.md) | 无锁队列 | L5 | LMAX Disruptor | -->
<!-- | [03_Kernel_Bypass.md](./03_High_Frequency_Trading/03_Kernel_Bypass.md) | 内核旁路 | L5 | Solarflare/OpenOnload | -->

**关联标准**: FIX Protocol, PTP (IEEE 1588)

---

### 04_5G_Baseband - 5G基带处理

通信系统信号处理。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
| [01_SIMD_Vectorization.md](./04_5G_Baseband/01_SIMD_Vectorization.md) | SIMD向量化 | L5 | ARM NEON, AVX-512 |
| [02_DMA_Offload.md](./04_5G_Baseband/02_DMA_Offload.md) | DMA卸载 | L5 | 3GPP TS 38.211 |
<!-- | [03_MIMO_Detection.md](./04_5G_Baseband/03_MIMO_Detection.md) | MIMO检测 | L5 | 3GPP TS 38.211 | -->

**关联标准**: 3GPP TS 38.xxx (5G NR), IEEE 802.11

---

### 05_Game_Engine - 游戏引擎

实时渲染系统。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
| [01_ECS_Architecture.md](./05_Game_Engine/01_ECS_Architecture.md) | ECS架构 | L5 | Bevy, Unity DOTS |
| [02_Atomic_Operations.md](./05_Game_Engine/02_Atomic_Operations.md) | 原子操作 | L5 | Data-Oriented Design |
<!-- | [03_Entity_Component_System.md](./05_Game_Engine/03_Entity_Component_System.md) | ECS实现 | L5 | Data-Oriented Design | -->

**关联技术**: Data-Oriented Design, SoA, Cache Optimization

---

### 06_Quantum_Computing - 量子计算

量子算法模拟。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
| [01_Quantum_Simulation_C.md](./06_Quantum_Computing/01_Quantum_Simulation_C.md) | 量子模拟 | L5 | Nielsen & Chuang |
| [02_Surface_Code_Decoder.md](./06_Quantum_Computing/02_Surface_Code_Decoder.md) | 表面码解码 | L6 | Quantum Computing |
<!-- | [03_Shor_Algorithm.md](./06_Quantum_Computing/03_Shor_Algorithm.md) | Shor算法 | L6 | Quantum Computing | -->

**关联理论**: Linear Algebra, Complex Numbers, Probability

---

### 09_Space_Computing - 航天计算

抗辐射加固计算。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
| [01_Radiation_Hardening.md](./09_Space_Computing/01_Radiation_Hardening.md) | 抗辐射加固 | L5 | NASA Standards |
| [01_EDAC_Memory.md](./09_Space_Computing/01_EDAC_Memory.md) | EDAC内存 | L5 | Hamming Codes |
| [02_TMR_Voting.md](./09_Space_Computing/02_TMR_Voting.md) | TMR表决 | L5 | Fault Tolerance |

**关联标准**: NASA-STD-8719.13, ECSS-Q-ST-60-02C

---

### 08_Neuromorphic - 神经形态计算

类脑计算系统。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
<!-- | [01_SNN_Simulation.md](./08_Neuromorphic/01_SNN_Simulation.md) | SNN模拟 | L5 | Brian Simulator | -->
| [02_STDP_Learning.md](./08_Neuromorphic/02_STDP_Learning.md) | STDP学习 | L5 | Neuromorphic Engineering |

---

### 09_DNA_Storage - DNA存储

生物信息存储。

| 文件 | 主题 | 难度 | 参考来源 |
|:-----|:-----|:----:|:---------|
| [01_DNA_Synthesis.md](./09_DNA_Storage/01_DNA_Synthesis.md) | DNA合成 | L5 | Nature DNA Storage |
| [02_Error_Correction_Coding.md](./09_DNA_Storage/02_Error_Correction_Coding.md) | 纠错编码 | L5 | Reed-Solomon |

---

## 行业关联图

```mermaid
graph TB
    subgraph Automotive[汽车电子]
        CAN[CAN总线]
        AUTOSAR[AUTOSAR]
        ISO26262[ISO 26262]
    end

    subgraph Telecom[通信]
        5G[5G基带]
        SIMD[SIMD优化]
        MIMO[MIMO检测]
    end

    subgraph Finance[金融]
        HFT[高频交易]
        DPDK[DPDK]
        LockFree[无锁编程]
    end

    subgraph Aerospace[航天]
        RadHard[抗辐射]
        EDAC[EDAC]
        TMR[TMR]
    end

    subgraph Game[游戏]
        ECS[ECS架构]
        DOD[面向数据]
    end

    subgraph Quantum[量子]
        QSim[量子模拟]
        QC[量子算法]
    end
```

---

## 关键行业标准

| 行业 | 标准 | 描述 |
|:-----|:-----|:-----|
| 汽车 | ISO 26262 | 道路车辆功能安全 |
| 汽车 | AUTOSAR | 汽车软件架构标准 |
| 航空 | DO-178C | 机载软件适航认证 |
| 航空 | ARINC 653 | 航空电子分区 |
| 通信 | 3GPP | 移动通信标准 |
| 通用 | IEC 61508 | 工业功能安全 |
| 航天 | NASA-STD | NASA软件工程标准 |
| 金融 | MiFID II | 金融工具市场指令 |

---

## 关联知识库

| 目标 | 路径 |
|:-----|:-----|
| 核心基础 | [01_Core_Knowledge_System](../01_Core_Knowledge_System/README.md) |
| 系统技术 | [03_System_Technology_Domains](../03_System_Technology_Domains/README.md) |
| 理论基础 | [05_Deep_Structure_MetaPhysics](../05_Deep_Structure_MetaPhysics/README.md) |

---

> **最后更新**: 2025-03-09
