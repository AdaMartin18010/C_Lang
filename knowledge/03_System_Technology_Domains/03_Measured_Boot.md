# 可信启动与度量启动


---

## 📑 目录

- [可信启动与度量启动](#可信启动与度量启动)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [TPM基础](#tpm基础)
    - [什么是TPM](#什么是tpm)
    - [TPM版本演进](#tpm版本演进)
    - [TPM 2.0架构](#tpm-20架构)
    - [TPM关键能力](#tpm关键能力)
      - [1. 平台配置寄存器 (PCR)](#1-平台配置寄存器-pcr)
      - [2. 密钥层次结构](#2-密钥层次结构)
  - [度量启动流程](#度量启动流程)
    - [度量启动原理](#度量启动原理)
    - [CRTM（Core Root of Trust for Measurement）](#crtmcore-root-of-trust-for-measurement)
    - [启动度量序列](#启动度量序列)
  - [PCR寄存器详解](#pcr寄存器详解)
    - [PCR寄存器分配](#pcr寄存器分配)
    - [PCR扩展操作](#pcr扩展操作)
    - [PCR引用与验证](#pcr引用与验证)
  - [安全启动链](#安全启动链)
    - [信任链构建](#信任链构建)
    - [远程证明（Remote Attestation）](#远程证明remote-attestation)
  - [UEFI Secure Boot](#uefi-secure-boot)
    - [安全启动原理](#安全启动原理)
    - [签名验证数据库](#签名验证数据库)
    - [Secure Boot与度量启动结合](#secure-boot与度量启动结合)
  - [Linux IMA（Integrity Measurement Architecture）](#linux-imaintegrity-measurement-architecture)
    - [IMA架构](#ima架构)
    - [IMA运行模式](#ima运行模式)
    - [IMA策略配置](#ima策略配置)
    - [IMA与TPM集成](#ima与tpm集成)
    - [EVM（Extended Verification Module）](#evmextended-verification-module)
  - [实际应用示例](#实际应用示例)
    - [示例1：启用度量启动（Ubuntu）](#示例1启用度量启动ubuntu)
    - [示例2：生成TPM引用（Quote）](#示例2生成tpm引用quote)
    - [示例3：配置IMA强制模式](#示例3配置ima强制模式)
  - [安全最佳实践](#安全最佳实践)
    - [1. 密钥管理](#1-密钥管理)
    - [2. PCR选择](#2-pcr选择)
    - [3. 撤销与更新](#3-撤销与更新)
  - [相关技术对比](#相关技术对比)
  - [参考资源](#参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

可信启动（Trusted Boot）和度量启动（Measured Boot）是保障系统启动过程安全性的核心技术。
通过建立从硬件到操作系统的完整信任链，确保系统启动过程中的每个组件都经过验证，防止恶意软件在启动阶段植入系统。

## TPM基础

### 什么是TPM

可信平台模块（Trusted Platform Module, TPM）是一种专用的安全加密处理器，设计用于保护硬件安全。TPM芯片提供以下核心功能：

- **密钥生成与存储**：生成和存储非对称密钥对
- **平台完整性度量**：记录系统启动状态
- **安全密钥管理**：保护密钥不被非法访问
- **随机数生成**：提供高质量的随机数

### TPM版本演进

| 版本 | 特性 | 发布年份 |
|------|------|----------|
| TPM 1.1 | 基本功能，SHA-1哈希 | 2003 |
| TPM 1.2 | 增强功能，新增命令 | 2009 |
| TPM 2.0 | 算法敏捷性，增强授权 | 2014 |

### TPM 2.0架构

```
┌─────────────────────────────────────────┐
│            TPM 2.0 架构                  │
├─────────────────────────────────────────┤
│  ┌─────────┐  ┌─────────┐  ┌─────────┐ │
│  │ 密码引擎 │  │ 密钥管理 │  │ 随机数  │ │
│  │ (Crypto)│  │ (Key)   │  │ (RNG)   │ │
│  └────┬────┘  └────┬────┘  └────┬────┘ │
│       └─────────────┼─────────────┘     │
│                     ▼                   │
│  ┌─────────────────────────────────┐   │
│  │        授权与会话管理            │   │
│  │    (Authorization & Sessions)   │   │
│  └────────────────┬────────────────┘   │
│                   ▼                     │
│  ┌─────────────────────────────────┐   │
│  │      PCR寄存器与NV存储           │   │
│  │   (PCR & Non-Volatile Memory)   │   │
│  └─────────────────────────────────┘   │
└─────────────────────────────────────────┘
```

### TPM关键能力

#### 1. 平台配置寄存器 (PCR)

PCR是TPM内部用于存储度量值的特殊寄存器，具有抗篡改特性：

- **PCR特性**：
  - 只能扩展（Extend），不能直接写入
  - 扩展操作：`PCR_new = Hash(PCR_old || Data)`
  - 重启后可通过特定授权重置

- **PCR银行（Banks）**：
  - SHA-1银行：24个寄存器
  - SHA-256银行：24个寄存器
  - 可同时存在多个算法银行

#### 2. 密钥层次结构

```
┌──────────────────────────────────────┐
│         TPM密钥层次结构               │
├──────────────────────────────────────┤
│                                      │
│  ┌──────────────────────────────┐   │
│  │    Primary Key (主密钥)      │   │
│  │   (存储根密钥 - SRK)         │   │
│  │      TPM_GENERATED           │   │
│  └──────────────┬───────────────┘   │
│                 │                    │
│         ┌───────┴───────┐           │
│         ▼               ▼           │
│  ┌───────────┐    ┌───────────┐     │
│  │ Storage   │    │  Signing  │     │
│  │  Keys     │    │   Keys    │     │
│  └─────┬─────┘    └─────┬─────┘     │
│        │                │           │
│   ┌────┴────┐      ┌────┴────┐      │
│   ▼         ▼      ▼         ▼      │
│ 子密钥1   子密钥2  签名1    签名2    │
│                                      │
└──────────────────────────────────────┘
```

## 度量启动流程

### 度量启动原理

度量启动通过链式度量（Chain of Trust）建立信任链，每个启动组件在加载下一个组件前，先计算其哈希值并扩展到PCR中。

```
┌─────────────────────────────────────────────────────────────┐
│                     度量启动流程                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐ │
│   │  CRTM   │───▶│   BIOS  │───▶│  Boot   │───▶│   OS    │ │
│   │ (信任根) │    │  (固件) │    │ Loader  │    │ Kernel  │ │
│   └────┬────┘    └────┬────┘    └────┬────┘    └────┬────┘ │
│        │              │              │              │      │
│        ▼              ▼              ▼              ▼      │
│   ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐ │
│   │ 度量BIOS │    │度量Option│    │ 度量    │    │ 度量    │ │
│   │         │    │   ROM   │    │ Bootloader│   │  Kernel │ │
│   └────┬────┘    └────┬────┘    └────┬────┘    └────┬────┘ │
│        │              │              │              │      │
│        ▼              ▼              ▼              ▼      │
│   ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐ │
│   │ PCR[0]  │    │ PCR[1]  │    │ PCR[4]  │    │ PCR[9]  │ │
│   │ 固件度量 │    │ 配置度量 │    │ 引导代码 │    │ 内核度量 │ │
│   └─────────┘    └─────────┘    └─────────┘    └─────────┘ │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### CRTM（Core Root of Trust for Measurement）

CRTM是度量启动的信任根，是不可篡改的代码：

- **静态CRTM (S-RTM)**：系统启动时执行，从BIOS开始度量
- **动态CRTM (D-RTM)**：运行时建立，通过Intel TXT或AMD-V技术实现

### 启动度量序列

| 启动阶段 | 被度量组件 | 目标PCR | 说明 |
|---------|-----------|--------|------|
| 固件 | BIOS/UEFI固件代码 | PCR[0] | 固件完整性 |
| 配置 | UEFI配置变量 | PCR[1] | 配置完整性 |
| 选项ROM | 设备扩展ROM | PCR[2] | 扩展固件 |
| 引导管理器 | UEFI引导管理器 | PCR[4] | 引导代码 |
| 引导加载器 | GRUB/systemd-boot | PCR[8] | 引导加载器 |
| 内核 | Linux内核镜像 | PCR[9] | 内核代码 |
| 命令行 | 内核启动参数 | PCR[12] | 启动配置 |
| 模块 | 内核模块 | PCR[10] | 可扩展模块 |

## PCR寄存器详解

### PCR寄存器分配

TPM 2.0定义了24个PCR寄存器（0-23），标准用途如下：

```
PCR索引    标准用途                              扩展来源
─────────────────────────────────────────────────────────────
PCR[0]     固件代码度量（BIOS/UEFI）              CRTM/BIOS
PCR[1]     固件配置和可变数据                      BIOS
PCR[2]     选项ROM代码                            扩展固件
PCR[3]     选项ROM配置                            扩展固件
PCR[4]     引导管理器代码                         UEFI
PCR[5]     引导管理器配置                         UEFI
PCR[6]     状态转换和唤醒事件                      ACPI
PCR[7]     安全启动策略（Secure Boot）             UEFI
PCR[8-15]  操作系统定义用途                        OS
PCR[16]    调试保留                               -
PCR[17-22] 动态信任根（D-RTM）                     TXT/SKINIT
PCR[23]    应用程序特定                            App
```

### PCR扩展操作

```c
// TPM2 PCR_Extend 操作伪代码
void pcr_extend(TPM2_HANDLE pcr_handle, TPM2B_DIGEST *data) {
    // 1. 读取当前PCR值
    TPM2B_DIGEST current_pcr = tpm2_pcr_read(pcr_handle);

    // 2. 连接旧值与新数据
    TPM2B_DIGEST concat_data;
    memcpy(concat_data.buffer, current_pcr.buffer, current_pcr.size);
    memcpy(concat_data.buffer + current_pcr.size, data->buffer, data->size);
    concat_data.size = current_pcr.size + data->size;

    // 3. 计算哈希
    TPM2B_DIGEST new_pcr = hash_sha256(concat_data.buffer, concat_data.size);

    // 4. 写入PCR
    tpm2_pcr_write(pcr_handle, &new_pcr);
}
```

### PCR引用与验证

```bash
# 读取PCR值（Linux示例）

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../README.md](../README.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 系统技术领域
**难度等级**: L3-L5
**前置依赖**: 核心知识体系
**后续延伸**: 工业场景应用

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
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/README.md](../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/README.md](../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/README.md](../04_Industrial_Scenarios/README.md) | 工业实践场景 |

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
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
tpm2_pcrread

# 输出示例：
sha256:
  0 : 0x3D3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F3F
  1 : 0x2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E2E
  4 : 0x5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A
  7 : 0x7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
  9 : 0x8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B8B
```

## 安全启动链

### 信任链构建

```
┌─────────────────────────────────────────────────────────────┐
│                      信任链层次                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Layer 0: 硬件信任根 (Hardware Root of Trust)                │
│     └── TPM + 安全启动硬件支持                               │
│                    ▼                                        │
│  Layer 1: 固件信任根 (Firmware Root of Trust)                │
│     └── 经过验证的UEFI/BIOS固件                              │
│                    ▼                                        │
│  Layer 2: 引导加载器信任 (Bootloader Trust)                  │
│     └── 签名的引导加载器 (GRUB/systemd-boot)                 │
│                    ▼                                        │
│  Layer 3: 操作系统信任 (Operating System Trust)              │
│     └── 验证的内核和初始RAM磁盘                              │
│                    ▼                                        │
│  Layer 4: 应用程序信任 (Application Trust)                   │
│     └── 用户空间应用程序验证                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 远程证明（Remote Attestation）

度量启动的最终目标是实现远程证明，允许验证方确认系统的启动状态：

```
┌──────────────┐                              ┌──────────────┐
│   客户端      │                              │   证明服务器  │
│  (Attestor)  │◄────── 挑战/Nonce ──────────▶│  (Verifier)  │
└──────┬───────┘                              └──────┬───────┘
       │                                            │
       │  1. 收集PCR值                               │
       │  2. 使用AIK签名                             │
       │  3. 生成引用（Quote）                        │
       ▼                                            │
┌──────────────┐                                    │
│  TPM Quote:  │                                    │
│  - PCR值     │                                    │
│  - Nonce     │                                    │
│  - 签名      │                                    │
└──────┬───────┘                                    │
       │                                            │
       └──────────── Quote ────────────────────────▶│
                                                    ▼
                                          ┌──────────────┐
                                          │  验证签名     │
                                          │  比对PCR值    │
                                          │  决策授权     │
                                          └──────────────┘
```

## UEFI Secure Boot

### 安全启动原理

UEFI Secure Boot通过数字签名验证每个启动组件：

```
┌─────────────────────────────────────────────────────────────┐
│                  UEFI Secure Boot流程                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐                                            │
│  │ Platform Key│  ← 平台所有者证书（通常由OEM持有）          │
│  │    (PK)     │                                            │
│  └──────┬──────┘                                            │
│         │ 签署                                               │
│         ▼                                                   │
│  ┌─────────────┐                                            │
│  │  Key Exchange│  ← 交换密钥，可包含多个                    │
│  │    Key (KEK) │     - Microsoft KEK                        │
│  └──────┬──────┘      - 自定义KEK                            │
│         │ 签署                                               │
│         ▼                                                   │
│  ┌─────────────┐                                            │
│  │   DB (允许)  │  ← 签名数据库                              │
│  │   DBX (禁止) │  ← 被撤销签名                              │
│  └──────┬──────┘                                            │
│         │ 验证                                               │
│         ▼                                                   │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐     │
│  │  UEFI驱动   │───▶│  引导加载器  │───▶│  操作系统   │     │
│  │  Option ROM │    │  shim/grub  │    │  内核      │     │
│  └─────────────┘    └─────────────┘    └─────────────┘     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 签名验证数据库

| 数据库 | 用途 | 内容 |
|-------|------|------|
| **PK** | Platform Key | 平台所有者公钥证书 |
| **KEK** | Key Exchange Key | 用于更新DB/DBX的密钥 |
| **DB** | Signature Database | 信任的代码签名证书 |
| **DBX** | Forbidden Database | 被撤销的签名/哈希 |

### Secure Boot与度量启动结合

```bash
# 在Secure Boot启用时，PCR[7]记录安全启动状态
tpm2_pcrread sha256:7

# 典型PCR[7]内容包含：
# - Secure Boot启用状态
# - 使用的证书哈希
# - 策略配置
```

## Linux IMA（Integrity Measurement Architecture）

### IMA架构

IMA是Linux内核的完整性度量框架，扩展了度量启动到运行时：

```
┌─────────────────────────────────────────────────────────────┐
│                   Linux IMA架构                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                  User Space                          │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────────────────┐  │   │
│  │  │  ima-evm-utils │  │ auditd  │  │  TEE/TPM工具        │  │   │
│  │  └─────────┘  └─────────┘  └─────────────────────┘  │   │
│  └────────────────────────┬────────────────────────────┘   │
│                           │                                 │
│  ┌────────────────────────▼────────────────────────────┐   │
│  │                  Kernel Space                        │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  │   │
│  │  │   IMA-Measure │  │   IMA-Appraise │  │   EVM        │  │   │
│  │  │   (度量)     │  │   (评估)      │  │  (扩展验证)  │  │   │
│  │  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘  │   │
│  │         │                │                │         │   │
│  │  ┌──────▼────────────────▼────────────────▼──────┐  │   │
│  │  │              IMA Policy Engine                  │  │   │
│  │  │   (测量/评估策略配置：tcb/appraise_tcb等)       │  │   │
│  │  └───────────────────┬───────────────────────────┘  │   │
│  │                      │                                │   │
│  │  ┌───────────────────▼───────────────────────────┐  │   │
│  │  │              TPM Driver/iTPM                   │  │   │
│  │  │         (与TPM芯片交互)                        │  │   │
│  │  └───────────────────────────────────────────────┘  │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Hardware (TPM 1.2/2.0)                  │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### IMA运行模式

| 模式 | 功能 | 用途 |
|------|------|------|
| **measure** | 度量文件访问，扩展到PCR[10] | 审计和证明 |
| **appraise** | 验证文件完整性签名 | 强制完整性 |
| **audit** | 记录访问但不度量 | 合规审计 |
| **hash** | 计算文件哈希存储在xattr | 后续验证 |

### IMA策略配置

```bash
# /etc/ima/ima-policy 示例
# 度量所有可执行文件
dont_measure fsmagic=0x9fa0  # 跳过sysfs
dont_measure fsmagic=0x62656572  # 跳过sysfs
measure func=FILE_CHECK mask=MAY_EXEC
measure func=MODULE_CHECK
measure func=FIRMWARE_CHECK
```

### IMA与TPM集成

```bash
# 查看IMA度量日志
cat /sys/kernel/security/ima/ascii_runtime_measurements

# 输出格式：PCR 模板哈希 模板 文件名 文件哈希
# 示例：
# 10 6a0f8d3... ima 5a3b2c1... /usr/bin/bash
# 10 7b1e9f4... ima 8c2d3e5... /usr/sbin/sshd

# 验证PCR[10]包含IMA度量
tpm2_pcrread sha256:10
```

### EVM（Extended Verification Module）

EVM补充IMA，提供文件元数据完整性保护：

```bash
# 设置EVM属性（需要IMA密钥）
evmctl sign --imasig --key private.pem /usr/bin/critical-app

# 验证EVM属性
getfattr -d -m . /usr/bin/critical-app
# 输出：
# security.ima=0sAQH2dX...
# security.evm=0sAQIDBA...
```

## 实际应用示例

### 示例1：启用度量启动（Ubuntu）

```bash
# 1. 安装必要工具
sudo apt install tpm2-tools tpm2-abrmd

# 2. 验证TPM可用性
tpm2_getcap properties-fixed

# 3. 配置GRUB启用度量
sudo grub-install --target=x86_64-efi --efi-directory=/boot/efi \
    --modules="tpm"

# 4. 更新GRUB配置
sudo update-grub

# 5. 验证启动度量
tpm2_pcrread
```

### 示例2：生成TPM引用（Quote）

```bash
# 1. 创建AIK密钥
tpm2_createprimary -C e -c primary.ctx
tpm2_create -C primary.ctx -u aik.pub -r aik.priv
tpm2_load -C primary.ctx -u aik.pub -r aik.priv -c aik.ctx

# 2. 生成引用（包含PCR 0-7和nonce）
tpm2_quote -c aik.ctx -l sha256:0,1,2,4,7 \
    -q 1234567890abcdef -m quote.msg -s quote.sig

# 3. 验证引用
tpm2_checkquote -u aik.pub -m quote.msg -s quote.sig \
    -g sha256 -q 1234567890abcdef
```

### 示例3：配置IMA强制模式

```bash
# 1. 内核启动参数
ima_appraise=enforce ima_policy=tcb ima_hash=sha256

# 2. 签名关键文件
find /usr/bin -type f -exec evmctl sign --imasig {} \;

# 3. 验证IMA策略
cat /sys/kernel/security/ima/policy
```

## 安全最佳实践

### 1. 密钥管理

- 使用独立CA签署引导组件
- 定期轮换AIK和签名密钥
- 安全存储EK（Endorsement Key）证书

### 2. PCR选择

- PCR[0-7]：保留给固件和引导管理器
- PCR[8-9]：用于引导加载器和内核
- PCR[10]：IMA运行时度量
- PCR[16-23]：应用程序自定义使用

### 3. 撤销与更新

- 维护DBX数据库防止回滚攻击
- 使用防回滚计数器（NV counters）
- 建立安全的固件更新机制

## 相关技术对比

| 特性 | 度量启动 | 安全启动 | IMA |
|------|---------|---------|-----|
| 验证时机 | 启动时 | 启动时 | 启动+运行时 |
| 验证方式 | 哈希度量 | 数字签名 | 哈希+签名 |
| 信任基础 | TPM | PKI证书链 | TPM+本地密钥 |
| 防篡改 | 检测 | 阻止执行 | 阻止执行 |
| 远程证明 | 支持 | 不支持 | 支持 |

## 参考资源

- [TCG TPM 2.0 Specification](https://trustedcomputinggroup.org/resource/tpm-2-0-library-specification/)
- [UEFI Specification](https://uefi.org/specifications)
- [Linux IMA Documentation](https://www.kernel.org/doc/html/latest/security/IMA-templates.html)
- [NIST SP 800-155](https://csrc.nist.gov/publications/detail/white-paper/2011/12/16/bios-integrity-guidelines)

---

> **状态**: ✅ 已完成


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
