# 容器化嵌入式开发完整指南


> **版本**: v1.0.0
> **最后更新**: 2026-03-25
> **作者**: C_Lang Team
> **难度**: L3 进阶

---

## 目录

- [容器化嵌入式开发完整指南](#容器化嵌入式开发完整指南)
  - [目录](#目录)
  - [1. 容器化概述](#1-容器化概述)
    - [1.1 为什么嵌入式开发需要容器化](#11-为什么嵌入式开发需要容器化)
      - [传统嵌入式开发的挑战](#传统嵌入式开发的挑战)
      - [容器化带来的解决方案](#容器化带来的解决方案)
      - [容器化嵌入式开发的核心优势](#容器化嵌入式开发的核心优势)
  - [2. Docker基础](#2-docker基础)
    - [2.1 Docker安装与配置](#21-docker安装与配置)
      - [Ubuntu/Debian 安装](#ubuntudebian-安装)
      - [Windows 安装 (PowerShell)](#windows-安装-powershell)
      - [macOS 安装](#macos-安装)
    - [2.2 Docker 核心概念](#22-docker-核心概念)
      - [镜像层结构详解](#镜像层结构详解)
      - [Dockerfile 指令速查表](#dockerfile-指令速查表)
    - [2.3 Docker 网络配置](#23-docker-网络配置)
  - [3. 交叉编译容器](#3-交叉编译容器)
    - [3.1 ARM GCC 工具链容器](#31-arm-gcc-工具链容器)
      - [基础 ARM GCC 容器](#基础-arm-gcc-容器)
      - [多架构支持容器](#多架构支持容器)
    - [3.2 构建脚本](#32-构建脚本)
  - [4. 嵌入式Linux构建容器](#4-嵌入式linux构建容器)
    - [4.1 Buildroot 构建环境](#41-buildroot-构建环境)
      - [Buildroot 使用示例](#buildroot-使用示例)
    - [4.2 Yocto Project 构建环境](#42-yocto-project-构建环境)
  - [5. VS Code Dev Container配置](#5-vs-code-dev-container配置)
    - [5.1 基础 Dev Container 配置](#51-基础-dev-container-配置)
    - [5.2 Dev Container Dockerfile](#52-dev-container-dockerfile)
    - [5.3 容器创建后脚本](#53-容器创建后脚本)
  - [6. GitHub Codespaces用于嵌入式](#6-github-codespaces用于嵌入式)
    - [6.1 Codespaces 配置](#61-codespaces-配置)
    - [6.2 Codespaces 到本地设备调试](#62-codespaces-到本地设备调试)
  - [7. Docker Compose多服务开发](#7-docker-compose多服务开发)
    - [7.1 完整开发环境配置](#71-完整开发环境配置)
    - [7.2 使用脚本](#72-使用脚本)
  - [8. 容器化测试](#8-容器化测试)
    - [8.1 QEMU 测试容器](#81-qemu-测试容器)
    - [8.2 QEMU 测试配置](#82-qemu-测试配置)
    - [8.3 自动化测试脚本](#83-自动化测试脚本)
  - [9. CI/CD流水线](#9-cicd流水线)
    - [9.1 GitHub Actions 完整配置](#91-github-actions-完整配置)
    - [9.2 GitLab CI 配置](#92-gitlab-ci-配置)
  - [10. 安全扫描](#10-安全扫描)
    - [10.1 Trivy 安全配置](#101-trivy-安全配置)
    - [10.2 GitHub Actions 安全扫描工作流](#102-github-actions-安全扫描工作流)
    - [10.3 安全扫描 Docker Compose](#103-安全扫描-docker-compose)
  - [11. 实际项目：完整容器化STM32开发环境](#11-实际项目完整容器化stm32开发环境)
    - [11.1 项目结构](#111-项目结构)
    - [11.2 完整 Dockerfile](#112-完整-dockerfile)
    - [11.3 Docker Compose 完整配置](#113-docker-compose-完整配置)
    - [11.4 项目管理脚本](#114-项目管理脚本)
    - [11.5 使用示例](#115-使用示例)
  - [附录](#附录)
    - [A. 常见问题解答](#a-常见问题解答)
      - [Q1: 如何解决 USB 设备权限问题？](#q1-如何解决-usb-设备权限问题)
      - [Q2: 如何在 macOS 上使用 USB 设备？](#q2-如何在-macos-上使用-usb-设备)
      - [Q3: 如何减小镜像大小？](#q3-如何减小镜像大小)
    - [B. 参考资源](#b-参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. 容器化概述

### 1.1 为什么嵌入式开发需要容器化

#### 传统嵌入式开发的挑战

```
┌─────────────────────────────────────────────────────────────────────┐
│                    传统嵌入式开发环境挑战                              │
├─────────────────────────────────────────────────────────────────────┤
│  ┌──────────────┐   ┌──────────────┐   ┌──────────────┐             │
│  │  开发者A      │   │  开发者B      │   │  CI/CD服务器  │             │
│  │  Ubuntu 20.04│   │  Windows 11  │   │  Ubuntu 22.04│             │
│  │  GCC 9.3     │   │  GCC 10.2    │   │  GCC 11.3    │             │
│  │  OpenOCD 0.11│   │  OpenOCD 0.12│   │  OpenOCD 0.10│             │
│  └──────┬───────┘   └──────┬───────┘   └──────┬───────┘             │
│         │                  │                  │                      │
│         └──────────────────┼──────────────────┘                      │
│                            ▼                                        │
│                    ┌───────────────┐                                │
│                    │  编译结果不一致  │                                │
│                    │  环境问题难以复现  │                                │
│                    │  新人上手成本高   │                                │
│                    └───────────────┘                                │
└─────────────────────────────────────────────────────────────────────┘
```

#### 容器化带来的解决方案

```
┌─────────────────────────────────────────────────────────────────────┐
│                    容器化嵌入式开发环境                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│    ┌─────────────────────────────────────────────────────┐         │
│    │           Docker 镜像 (版本化/可复现)                  │         │
│    │  ┌─────────────────────────────────────────────┐     │         │
│    │  │  基础镜像: ubuntu:22.04                      │     │         │
│    │  │  ├── GCC ARM Embedded 12.2.rel1             │     │         │
│    │  │  ├── OpenOCD 0.12.0                         │     │         │
│    │  │  ├── STM32CubeMX 6.9.0                      │     │         │
│    │  │  ├── CMake 3.27+                            │     │         │
│    │  │  ├── Ninja 1.11+                            │     │         │
│    │  │  └── Python 3.11 + 工具脚本                  │     │         │
│    │  └─────────────────────────────────────────────┘     │         │
│    └────────────────────────┬────────────────────────────┘         │
│                             │                                       │
│         ┌───────────────────┼───────────────────┐                  │
│         ▼                   ▼                   ▼                  │
│    ┌─────────┐        ┌─────────┐        ┌─────────┐              │
│    │开发者A   │        │开发者B   │        │CI/CD    │              │
│    │容器实例  │        │容器实例  │        │容器实例  │              │
│    └─────────┘        └─────────┘        └─────────┘              │
│                                                                     │
│    ✅ 环境完全一致    ✅ 快速启动    ✅ 版本可控    ✅ 易于扩展        │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

#### 容器化嵌入式开发的核心优势

| 优势 | 描述 | 实际收益 |
|------|------|----------|
| **环境一致性** | 开发、测试、生产环境完全一致 | 消除"在我机器上可以运行"问题 |
| **快速 onboarding** | 新员工几分钟内搭建完整环境 | 从数天缩短到数分钟 |
| **版本控制** | 工具链版本与代码版本绑定 | 可重现任意历史构建 |
| **隔离性** | 不同项目使用不同工具链版本 | 避免版本冲突 |
| **可扩展性** | 轻松添加新工具或更新版本 | 持续集成现代工具 |
| **跨平台** | 支持 Windows/macOS/Linux | 统一开发体验 |

---

## 2. Docker基础

### 2.1 Docker安装与配置

#### Ubuntu/Debian 安装

```bash
#!/bin/bash
### docker_install_ubuntu.sh

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
**当前模块**: 现代工具链
**难度等级**: L2-L4
**前置依赖**: 核心知识体系
**后续延伸**: 工程实践

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

## 🔗 知识关联网络

### 1. 全局导航
| 层级 | 文档 | 作用 |
|:-----|:-----|:-----|
| 总索引 | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口 |
| 本模块 | [../../readme.md](../../readme.md) | 模块总览与导航 |
| 学习路径 | [../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 推荐学习路线 |

### 2. 前置知识依赖
| 文档 | 关系 | 掌握要求 |
|:-----|:-----|:---------|
| [../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md](../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 语言基础 | 必须掌握 |
| [../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心机制 | 必须掌握 |
| [../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 内存基础 | 必须掌握 |

### 3. 同层横向关联
| 文档 | 关系 | 互补内容 |
|:-----|:-----|:---------|
| [../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 技术扩展 | 并发编程技术 |
| [../../02_Formal_Semantics_and_Physics/readme.md](../../02_Formal_Semantics_and_Physics/readme.md) | 理论支撑 | 形式化方法 |
| [../../04_Industrial_Scenarios/readme.md](../../04_Industrial_Scenarios/readme.md) | 实践应用 | 工业实践案例 |

### 4. 深层理论关联
| 文档 | 关系 | 理论深度 |
|:-----|:-----|:---------|
| [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义基础 | 操作语义、类型理论 |
| [../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 概念映射 | 知识关联网络 |

### 5. 后续进阶延伸
| 文档 | 关系 | 进阶方向 |
|:-----|:-----|:---------|
| [../../03_System_Technology_Domains/readme.md](../../03_System_Technology_Domains/readme.md) | 系统技术 | 系统级开发 |
| [../../01_Core_Knowledge_System/09_Safety_Standards/readme.md](../../01_Core_Knowledge_System/09_Safety_Standards/readme.md) | 安全标准 | 安全编码规范 |
| [../../07_Modern_Toolchain/readme.md](../../07_Modern_Toolchain/readme.md) | 工具链 | 现代开发工具 |

### 6. 阶段学习定位
```

当前位置: 根据文档主题确定学习阶段
├─ 入门阶段: 基础语法、数据类型
├─ 基础阶段: 控制流程、函数
├─ 进阶阶段: 指针、内存管理 ⬅️ 可能在此
├─ 高级阶段: 并发、系统编程
└─ 专家阶段: 形式验证、编译器

```

### 7. 局部索引
本文件所属模块的详细内容：
- 参见本模块 [readme.md](../../readme.md)
- 相关子目录文档

### Docker 安装脚本 - Ubuntu/Debian 系统

set -e

echo "=== Docker 安装脚本 ==="

### 1. 卸载旧版本
echo "[1/6] 卸载旧版本 Docker..."
sudo apt-get remove -y docker docker-engine docker.io containerd runc 2>/dev/null || true

### 2. 更新 apt 包索引
echo "[2/6] 更新包索引..."
sudo apt-get update

### 3. 安装依赖包
echo "[3/6] 安装依赖包..."
sudo apt-get install -y \
    ca-certificates \
    curl \
    gnupg \
    lsb-release \
    software-properties-common

### 4. 添加 Docker 官方 GPG 密钥
echo "[4/6] 添加 Docker GPG 密钥..."
sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | \
    sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo chmod a+r /etc/apt/keyrings/docker.gpg

### 5. 设置稳定版仓库
echo "[5/6] 设置 Docker 仓库..."
echo \
  "deb [arch="$(dpkg --print-architecture)" signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/ubuntu \
  "$(. /etc/os-release && echo "$VERSION_CODENAME")" stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

### 6. 安装 Docker Engine
echo "[6/6] 安装 Docker Engine..."
sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io \
    docker-buildx-plugin docker-compose-plugin

### 7. 将当前用户添加到 docker 组
echo "[7/6] 配置用户权限..."
sudo usermod -aG docker $USER

### 8. 启动 Docker 服务
sudo systemctl enable docker
sudo systemctl start docker

echo "=== Docker 安装完成 ==="
echo "请注销并重新登录以应用组权限更改"
echo "验证安装: docker run hello-world"
```

#### Windows 安装 (PowerShell)

```powershell
### docker_install_windows.ps1
### Docker Desktop 安装脚本 - Windows

### 检查系统要求
$osInfo = Get-CimInstance -ClassName Win32_OperatingSystem
$totalMemory = [math]::Round($osInfo.TotalVisibleMemorySize / 1MB, 2)

Write-Host "=== Docker Desktop 安装检查 ===" -ForegroundColor Green
Write-Host "操作系统: $($osInfo.Caption)"
Write-Host "内存: $totalMemory GB"

### 检查 WSL2
$wslInstalled = Get-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux
if ($wslInstalled.State -ne "Enabled") {
    Write-Host "启用 WSL2..." -ForegroundColor Yellow
    wsl --install
    Write-Host "请重启计算机后再次运行此脚本" -ForegroundColor Red
    exit
}

### 下载 Docker Desktop
$dockerUrl = "https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe"
$installerPath = "$env:TEMP\DockerDesktopInstaller.exe"

Write-Host "下载 Docker Desktop..." -ForegroundColor Yellow
Invoke-WebRequest -Uri $dockerUrl -OutFile $installerPath

### 安装 Docker Desktop
Write-Host "安装 Docker Desktop..." -ForegroundColor Yellow
Start-Process -FilePath $installerPath -ArgumentList "install", "--quiet" -Wait

Write-Host "=== 安装完成 ===" -ForegroundColor Green
Write-Host "请重启计算机完成安装"
```

#### macOS 安装

```bash
#!/bin/bash
### docker_install_macos.sh
### Docker Desktop 安装 - macOS

echo "=== Docker Desktop macOS 安装 ==="

### 检查 Homebrew
if ! command -v brew &> /dev/null; then
    echo "安装 Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

### 使用 Homebrew 安装 Docker Desktop
echo "通过 Homebrew 安装 Docker Desktop..."
brew install --cask docker

echo "=== 安装完成 ==="
echo "请从应用程序文件夹启动 Docker Desktop"
echo "验证安装: docker --version"
```

### 2.2 Docker 核心概念

#### 镜像层结构详解

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Docker 镜像层结构                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │  Layer 5: Application Layer (rw)                            │   │
│  │  ├── /workspace/src/                                        │   │
│  │  ├── /workspace/build/                                      │   │
│  │  └── 容器运行时写入层 (Container Layer)                      │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │  Layer 4: Toolchain Layer (ro)                              │   │
│  │  ├── /opt/gcc-arm-none-eabi-12.2/                           │   │
│  │  ├── /opt/openocd/                                          │   │
│  │  └── /opt/stm32cubemx/                                      │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │  Layer 3: Build Tools Layer (ro)                            │   │
│  │  ├── cmake, ninja, make                                     │   │
│  │  ├── python3, pip                                           │   │
│  │  └── git, ssh                                               │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │  Layer 2: System Dependencies (ro)                          │   │
│  │  ├── libusb-1.0, libftdi                                    │   │
│  │  ├── libncurses5, libpython3                                │   │
│  │  └── udev rules                                             │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │  Layer 1: Base Image (ro)                                   │   │
│  │  └── ubuntu:22.04                                           │   │
│  ├─────────────────────────────────────────────────────────────┤   │   │
│  │  Layer 0: 基础系统 (ro)                                      │   │
│  │  └── 来自 Docker Hub 的基础镜像                              │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  说明:                                                              │
│  • ro = 只读层 (Read-Only) - 镜像层，可共享和缓存                   │
│  • rw = 读写层 (Read-Write) - 容器层，容器销毁时数据丢失             │
│  • 每层都是前一层的增量变化                                          │
│  • 使用联合文件系统 (UnionFS) 实现                                   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

#### Dockerfile 指令速查表

```dockerfile
### ============================================================================
### Dockerfile 指令详解 - 嵌入式开发专用
### ============================================================================

### ----------------------------------------------------------------------------
## 基础指令
### ----------------------------------------------------------------------------

## FROM - 指定基础镜像，每个 Dockerfile 必须以 FROM 开始
### 语法: FROM [--platform=<platform>] <image>[:<tag>][@<digest>]
FROM ubuntu:22.04

### ARG - 定义构建时变量，仅在构建阶段可用
### 语法: ARG <name>[=<default value>]
ARG TOOLCHAIN_VERSION=12.2.rel1
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=$USER_UID

### ENV - 设置环境变量，构建和运行阶段都可用
### 语法: ENV <key>=<value> ...
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Asia/Shanghai \
    PATH="/opt/gcc-arm-none-eabi/bin:${PATH}"

### LABEL - 添加元数据到镜像
LABEL maintainer="embedded-team@company.com" \
      version="1.0.0" \
      description="STM32 Development Environment"

### ----------------------------------------------------------------------------
### 用户和权限
### ----------------------------------------------------------------------------

### 创建非 root 用户（安全最佳实践）
RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME && \
    apt-get update && \
    apt-get install -y sudo && \
    echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME && \
    chmod 0440 /etc/sudoers.d/$USERNAME

### USER - 切换当前用户
USER $USERNAME

### WORKDIR - 设置工作目录，如果不存在会自动创建
WORKDIR /workspace

### ----------------------------------------------------------------------------
### 文件操作
### ----------------------------------------------------------------------------

### COPY - 从构建上下文复制文件到镜像
### 语法: COPY [options] <src>... <dest>
COPY --chown=$USERNAME:$USERNAME requirements.txt ./
COPY --chown=$USERNAME:$USERNAME scripts/ ./scripts/

### ADD - 类似 COPY，但支持自动解压和 URL
### 建议优先使用 COPY，除非需要 ADD 的特殊功能
ADD --chown=$USERNAME:$USERNAME https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz /opt/

### ----------------------------------------------------------------------------
### 运行命令
### ----------------------------------------------------------------------------

### RUN - 在镜像构建时执行命令
### 语法: RUN [options] <command> ...

### Shell 形式（使用 /bin/sh -c）
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

### Exec 形式（推荐，更明确）
RUN ["apt-get", "update"]
RUN ["apt-get", "install", "-y", "cmake"]

### 合并 RUN 指令减少镜像层数
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    wget \
    curl \
    python3 \
    python3-pip \
    libusb-1.0-0-dev \
    libftdi1-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

### ----------------------------------------------------------------------------
### 容器配置
### ----------------------------------------------------------------------------

### EXPOSE - 声明容器运行时监听的端口
### 语法: EXPOSE <port>[/<protocol>]
EXPOSE 3333/tcp    # GDB 服务器端口
EXPOSE 4444/tcp    # Telnet 端口
EXPOSE 6666/tcp    # TCL 端口

### VOLUME - 创建挂载点，用于持久化数据
### 语法: VOLUME ["/data"]
VOLUME ["/workspace", "/dev/bus/usb"]

### ----------------------------------------------------------------------------
### 容器启动
### ----------------------------------------------------------------------------

### CMD - 容器启动时的默认命令（可被覆盖）
### 语法: CMD ["executable","param1","param2"] (exec 形式)
###       CMD command param1 param2 (shell 形式)
CMD ["/bin/bash"]

### ENTRYPOINT - 容器启动时执行的命令（不易被覆盖）
### 通常与 CMD 结合使用，CMD 作为默认参数
ENTRYPOINT ["docker-entrypoint.sh"]
CMD ["bash"]

### HEALTHCHECK - 检查容器健康状态
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

### SHELL - 更改默认 shell
SHELL ["/bin/bash", "-c"]

### STOPSIGNAL - 设置停止容器的信号
STOPSIGNAL SIGTERM
```

### 2.3 Docker 网络配置

```dockerfile
### docker_network_guide.md
### Docker 网络配置指南 - 嵌入式开发场景

```

```yaml
### networks.yml
### Docker Compose 网络配置示例

version: '3.8'

networks:
  # 桥接网络 - 默认网络模式，容器间通信
  embedded-net:
    driver: bridge
    ipam:
      config:
        - subnet: 172.28.0.0/16
          gateway: 172.28.0.1
    # 启用 IPv6
    enable_ipv6: true
    ipam:
      config:
        - subnet: 172.28.0.0/16
        - subnet: 2001:db8::/32

  # Host 网络 - 与宿主机共享网络命名空间
  # 用于需要直接访问网络接口的场景（如抓包）
  host-net:
    driver: host

  # Macvlan 网络 - 容器获得独立 MAC 地址
  # 用于模拟真实网络设备
  macvlan-net:
    driver: macvlan
    driver_opts:
      parent: eth0
    ipam:
      config:
        - subnet: 192.168.1.0/24
          gateway: 192.168.1.1

services:
  gdb-server:
    image: embedded-gdb:latest
    networks:
      embedded-net:
        ipv4_address: 172.28.0.10
    ports:
      - "3333:3333"      # GDB 端口
      - "4444:4444"      # Telnet 端口
    # 端口范围映射
    # ports:
    #   - "3000-3010:3000-3010"
    #   - "127.0.0.1:8080:8080"  # 仅本地访问

  web-server:
    image: nginx:alpine
    networks:
      - embedded-net
    # DNS 配置
    dns:
      - 8.8.8.8
      - 8.8.4.4
    dns_search:
      - example.com
    # 主机名解析
    extra_hosts:
      - "target-device:192.168.1.100"
      - "debugger:172.28.0.10"
```

---

## 3. 交叉编译容器

### 3.1 ARM GCC 工具链容器


#### 基础 ARM GCC 容器

```dockerfile
### ============================================================================
### Dockerfile.arm-gcc-basic
## 基础 ARM GCC 交叉编译环境
### ============================================================================

### 使用多阶段构建减小最终镜像大小
### 阶段 1: 下载和准备工具链
FROM ubuntu:22.04 AS toolchain-downloader

### 构建参数
ARG ARM_TOOLCHAIN_VERSION=12.2.rel1
ARG ARM_TOOLCHAIN_ARCH=x86_64

### 安装下载工具
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*

### 设置下载目录
WORKDIR /downloads

### 下载 ARM GCC 工具链
### 使用官方 ARM 编译器下载链接
RUN curl -fsSL -o gcc-arm.tar.xz \
    "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_TOOLCHAIN_VERSION}/binrel/arm-gnu-toolchain-${ARM_TOOLCHAIN_VERSION}-${ARM_TOOLCHAIN_ARCH}-arm-none-eabi.tar.xz" \
    && echo "下载完成: gcc-arm.tar.xz" \
    && ls -lh gcc-arm.tar.xz

### 解压工具链
RUN tar -xf gcc-arm.tar.xz -C /opt/ \
    && rm gcc-arm.tar.xz \
    && mv /opt/arm-gnu-toolchain-* /opt/gcc-arm-none-eabi

### ============================================================================
### 阶段 2: 最终运行镜像
### ============================================================================
FROM ubuntu:22.04 AS embedded-builder

### 镜像元数据
LABEL maintainer="embedded-dev@company.com"
LABEL description="ARM GCC Cross Compiler for STM32 Development"
LABEL version="1.0.0"

### 构建参数
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=1000
ARG ARM_TOOLCHAIN_VERSION=12.2.rel1

### 环境变量
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Asia/Shanghai \
    ARM_TOOLCHAIN_VERSION=${ARM_TOOLCHAIN_VERSION} \
    PATH="/opt/gcc-arm-none-eabi/bin:/usr/local/bin:${PATH}" \
    CC=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc \
    CXX=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-g++ \
    AR=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-ar \
    LD=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-ld \
    OBJCOPY=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-objcopy \
    OBJDUMP=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-objdump \
    SIZE=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-size \
    GDB=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gdb

### 安装系统依赖
### 合并 RUN 指令以减少镜像层数
RUN apt-get update && apt-get install -y --no-install-recommends \
    # 基础构建工具
    build-essential \
    make \
    cmake \
    ninja-build \
    # 版本控制
    git \
    git-lfs \
    ssh \
    # 网络工具
    curl \
    wget \
    ca-certificates \
    # Python 环境
    python3 \
    python3-pip \
    python3-venv \
    python3-dev \
    # 调试工具
    gdb-multiarch \
    minicom \
    picocom \
    # USB 设备支持
    libusb-1.0-0 \
    libusb-1.0-0-dev \
    libftdi1-2 \
    libftdi1-dev \
    libhidapi-dev \
    udev \
    # 实用工具
    vim \
    nano \
    less \
    htop \
    tree \
    file \
    # 清理缓存
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

### 从第一阶段复制工具链
COPY --from=toolchain-downloader /opt/gcc-arm-none-eabi /opt/gcc-arm-none-eabi

### 验证工具链安装
RUN arm-none-eabi-gcc --version && \
    arm-none-eabi-g++ --version && \
    arm-none-eabi-gdb --version

### 创建非 root 用户
RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME && \
    usermod -aG dialout $USERNAME && \
    usermod -aG plugdev $USERNAME

### 安装 Python 包（用于嵌入式开发工具）
RUN pip3 install --no-cache-dir --upgrade pip && \
    pip3 install --no-cache-dir \
    pyserial \
    pyusb \
    pyocd \
    mbed-cli \
    platformio \
    imgtool \
    intelhex \
    srecord

### 设置工作目录
WORKDIR /workspace

### 切换非 root 用户
USER $USERNAME

### 默认命令
CMD ["/bin/bash"]
```

#### 多架构支持容器

```dockerfile
### ============================================================================
### Dockerfile.multi-arch
### 支持多架构的交叉编译容器
### 支持: ARM Cortex-M, ARM Cortex-A, RISC-V
### ============================================================================

FROM ubuntu:22.04 AS multi-arch-toolchain

### 构建参数 - 各架构工具链版本
ARG ARM_NONE_EABI_VER=12.2.rel1
ARG ARM_LINUX_GNUEABI_VER=12.2.rel1
ARG RISCV_GNU_VER=2023.07.05

ENV DEBIAN_FRONTEND=noninteractive

## 安装基础工具和依赖
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    curl \
    wget \
    git \
    cmake \
    ninja-build \
    python3 \
    python3-pip \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt

### -----------------------------------------------------------------------------
### 安装 ARM Cortex-M 工具链 (arm-none-eabi)
### 用于: STM32, NXP Kinetis, Nordic nRF52/53, etc.
### -----------------------------------------------------------------------------
RUN echo "=== Installing ARM Cortex-M toolchain ===" && \
    curl -fsSL -o arm-none-eabi.tar.xz \
    "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_NONE_EABI_VER}/binrel/arm-gnu-toolchain-${ARM_NONE_EABI_VER}-x86_64-arm-none-eabi.tar.xz" && \
    tar -xf arm-none-eabi.tar.xz && \
    rm arm-none-eabi.tar.xz && \
    mv arm-gnu-toolchain-*-arm-none-eabi gcc-arm-none-eabi && \
    echo "ARM Cortex-M toolchain installed"

### -----------------------------------------------------------------------------
### 安装 ARM Linux 工具链 (arm-linux-gnueabihf)
### 用于: ARM Linux 开发, Raspberry Pi, BeagleBone
### -----------------------------------------------------------------------------
RUN echo "=== Installing ARM Linux toolchain ===" && \
    curl -fsSL -o arm-linux.tar.xz \
    "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_LINUX_GNUEABI_VER}/binrel/arm-gnu-toolchain-${ARM_LINUX_GNUEABI_VER}-x86_64-arm-none-linux-gnueabihf.tar.xz" && \
    tar -xf arm-linux.tar.xz && \
    rm arm-linux.tar.xz && \
    mv arm-gnu-toolchain-*-arm-none-linux-gnueabihf gcc-arm-linux-gnueabihf && \
    echo "ARM Linux toolchain installed"

### -----------------------------------------------------------------------------
### 安装 ARM64 Linux 工具链 (aarch64-linux-gnu)
### 用于: 64-bit ARM Linux, Raspberry Pi 4/5, Jetson
### -----------------------------------------------------------------------------
RUN echo "=== Installing ARM64 Linux toolchain ===" && \
    curl -fsSL -o aarch64-linux.tar.xz \
    "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_LINUX_GNUEABI_VER}/binrel/arm-gnu-toolchain-${ARM_LINUX_GNUEABI_VER}-x86_64-aarch64-none-linux-gnu.tar.xz" && \
    tar -xf aarch64-linux.tar.xz && \
    rm aarch64-linux.tar.xz && \
    mv arm-gnu-toolchain-*-aarch64-none-linux-gnu gcc-aarch64-linux-gnu && \
    echo "ARM64 Linux toolchain installed"

### -----------------------------------------------------------------------------
### 安装 RISC-V 工具链
### 用于: RISC-V MCU 和 Linux 开发
### -----------------------------------------------------------------------------
RUN echo "=== Installing RISC-V toolchain ===" && \
    curl -fsSL -o riscv.tar.gz \
    "https://github.com/riscv-collab/riscv-gnu-toolchain/releases/download/${RISCV_GNU_VER}/riscv64-elf-ubuntu-22.04-nightly-${RISCV_GNU_VER}-nightly.tar.gz" && \
    tar -xzf riscv.tar.gz && \
    rm riscv.tar.gz && \
    mv riscv riscv64-unknown-elf && \
    echo "RISC-V toolchain installed"

### -----------------------------------------------------------------------------
### 最终镜像
### -----------------------------------------------------------------------------
FROM ubuntu:22.04 AS final

LABEL maintainer="embedded-dev@company.com"
LABEL description="Multi-Architecture Cross Compilation Environment"

ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=1000

### 设置环境变量 - 所有工具链路径
ENV DEBIAN_FRONTEND=noninteractive \
    PATH="/opt/gcc-arm-none-eabi/bin:/opt/gcc-arm-linux-gnueabihf/bin:/opt/gcc-aarch64-linux-gnu/bin:/opt/riscv64-unknown-elf/bin:${PATH}" \
    # ARM Cortex-M
    ARM_NONE_EABI_GCC=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc \
    ARM_NONE_EABI_GDB=/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gdb \
    # ARM Linux (32-bit)
    ARM_LINUX_GCC=/opt/gcc-arm-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc \
    ARM_LINUX_GDB=/opt/gcc-arm-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gdb \
    # ARM64 Linux
    AARCH64_LINUX_GCC=/opt/gcc-aarch64-linux-gnu/bin/aarch64-none-linux-gnu-gcc \
    AARCH64_LINUX_GDB=/opt/gcc-aarch64-linux-gnu/bin/aarch64-none-linux-gnu-gdb \
    # RISC-V
    RISCV_GCC=/opt/riscv64-unknown-elf/bin/riscv64-unknown-elf-gcc \
    RISCV_GDB=/opt/riscv64-unknown-elf/bin/riscv64-unknown-elf-gdb

### 安装运行时依赖
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    make \
    cmake \
    ninja-build \
    git \
    git-lfs \
    python3 \
    python3-pip \
    libusb-1.0-0 \
    libftdi1-2 \
    gdb-multiarch \
    qemu-user-static \
    qemu-system-arm \
    binfmt-support \
    && rm -rf /var/lib/apt/lists/*

### 复制所有工具链
COPY --from=multi-arch-toolchain /opt/gcc-arm-none-eabi /opt/gcc-arm-none-eabi
COPY --from=multi-arch-toolchain /opt/gcc-arm-linux-gnueabihf /opt/gcc-arm-linux-gnueabihf
COPY --from=multi-arch-toolchain /opt/gcc-aarch64-linux-gnu /opt/gcc-aarch64-linux-gnu
COPY --from=multi-arch-toolchain /opt/riscv64-unknown-elf /opt/riscv64-unknown-elf

### 验证所有工具链
RUN echo "=== Verifying toolchains ===" && \
    echo "ARM Cortex-M:" && arm-none-eabi-gcc --version && \
    echo "ARM Linux:" && arm-none-linux-gnueabihf-gcc --version && \
    echo "ARM64 Linux:" && aarch64-none-linux-gnu-gcc --version && \
    echo "RISC-V:" && riscv64-unknown-elf-gcc --version

### 创建用户
RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

### 安装开发工具
RUN pip3 install --no-cache-dir \
    pyserial \
    pyocd \
    platformio

WORKDIR /workspace
USER $USERNAME

CMD ["/bin/bash"]
```

### 3.2 构建脚本

```bash
#!/bin/bash
### build_containers.sh
### 容器构建脚本 - 支持多种配置

set -e

### 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

### 默认配置
DOCKER_REGISTRY=""
IMAGE_PREFIX="embedded"
IMAGE_TAG="latest"
PUSH_IMAGES=false
PLATFORMS="linux/amd64"

### 打印帮助信息
show_help() {
    cat << EOF
Usage: $0 [OPTIONS] [TARGETS]

Build container images for embedded development.

TARGETS:
    arm-gcc         ARM GCC toolchain container
    multi-arch      Multi-architecture toolchain container
    all             Build all containers

OPTIONS:
    -r, --registry REGISTRY    Docker registry URL
    -p, --prefix PREFIX        Image name prefix (default: embedded)
    -t, --tag TAG             Image tag (default: latest)
    --push                    Push images after build
    --platforms PLATFORMS     Target platforms (default: linux/amd64)
                              Example: linux/amd64,linux/arm64
    -h, --help                Show this help message

EXAMPLES:
    $0 arm-gcc                                    # Build ARM GCC container
    $0 all --tag v1.0.0                           # Build all with version tag
    $0 arm-gcc --registry registry.example.com    # With private registry
    $0 multi-arch --platforms linux/amd64,linux/arm64 --push
EOF
}

### 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -r|--registry)
            DOCKER_REGISTRY="$2"
            shift 2
            ;;
        -p|--prefix)
            IMAGE_PREFIX="$2"
            shift 2
            ;;
        -t|--tag)
            IMAGE_TAG="$2"
            shift 2
            ;;
        --push)
            PUSH_IMAGES=true
            shift
            ;;
        --platforms)
            PLATFORMS="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        arm-gcc|multi-arch|all)
            TARGET="$1"
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

### 构建完整的镜像名称
build_image_name() {
    local name=$1
    if [[ -n "$DOCKER_REGISTRY" ]]; then
        echo "${DOCKER_REGISTRY}/${IMAGE_PREFIX}/${name}:${IMAGE_TAG}"
    else
        echo "${IMAGE_PREFIX}/${name}:${IMAGE_TAG}"
    fi
}

### 检查 Docker 是否可用
check_docker() {
    if ! command -v docker &> /dev/null; then
        echo -e "${RED}Error: Docker is not installed${NC}"
        exit 1
    fi

    if ! docker info &> /dev/null; then
        echo -e "${RED}Error: Docker daemon is not running${NC}"
        exit 1
    fi

    echo -e "${GREEN}Docker is ready${NC}"
}

### 设置 buildx（用于多平台构建）
setup_buildx() {
    echo -e "${YELLOW}Setting up Docker Buildx...${NC}"

    # 创建或使用现有的 buildx builder
    docker buildx create --name multi-platform-builder --use 2>/dev/null || \
        docker buildx use multi-platform-builder

    docker buildx inspect --bootstrap
}

### 构建 ARM GCC 容器
build_arm_gcc() {
    local image_name=$(build_image_name "arm-gcc")
    echo -e "${YELLOW}Building ARM GCC container: ${image_name}${NC}"

    docker buildx build \
        --platform "$PLATFORMS" \
        --build-arg ARM_TOOLCHAIN_VERSION=12.2.rel1 \
        --build-arg USER_UID=$(id -u) \
        --build-arg USER_GID=$(id -g) \
        --tag "$image_name" \
        --file Dockerfile.arm-gcc-basic \
        $( [[ "$PUSH_IMAGES" == true ]] && echo "--push" || echo "--load" ) \
        .

    echo -e "${GREEN}ARM GCC container built successfully${NC}"
}

### 构建多架构容器
build_multi_arch() {
    local image_name=$(build_image_name "multi-arch")
    echo -e "${YELLOW}Building Multi-Arch container: ${image_name}${NC}"

    docker buildx build \
        --platform "$PLATFORMS" \
        --build-arg ARM_NONE_EABI_VER=12.2.rel1 \
        --build-arg ARM_LINUX_GNUEABI_VER=12.2.rel1 \
        --build-arg RISCV_GNU_VER=2023.07.05 \
        --tag "$image_name" \
        --file Dockerfile.multi-arch \
        $( [[ "$PUSH_IMAGES" == true ]] && echo "--push" || echo "--load" ) \
        .

    echo -e "${GREEN}Multi-Arch container built successfully${NC}"
}

### 运行容器测试
run_tests() {
    local image_name=$1
    echo -e "${YELLOW}Running tests for ${image_name}...${NC}"

    # 测试 GCC 版本
    docker run --rm "$image_name" arm-none-eabi-gcc --version

    # 测试 GDB
    docker run --rm "$image_name" arm-none-eabi-gdb --version

    echo -e "${GREEN}Tests passed${NC}"
}

### 主函数
main() {
    echo "=== Embedded Container Builder ==="

    check_docker
    setup_buildx

    case "$TARGET" in
        arm-gcc)
            build_arm_gcc
            run_tests $(build_image_name "arm-gcc")
            ;;
        multi-arch)
            build_multi_arch
            run_tests $(build_image_name "multi-arch")
            ;;
        all)
            build_arm_gcc
            build_multi_arch
            run_tests $(build_image_name "arm-gcc")
            run_tests $(build_image_name "multi-arch")
            ;;
        *)
            echo -e "${RED}Please specify a target to build${NC}"
            show_help
            exit 1
            ;;
    esac

    echo -e "${GREEN}=== Build Complete ===${NC}"
}

main
```

---

## 4. 嵌入式Linux构建容器

### 4.1 Buildroot 构建环境

```dockerfile
### ============================================================================
### Dockerfile.buildroot
### Buildroot 编译环境容器
### 用于构建完整的嵌入式 Linux 系统
### ============================================================================

FROM ubuntu:22.04 AS buildroot-base

LABEL maintainer="embedded-linux@company.com"
LABEL description="Buildroot Compilation Environment"

### 构建参数
ARG BUILDROOT_VERSION=2023.11.1
ARG USERNAME=builder
ARG USER_UID=1000
ARG USER_GID=1000

### 环境变量
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Asia/Shanghai \
    LC_ALL=C \
    BUILDROOT_VERSION=${BUILDROOT_VERSION}

### -----------------------------------------------------------------------------
### 安装 Buildroot 依赖
### Buildroot 需要大量的构建工具和库
### -----------------------------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
    # 基础构建工具
    build-essential \
    gcc \
    g++ \
    make \
    cmake \
    ninja-build \
    bc \
    bzip2 \
    cpio \
    gawk \
    git \
    # 压缩工具
    unzip \
    zip \
    gzip \
    xz-utils \
    lzma \
    # 文件系统工具
    dosfstools \
    e2fsprogs \
    genext2fs \
    genisoimage \
    libconfuse-dev \
    mtools \
    parted \
    rsync \
    squashfs-tools \
    # 文档工具
    asciidoc \
    dblatex \
    docbook-utils \
    w3m \
    # 网络工具
    wget \
    curl \
    ca-certificates \
    # Python 环境
    python3 \
    python3-pip \
    python3-venv \
    python3-setuptools \
    python3-dev \
    # Perl 模块
    libstring-shellquote-perl \
    libswitch-perl \
    # 库文件
    libncurses5-dev \
    libncursesw5-dev \
    libssl-dev \
    libffi-dev \
    # 其他工具
    file \
    libelf-dev \
    u-boot-tools \
    device-tree-compiler \
    fakeroot \
    fakeroot-sysv \
    bison \
    flex \
    texinfo \
    help2man \
    gperf \
    patch \
    # 清理
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

### 创建构建用户
RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME && \
    usermod -aG sudo $USERNAME && \
    echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME

### 切换到构建用户
USER $USERNAME
WORKDIR /home/$USERNAME

### -----------------------------------------------------------------------------
### 下载 Buildroot
### -----------------------------------------------------------------------------
RUN echo "=== Downloading Buildroot ${BUILDROOT_VERSION} ===" && \
    wget -q "https://buildroot.org/downloads/buildroot-${BUILDROOT_VERSION}.tar.gz" && \
    tar -xzf "buildroot-${BUILDROOT_VERSION}.tar.gz" && \
    rm "buildroot-${BUILDROOT_VERSION}.tar.gz" && \
    mv "buildroot-${BUILDROOT_VERSION}" buildroot && \
    echo "Buildroot downloaded successfully"

### 设置工作目录
WORKDIR /home/$USERNAME/buildroot

### 预下载常用包（加速后续构建）
RUN make source 2>/dev/null || true

### 环境设置脚本
COPY --chown=$USERNAME:$USERNAME <<'EOF' /home/$USERNAME/.buildroot-env
#!/bin/bash
### Buildroot 环境设置

export BUILDROOT_DIR=/home/builder/buildroot
export PATH="$BUILDROOT_DIR/output/host/usr/bin:$PATH"

### 快速构建别名
alias br-make='make -C $BUILDROOT_DIR'
alias br-menuconfig='make -C $BUILDROOT_DIR menuconfig'
alias br-linux-menuconfig='make -C $BUILDROOT_DIR linux-menuconfig'
alias br-busybox-menuconfig='make -C $BUILDROOT_DIR busybox-menuconfig'
alias br-clean='make -C $BUILDROOT_DIR clean'
alias br-distclean='make -C $BUILDROOT_DIR distclean'

echo "Buildroot environment loaded"
echo "Buildroot directory: $BUILDROOT_DIR"
EOF

RUN chmod +x /home/$USERNAME/.buildroot-env && \
    echo "source /home/$USERNAME/.buildroot-env" >> /home/$USERNAME/.bashrc

### 默认命令
CMD ["/bin/bash"]
```

#### Buildroot 使用示例

```bash
#!/bin/bash
### buildroot_workflow.sh
### Buildroot 容器化工作流示例

### 1. 启动 Buildroot 容器
start_buildroot_container() {
    docker run -it --rm \
        --name buildroot-builder \
        -v "$(pwd)/configs":/home/builder/configs \
        -v "$(pwd)/output":/home/builder/output \
        -v "buildroot-ccache":/home/builder/.buildroot-ccache \
        embedded/buildroot:latest
}

### 2. 使用特定板级配置
build_for_board() {
    local board=$1  # 例如: raspberrypi4, stm32mp157a-dk1

    docker run --rm \
        -v "$(pwd)/output":/home/builder/output \
        -v "buildroot-ccache":/home/builder/.buildroot-ccache \
        embedded/buildroot:latest \
        bash -c "
            cd /home/builder/buildroot
            make ${board}_defconfig
            make -j\$(nproc)
            cp -r output/images /home/builder/output/
        "
}

### 3. 自定义配置构建
build_custom() {
    local config_file=$1

    docker run -it --rm \
        -v "$(pwd)/configs":/home/builder/configs \
        -v "$(pwd)/output":/home/builder/output \
        -v "buildroot-ccache":/home/builder/.buildroot-ccache \
        embedded/buildroot:latest \
        bash -c "
            cd /home/builder/buildroot
            cp /home/builder/configs/$config_file .config
            make oldconfig
            make -j\$(nproc)
        "
}

### 4. 增量构建（保留之前的构建结果）
incremental_build() {
    docker run --rm \
        -v "buildroot-output":/home/builder/buildroot/output \
        -v "buildroot-ccache":/home/builder/.ccache \
        embedded/buildroot:latest \
        bash -c "cd /home/builder/buildroot && make -j\$(nproc)"
}
```

### 4.2 Yocto Project 构建环境

```dockerfile
### ============================================================================
### Dockerfile.yocto
### Yocto Project 编译环境容器
### 支持 Poky、OpenEmbedded、meta-layers
### ============================================================================

FROM ubuntu:22.04 AS yocto-builder

LABEL maintainer="yocto-dev@company.com"
LABEL description="Yocto Project Build Environment"

### 构建参数
ARG YOCTO_RELEASE=mickledore
ARG USERNAME=yocto
ARG USER_UID=1000
ARG USER_GID=1000

### 环境变量
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Asia/Shanghai \
    LC_ALL=en_US.UTF-8 \
    LANG=en_US.UTF-8 \
    LANGUAGE=en_US:en \
    YOCTO_RELEASE=${YOCTO_RELEASE}

### -----------------------------------------------------------------------------
### 安装 Yocto 依赖
### Yocto 有严格的依赖要求
### -----------------------------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
    # 基础构建工具
    build-essential \
    chrpath \
    cpio \
    debianutils \
    diffstat \
    file \
    gawk \
    gcc \
    g++ \
    git \
    git-core \
    # 图形界面依赖（用于 bitbake -g）
    graphviz \
    python3-git \
    python3-jinja2 \
    python3-pexpect \
    python3-pip \
    # 压缩工具
    lz4 \
    zstd \
    # 库文件
    libegl1-mesa \
    libsdl1.2-dev \
    libssl-dev \
    liblz4-tool \
    # 其他工具
    pylint \
    python3-subunit \
    socat \
    texinfo \
    unzip \
    wget \
    xterm \
    xz-utils \
    # 清理
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

### 设置 locale
RUN apt-get update && apt-get install -y locales && \
    locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    rm -rf /var/lib/apt/lists/*

### 创建 Yocto 用户
RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME && \
    usermod -aG sudo $USERNAME && \
    echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME

### 切换到 Yocto 用户
USER $USERNAME
WORKDIR /home/$USERNAME

### -----------------------------------------------------------------------------
### 下载 Poky
### -----------------------------------------------------------------------------
RUN echo "=== Cloning Poky (${YOCTO_RELEASE}) ===" && \
    git clone -b ${YOCTO_RELEASE} \
        --single-branch --depth 1 \
        https://git.yoctoproject.org/git/poky && \
    echo "Poky cloned successfully"

### 工作目录
WORKDIR /home/$USERNAME/poky

### 初始化脚本
COPY --chown=$USERNAME:$USERNAME <<'EOF' /home/$USERNAME/init-yocto.sh
#!/bin/bash
### Yocto 初始化脚本

YOCTO_DIR=/home/yocto/poky

cd $YOCTO_DIR

### 初始化构建环境
source oe-init-build-env

### 设置并行编译选项
cat >> conf/local.conf << 'LOCALCONF'

### 并行编译设置
BB_NUMBER_THREADS = "${@oe.utils.cpu_count()}"
PARALLEL_MAKE = "-j${@oe.utils.cpu_count()}"

### 使用本地镜像加速
### SOURCE_MIRROR_URL = "file:///home/yocto/downloads"
### INHERIT += "own-mirrors"

### 启用 ccache
INHERIT += "ccache"
CCACHE_TOP_DIR = "/home/yocto/.ccache"

### 共享状态缓存
SSTATE_DIR = "/home/yocto/sstate-cache"

### 下载目录
DL_DIR = "/home/yocto/downloads"
LOCALCONF

echo "Yocto build environment initialized"
echo "Build directory: $(pwd)/build"
EOF

RUN chmod +x /home/$USERNAME/init-yocto.sh

### 默认命令
CMD ["/bin/bash"]
```

---

## 5. VS Code Dev Container配置

### 5.1 基础 Dev Container 配置

```json
// .devcontainer/devcontainer.json
// VS Code Dev Container 配置文件
// 提供完整的嵌入式开发环境

{
    // 容器名称
    "name": "Embedded Development Environment",

    // 构建配置
    "build": {
        // Dockerfile 路径
        "dockerfile": "Dockerfile",
        // 构建参数
        "args": {
            "ARM_TOOLCHAIN_VERSION": "12.2.rel1",
            "USERNAME": "vscode",
            "USER_UID": "1000",
            "USER_GID": "1000"
        }
    },

    // 容器运行配置
    "runArgs": [
        // 特权模式（用于访问 USB 设备）
        "--privileged",
        // 添加设备访问权限
        "--device=/dev/bus/usb:/dev/bus/usb",
        // 主机网络（用于调试服务器）
        "--network=host",
        // 共享 IPC 命名空间
        "--ipc=host"
    ],

    // 挂载卷
    "mounts": [
        // 持久化 ccache
        "source=ccache,target=/home/vscode/.ccache,type=volume",
        // 挂载 SSH 密钥
        "source=${localEnv:HOME}/.ssh,target=/home/vscode/.ssh,type=bind,consistency=cached",
        // 挂载 Git 配置
        "source=${localEnv:HOME}/.gitconfig,target=/home/vscode/.gitconfig,type=bind,consistency=cached"
    ],

    // VS Code 扩展
    "extensions": [
        // C/C++ 开发
        "ms-vscode.cpptools",
        "ms-vscode.cpptools-extension-pack",
        "jeff-hykin.better-cpp-syntax",

        // CMake 支持
        "twxs.cmake",
        "ms-vscode.cmake-tools",

        // 嵌入式专用
        "marus25.cortex-debug",           // ARM Cortex-M 调试
        "dan-c-underwood.arm",             // ARM 汇编支持
        "keroc.hex-fmt",                   // HEX 文件查看
        "bierner.markdown-preview-github-styles",

        // Python 支持（用于脚本）
        "ms-python.python",
        "ms-python.vscode-pylance",

        // 代码质量
        "ms-vscode.vscode-json",
        "redhat.vscode-yaml",
        "cschlosser.doxdocgen",

        // 主题和图标
        "pkief.material-icon-theme",
        "zhuangtongfa.material-theme",

        // 实用工具
        "eamodio.gitlens",
        "gruntfuggly.todo-tree",
        "usernamehw.errorlens"
    ],

    // VS Code 设置
    "settings": {
        // 终端配置
        "terminal.integrated.defaultProfile.linux": "bash",
        "terminal.integrated.profiles.linux": {
            "bash": {
                "path": "/bin/bash",
                "icon": "terminal-bash"
            }
        },

        // C/C++ 配置
        "C_Cpp.default.intelliSenseMode": "linux-gcc-arm",
        "C_Cpp.default.compilerPath": "/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc",
        "C_Cpp.default.cStandard": "c11",
        "C_Cpp.default.cppStandard": "c++17",
        "C_Cpp.default.defines": [
            "USE_HAL_DRIVER",
            "STM32F407xx"
        ],
        "C_Cpp.default.includePath": [
            "${workspaceFolder}/**",
            "/opt/gcc-arm-none-eabi/arm-none-eabi/include/**"
        ],
        "C_Cpp.formatting": "clangFormat",
        "C_Cpp.clang_format_path": "/usr/bin/clang-format",

        // CMake 配置
        "cmake.cmakePath": "/usr/bin/cmake",
        "cmake.configureOnOpen": true,
        "cmake.buildDirectory": "${workspaceFolder}/build",
        "cmake.generator": "Ninja",
        "cmake.preferredGenerators": ["Ninja", "Unix Makefiles"],

        // 文件关联
        "files.associations": {
            "*.s": "arm",
            "*.S": "arm",
            "*.ld": "linkerscript",
            "*.ioc": "json"
        },

        // 编辑器配置
        "editor.formatOnSave": true,
        "editor.formatOnType": true,
        "editor.rulers": [80, 120],
        "editor.renderWhitespace": "boundary",
        "editor.insertSpaces": true,
        "editor.tabSize": 4,

        // Git 配置
        "git.enableSmartCommit": true,
        "git.confirmSync": false,

        // 文件排除
        "files.exclude": {
            "**/.git": true,
            "**/.DS_Store": true,
            "**/build": true
        }
    },

    // 创建后执行的命令
    "postCreateCommand": "bash .devcontainer/post-create.sh",

    // 用户配置
    "remoteUser": "vscode",

    // 功能特性
    "features": {
        "ghcr.io/devcontainers/features/git:1": {},
        "ghcr.io/devcontainers/features/github-cli:1": {}
    },

    // 转发端口
    "forwardPorts": [
        3333,  // GDB 服务器
        4444,  // OpenOCD Telnet
        8080   // Web 服务器
    ],
    "portsAttributes": {
        "3333": {
            "label": "GDB Server",
            "onAutoForward": "notify"
        },
        "4444": {
            "label": "OpenOCD Telnet",
            "onAutoForward": "silent"
        }
    }
}
```

### 5.2 Dev Container Dockerfile

```dockerfile
### ============================================================================
### .devcontainer/Dockerfile
### VS Code Dev Container 专用 Dockerfile
### ============================================================================

FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

### 构建参数
ARG ARM_TOOLCHAIN_VERSION=12.2.rel1
ARG STM32CUBEMX_VERSION=6.9.0
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=1000

### 环境变量
ENV DEBIAN_FRONTEND=noninteractive \
    ARM_TOOLCHAIN_VERSION=${ARM_TOOLCHAIN_VERSION} \
    PATH="/opt/gcc-arm-none-eabi/bin:/opt/st/stm32cubemx:${PATH}"

### -----------------------------------------------------------------------------
## 安装基础工具
### -----------------------------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
    # 构建工具
    build-essential \
    cmake \
    ninja-build \
    make \
    gdb-multiarch \
    # 版本控制
    git \
    git-lfs \
    # 网络工具
    curl \
    wget \
    ca-certificates \
    # Python
    python3 \
    python3-pip \
    python3-venv \
    # USB 支持
    libusb-1.0-0-dev \
    libftdi1-dev \
    udev \
    # 文档工具
    doxygen \
    graphviz \
    # 代码格式化
    clang-format \
    clang-tidy \
    # 实用工具
    vim \
    nano \
    htop \
    tree \
    jq \
    # 串口工具
    minicom \
    picocom \
    socat \
    # 清理
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

### -----------------------------------------------------------------------------
### 安装 ARM GCC 工具链
### -----------------------------------------------------------------------------
RUN echo "Installing ARM GCC ${ARM_TOOLCHAIN_VERSION}..." && \
    mkdir -p /opt/gcc-arm-none-eabi && \
    curl -fsSL \
        "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_TOOLCHAIN_VERSION}/binrel/arm-gnu-toolchain-${ARM_TOOLCHAIN_VERSION}-x86_64-arm-none-eabi.tar.xz" | \
    tar -xJC /opt/gcc-arm-none-eabi --strip-components=1 && \
    arm-none-eabi-gcc --version

### -----------------------------------------------------------------------------
### 安装 OpenOCD
### -----------------------------------------------------------------------------
RUN echo "Installing OpenOCD..." && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        openocd \
        libhidapi-hidraw0 \
        && rm -rf /var/lib/apt/lists/*

### -----------------------------------------------------------------------------
### 安装调试工具
### -----------------------------------------------------------------------------
RUN pip3 install --no-cache-dir --upgrade pip && \
    pip3 install --no-cache-dir \
    pyocd \
    pyserial \
    mbed-cli \
    imgtool

### -----------------------------------------------------------------------------
### 安装 STM32CubeMX（可选）
### -----------------------------------------------------------------------------
### 注意：STM32CubeMX 需要图形界面，在 Dev Container 中可能需要额外配置
### RUN mkdir -p /opt/st/stm32cubemx && \
###     curl -fsSL -o /tmp/cubemx.zip \
###         "https://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-configurators-and-code-generators/stm32cubemx.html" && \
###     unzip -q /tmp/cubemx.zip -d /opt/st/stm32cubemx && \
###     rm /tmp/cubemx.zip

### -----------------------------------------------------------------------------
### 配置用户
### -----------------------------------------------------------------------------
## 用户已在基础镜像中创建，这里只需添加权限
RUN usermod -aG dialout,plugdev ${USERNAME}

### 创建常用目录
RUN mkdir -p /home/${USERNAME}/.ccache && \
    chown -R ${USERNAME}:${USERNAME} /home/${USERNAME}/.ccache

### -----------------------------------------------------------------------------
### 安装附加工具脚本
### -----------------------------------------------------------------------------
COPY --chown=${USERNAME}:${USERNAME} scripts/* /usr/local/bin/
RUN chmod +x /usr/local/bin/*

### -----------------------------------------------------------------------------
### 设置工作目录
### -----------------------------------------------------------------------------
WORKDIR /workspace
USER ${USERNAME}

### 默认命令
CMD ["/bin/bash"]
```

### 5.3 容器创建后脚本

```bash
#!/bin/bash
### .devcontainer/post-create.sh
### Dev Container 创建后执行脚本

set -e

echo "=== Post-Create Setup ==="

### 1. 配置 Git（如果尚未配置）
if ! git config --global user.name &> /dev/null; then
    echo "提示: 请配置 Git 用户名: git config --global user.name 'Your Name'"
fi

if ! git config --global user.email &> /dev/null; then
    echo "提示: 请配置 Git 邮箱: git config --global user.email 'your@email.com'"
fi

### 2. 设置 ccache
export CCACHE_DIR=/home/vscode/.ccache
ccache --max-size=5G
echo "ccache configured with max size: 5GB"

### 3. 验证工具链
echo "Verifying toolchains..."
echo "ARM GCC:"
arm-none-eabi-gcc --version | head -n 1

echo "CMake:"
cmake --version | head -n 1

echo "OpenOCD:"
openocd --version | head -n 1

echo "Python3:"
python3 --version

### 4. 安装 Python 依赖（如果存在 requirements.txt）
if [ -f "requirements.txt" ]; then
    echo "Installing Python dependencies..."
    pip3 install --user -r requirements.txt
fi

### 5. 初始化 Git 子模块
if [ -f ".gitmodules" ]; then
    echo "Initializing git submodules..."
    git submodule update --init --recursive
fi

### 6. 设置 udev 规则（需要特权模式）
if [ -d ".devcontainer/udev" ]; then
    echo "Setting up udev rules..."
    sudo cp .devcontainer/udev/*.rules /etc/udev/rules.d/ 2>/dev/null || true
    sudo udevadm control --reload-rules 2>/dev/null || true
fi

### 7. 预下载常用文件
if [ -f ".devcontainer/setup-scripts/download-tools.sh" ]; then
    bash .devcontainer/setup-scripts/download-tools.sh
fi

echo "=== Setup Complete ==="
echo ""
echo "可用命令:"
echo "  build-project    - 构建项目"
echo "  flash-target     - 烧录目标设备"
echo "  debug-target     - 启动调试会话"
echo "  run-tests        - 运行测试"
```

---

## 6. GitHub Codespaces用于嵌入式

### 6.1 Codespaces 配置

```json
// .devcontainer/codespaces.json
// GitHub Codespaces 专用配置

{
    "name": "Embedded Dev - Codespaces",
    "image": "mcr.microsoft.com/devcontainers/universal:2",

    "features": {
        // 安装额外的工具
        "ghcr.io/devcontainers/features/github-cli:1": {},
        "ghcr.io/devcontainers/features/docker-in-docker:2": {},

        // 安装 ARM 工具链
        "ghcr.io/devcontainers-contrib/features/apt-get-packages:1": {
            "packages": "build-essential,cmake,ninja-build,libusb-1.0-0-dev,gdb-multiarch,openocd"
        }
    },

    "postCreateCommand": ".devcontainer/codespaces-setup.sh",

    "customizations": {
        "vscode": {
            "extensions": [
                "ms-vscode.cpptools",
                "ms-vscode.cmake-tools",
                "marus25.cortex-debug",
                "ms-vscode.vscode-serial-monitor",
                "github.vscode-github-actions"
            ],
            "settings": {
                "terminal.integrated.defaultProfile.linux": "bash"
            }
        }
    },

    // Codespaces 特定配置
    "hostRequirements": {
        "cpus": 4,
        "memory": "8gb",
        "storage": "32gb"
    }
}
```

```bash
#!/bin/bash
### .devcontainer/codespaces-setup.sh
### GitHub Codespaces 设置脚本

echo "Setting up GitHub Codespaces for embedded development..."

### 安装 ARM GCC
ARM_VERSION="12.2.rel1"
INSTALL_DIR="$HOME/.local/opt/gcc-arm-none-eabi"

mkdir -p "$INSTALL_DIR"
curl -fsSL "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_VERSION}/binrel/arm-gnu-toolchain-${VERSION}-x86_64-arm-none-eabi.tar.xz" | \
    tar -xJC "$INSTALL_DIR" --strip-components=1

### 添加到 PATH
echo "export PATH=\"$INSTALL_DIR/bin:\$PATH\"" >> ~/.bashrc
export PATH="$INSTALL_DIR/bin:$PATH"

### 验证安装
arm-none-eabi-gcc --version

echo "Setup complete!"
```

### 6.2 Codespaces 到本地设备调试

```yaml
### .github/codespaces/tunnel.yml
### 使用 GitHub CLI 创建本地设备调试隧道

name: Device Debug Tunnel

on:
  workflow_dispatch:

jobs:
  tunnel:
    runs-on: ubuntu-latest
    steps:
      - name: Setup tunnel
        run: |
          # 安装 ngrok 或其他隧道工具
          # 创建到本地设备的反向隧道
          echo "Use 'gh codespace ssh' to connect"
```

---

## 7. Docker Compose多服务开发

### 7.1 完整开发环境配置

```yaml
### docker-compose.yml
### 完整嵌入式开发环境

version: '3.8'

services:
  # ============================================================================
  # 主开发容器
  # ============================================================================
  dev:
    build:
      context: .
      dockerfile: Dockerfile.dev
      args:
        ARM_TOOLCHAIN_VERSION: "12.2.rel1"
        USER_UID: ${USER_UID:-1000}
        USER_GID: ${USER_GID:-1000}
    image: embedded-dev:latest
    container_name: embedded-dev

    # 保持容器运行
    stdin_open: true
    tty: true

    # 工作目录
    working_dir: /workspace

    # 卷挂载
    volumes:
      # 项目代码
      - .:/workspace:cached
      # ccache 缓存
      - ccache:/home/dev/.ccache
      # 下载缓存
      - downloads:/home/dev/downloads
      # SSH 密钥
      - ~/.ssh:/home/dev/.ssh:ro
      # Git 配置
      - ~/.gitconfig:/home/dev/.gitconfig:ro
      # USB 设备
      - /dev/bus/usb:/dev/bus/usb

    # 设备访问
    devices:
      - /dev/bus/usb:/dev/bus/usb

    # 特权模式（用于调试器访问）
    privileged: true

    # 环境变量
    environment:
      - CCACHE_DIR=/home/dev/.ccache
      - TERM=xterm-256color
      - LC_ALL=C.UTF-8

    # 端口映射
    ports:
      - "3333:3333"      # GDB 服务器
      - "4444:4444"      # OpenOCD Telnet
      - "6666:6666"      # OpenOCD TCL
      - "8080:8080"      # Web 服务器

    # 网络
    networks:
      - embedded-net

    # 命令
    command: /bin/bash

    # 健康检查
    healthcheck:
      test: ["CMD", "arm-none-eabi-gcc", "--version"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 40s

  # ============================================================================
  # GDB 服务器服务
  # ============================================================================
  gdb-server:
    build:
      context: .
      dockerfile: Dockerfile.openocd
    image: embedded-gdb-server:latest
    container_name: embedded-gdb-server

    privileged: true

    devices:
      - /dev/bus/usb:/dev/bus/usb

    ports:
      - "3333:3333"
      - "4444:4444"

    # 启动 OpenOCD
    command: >
      openocd
      -f interface/stlink.cfg
      -f target/stm32f4x.cfg
      -c "gdb_port 3333"
      -c "telnet_port 4444"

    networks:
      - embedded-net

    restart: unless-stopped

    depends_on:
      - dev

  # ============================================================================
  # 文档服务器
  # ============================================================================
  docs:
    image: nginx:alpine
    container_name: embedded-docs

    volumes:
      - ./docs:/usr/share/nginx/html:ro
      - ./nginx.conf:/etc/nginx/conf.d/default.conf:ro

    ports:
      - "8080:80"

    networks:
      - embedded-net

    restart: unless-stopped

  # ============================================================================
  # 构建缓存服务
  # ============================================================================
  cache:
    image: alpine:latest
    container_name: embedded-cache

    volumes:
      - ccache:/data/ccache
      - downloads:/data/downloads

    command: >
      sh -c "
        echo 'Cache volumes initialized';
        while true; do
          sleep 3600;
        done
      "

    networks:
      - embedded-net

  # ============================================================================
  # CI/CD 代理
  # ============================================================================
  ci-agent:
    build:
      context: .
      dockerfile: Dockerfile.ci
    image: embedded-ci:latest
    container_name: embedded-ci

    volumes:
      - .:/workspace:ro
      - ci-artifacts:/workspace/build

    environment:
      - CI=true
      - BUILD_TYPE=Release

    command: >
      sh -c "
        cd /workspace &&
        cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release &&
        cmake --build build &&
        ctest --test-dir build --output-on-failure
      "

    networks:
      - embedded-net

    profiles:
      - ci

### ============================================================================
### 网络配置
### ============================================================================
networks:
  embedded-net:
    driver: bridge
    ipam:
      config:
        - subnet: 172.28.0.0/16

### ============================================================================
### 数据卷
### ============================================================================
volumes:
  ccache:
    driver: local
  downloads:
    driver: local
  ci-artifacts:
    driver: local
```

### 7.2 使用脚本

```bash
#!/bin/bash
### dev.sh
### Docker Compose 开发环境管理脚本

set -e

### 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

### 获取当前用户 ID
export USER_UID=$(id -u)
export USER_GID=$(id -g)

### 显示帮助
show_help() {
    cat << EOF
嵌入式开发环境管理脚本

Usage: $0 <command> [options]

Commands:
    start       启动开发环境
    stop        停止开发环境
    restart     重启开发环境
    shell       进入开发容器 shell
    build       构建项目
    flash       烧录固件
    debug       启动调试会话
    test        运行测试
    ci          运行 CI 构建
    clean       清理构建产物
    logs        查看日志
    status      查看环境状态

Options:
    -h, --help  显示此帮助信息

Examples:
    $0 start                    # 启动环境
    $0 shell                    # 进入容器
    $0 build --target firmware  # 构建特定目标
EOF
}

### 启动环境
cmd_start() {
    echo -e "${YELLOW}Starting embedded development environment...${NC}"
    docker-compose up -d dev
    echo -e "${GREEN}Environment started!${NC}"
    echo "使用 '$0 shell' 进入开发容器"
}

### 停止环境
cmd_stop() {
    echo -e "${YELLOW}Stopping environment...${NC}"
    docker-compose down
    echo -e "${GREEN}Environment stopped.${NC}"
}

### 重启环境
cmd_restart() {
    cmd_stop
    cmd_start
}

### 进入 shell
cmd_shell() {
    docker-compose exec dev /bin/bash
}

### 构建项目
cmd_build() {
    echo -e "${YELLOW}Building project...${NC}"
    docker-compose exec dev bash -c "
        mkdir -p build &&
        cd build &&
        cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug &&
        ninja
    "
    echo -e "${GREEN}Build complete!${NC}"
}

### 烧录固件
cmd_flash() {
    local firmware=${1:-build/firmware.bin}
    echo -e "${YELLOW}Flashing firmware: $firmware${NC}"

    # 启动 GDB 服务器
    docker-compose up -d gdb-server
    sleep 2

    # 烧录
    docker-compose exec dev bash -c "
        arm-none-eabi-gdb -ex 'target remote gdb-server:3333' \
                         -ex 'monitor reset halt' \
                         -ex 'flash write_image erase $firmware 0x08000000' \
                         -ex 'monitor reset run' \
                         -ex 'disconnect' \
                         -ex 'quit'
    "

    echo -e "${GREEN}Flash complete!${NC}"
}

### 启动调试
cmd_debug() {
    echo -e "${YELLOW}Starting debug session...${NC}"

    docker-compose up -d gdb-server

    docker-compose exec dev bash -c "
        arm-none-eabi-gdb build/firmware.elf \
            -ex 'target remote gdb-server:3333' \
            -ex 'monitor reset halt' \
            -ex 'load' \
            -ex 'break main' \
            -ex 'continue'
    "
}

### 运行测试
cmd_test() {
    echo -e "${YELLOW}Running tests...${NC}"
    docker-compose exec dev bash -c "
        cd build &&
        ctest --output-on-failure
    "
}

### CI 构建
cmd_ci() {
    echo -e "${YELLOW}Running CI build...${NC}"
    docker-compose --profile ci run --rm ci-agent
    echo -e "${GREEN}CI build complete!${NC}"
}

### 清理
cmd_clean() {
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"
    docker-compose exec dev bash -c "rm -rf build/*"
    echo -e "${GREEN}Clean complete!${NC}"
}

### 查看日志
cmd_logs() {
    docker-compose logs -f "$@"
}

### 查看状态
cmd_status() {
    echo -e "${BLUE}Container Status:${NC}"
    docker-compose ps

    echo ""
    echo -e "${BLUE}Resource Usage:${NC}"
    docker stats --no-stream --format "table {{.Container}}\t{{.CPUPerc}}\t{{.MemUsage}}" \
        $(docker-compose ps -q) 2>/dev/null || echo "No containers running"
}

### 主函数
main() {
    case "${1:-}" in
        start)
            cmd_start
            ;;
        stop)
            cmd_stop
            ;;
        restart)
            cmd_restart
            ;;
        shell)
            cmd_shell
            ;;
        build)
            shift
            cmd_build "$@"
            ;;
        flash)
            shift
            cmd_flash "$@"
            ;;
        debug)
            cmd_debug
            ;;
        test)
            cmd_test
            ;;
        ci)
            cmd_ci
            ;;
        clean)
            cmd_clean
            ;;
        logs)
            shift
            cmd_logs "$@"
            ;;
        status)
            cmd_status
            ;;
        -h|--help|help)
            show_help
            ;;
        *)
            echo -e "${RED}Unknown command: ${1:-}${NC}"
            show_help
            exit 1
            ;;
    esac
}

main "$@"
```

---

## 8. 容器化测试

### 8.1 QEMU 测试容器

```dockerfile
### ============================================================================
### Dockerfile.qemu
### QEMU 测试环境容器
### 用于在容器中运行嵌入式软件测试
### ============================================================================

FROM ubuntu:22.04 AS qemu-base

LABEL maintainer="test-team@company.com"
LABEL description="QEMU Testing Environment for Embedded Software"

ENV DEBIAN_FRONTEND=noninteractive \
    QEMU_VERSION=8.1.0

### 安装 QEMU 和测试工具
RUN apt-get update && apt-get install -y --no-install-recommends \
    # QEMU 模拟器
    qemu-system-arm \
    qemu-system-misc \
    qemu-user-static \
    # 调试工具
    gdb-multiarch \
    # 测试框架
    python3 \
    python3-pip \
    python3-venv \
    # 覆盖率工具
    lcov \
    gcovr \
    # 其他工具
    socat \
    expect \
    && rm -rf /var/lib/apt/lists/*

### 安装 Python 测试库
RUN pip3 install --no-cache-dir \
    pytest \
    pytest-html \
    pytest-cov \
    pyserial \
    pexpect

### 创建工作目录
WORKDIR /test

### 测试入口脚本
COPY <<'EOF' /usr/local/bin/run-tests.sh
#!/bin/bash
### 容器内测试运行脚本

set -e

TEST_DIR=${TEST_DIR:-/test}
RESULTS_DIR=${RESULTS_DIR:-/test/results}
COVERAGE_DIR=${COVERAGE_DIR:-/test/coverage}

mkdir -p "$RESULTS_DIR" "$COVERAGE_DIR"

echo "=== Running Embedded Tests ==="
echo "Test directory: $TEST_DIR"
echo "Results directory: $RESULTS_DIR"

### 运行单元测试
echo "Running unit tests..."
cd "$TEST_DIR"
pytest tests/ \
    --verbose \
    --html="$RESULTS_DIR/report.html" \
    --self-contained-html \
    --cov=src \
    --cov-report=xml:"$COVERAGE_DIR/coverage.xml" \
    --cov-report=html:"$COVERAGE_DIR/html" \
    --junitxml="$RESULTS_DIR/junit.xml"

### 运行 QEMU 集成测试
echo "Running QEMU integration tests..."
bash tests/qemu/run_qemu_tests.sh

echo "=== Tests Complete ==="
echo "Results available in: $RESULTS_DIR"
EOF

RUN chmod +x /usr/local/bin/run-tests.sh

CMD ["/usr/local/bin/run-tests.sh"]
```

### 8.2 QEMU 测试配置

```yaml
### docker-compose.test.yml
### 测试环境 Docker Compose 配置

version: '3.8'

services:
  # 单元测试服务
  unit-test:
    build:
      context: .
      dockerfile: Dockerfile.test
    image: embedded-unit-test:latest
    volumes:
      - .:/test:ro
      - test-results:/test/results
    command: >
      pytest tests/unit
        --verbose
        --html=/test/results/unit-report.html
        --cov=src
        --cov-report=xml:/test/results/unit-coverage.xml

  # QEMU 集成测试
  qemu-test:
    build:
      context: .
      dockerfile: Dockerfile.qemu
    image: embedded-qemu-test:latest
    privileged: true  # QEMU 需要特权
    volumes:
      - ./build:/test/build:ro
      - ./tests:/test/tests:ro
      - test-results:/test/results
    environment:
      - QEMU_SYSTEM=qemu-system-arm
      - QEMU_MACHINE=stm32-p103
      - QEMU_CPU=cortex-m3
    command: >
      bash -c "
        qemu-system-arm
          -M $$QEMU_MACHINE
          -cpu $$QEMU_CPU
          -kernel /test/build/firmware.elf
          -serial stdio
          -S -s
          -daemonize
        sleep 2
        pytest tests/integration --qemu-port=1234
      "

  # 硬件在环测试（需要连接实际硬件）
  hitl-test:
    build:
      context: .
      dockerfile: Dockerfile.test
    image: embedded-hitl-test:latest
    privileged: true
    devices:
      - /dev/ttyUSB0:/dev/ttyUSB0
      - /dev/bus/usb:/dev/bus/usb
    volumes:
      - .:/test:ro
      - test-results:/test/results
    environment:
      - TEST_TARGET=/dev/ttyUSB0
      - TEST_BAUDRATE=115200
    command: >
      pytest tests/hitl
        --target=$$TEST_TARGET
        --baudrate=$$TEST_BAUDRATE
        --html=/test/results/hitl-report.html

volumes:
  test-results:
    driver: local
```

### 8.3 自动化测试脚本

```bash
#!/bin/bash
### run_tests.sh
### 自动化测试脚本

set -e

### 配置
TEST_TYPES=${TEST_TYPES:-"unit integration"}
RESULTS_DIR=${RESULTS_DIR:-"./test-results"}
COVERAGE_THRESHOLD=${COVERAGE_THRESHOLD:-80}

### 颜色
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

mkdir -p "$RESULTS_DIR"

echo "=========================================="
echo "Running Embedded Tests"
echo "Test types: $TEST_TYPES"
echo "=========================================="

### 构建测试容器
echo -e "${YELLOW}Building test containers...${NC}"
docker-compose -f docker-compose.test.yml build

### 运行单元测试
run_unit_tests() {
    echo -e "${YELLOW}Running unit tests...${NC}"

    docker-compose -f docker-compose.test.yml run --rm unit-test

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Unit tests passed!${NC}"
    else
        echo -e "${RED}Unit tests failed!${NC}"
        exit 1
    fi
}

### 运行 QEMU 测试
run_qemu_tests() {
    echo -e "${YELLOW}Running QEMU integration tests...${NC}"

    # 构建固件
    docker-compose exec -T dev cmake --build build

    # 运行 QEMU 测试
    docker-compose -f docker-compose.test.yml run --rm qemu-test

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}QEMU tests passed!${NC}"
    else
        echo -e "${RED}QEMU tests failed!${NC}"
        exit 1
    fi
}

### 运行 HIL 测试（如果有硬件）
run_hitl_tests() {
    echo -e "${YELLOW}Running Hardware-in-the-Loop tests...${NC}"

    # 检查硬件连接
    if [ -e "/dev/ttyUSB0" ]; then
        docker-compose -f docker-compose.test.yml run --rm hitl-test
    else
        echo -e "${YELLOW}No hardware detected, skipping HIL tests${NC}"
    fi
}

### 检查覆盖率
check_coverage() {
    echo -e "${YELLOW}Checking code coverage...${NC}"

    COVERAGE_FILE="$RESULTS_DIR/unit-coverage.xml"
    if [ -f "$COVERAGE_FILE" ]; then
        # 解析覆盖率（简化示例）
        COVERAGE=$(grep -oP 'line-rate="\K[0-9.]+' "$COVERAGE_FILE" | head -1)
        COVERAGE_PERCENT=$(echo "$COVERAGE * 100" | bc)

        echo "Code coverage: ${COVERAGE_PERCENT}%"

        if (( $(echo "$COVERAGE_PERCENT < $COVERAGE_THRESHOLD" | bc -l) )); then
            echo -e "${RED}Coverage below threshold: ${COVERAGE_THRESHOLD}%${NC}"
            exit 1
        else
            echo -e "${GREEN}Coverage meets threshold!${NC}"
        fi
    fi
}

### 生成报告
generate_report() {
    echo -e "${YELLOW}Generating test report...${NC}"

    cat > "$RESULTS_DIR/summary.html" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>Test Report Summary</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        h1 { color: #333; }
        .pass { color: green; }
        .fail { color: red; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #4CAF50; color: white; }
    </style>
</head>
<body>
    <h1>Embedded Test Report</h1>
    <p>Generated: $(date)</p>
    <h2>Test Results</h2>
    <ul>
        <li>Unit Tests: <span class="pass">PASSED</span></li>
        <li>Integration Tests: <span class="pass">PASSED</span></li>
    </ul>
</body>
</html>
EOF
}

### 主执行流程
main() {
    for test_type in $TEST_TYPES; do
        case $test_type in
            unit)
                run_unit_tests
                ;;
            integration)
                run_qemu_tests
                ;;
            hitl)
                run_hitl_tests
                ;;
            *)
                echo "Unknown test type: $test_type"
                ;;
        esac
    done

    check_coverage
    generate_report

    echo ""
    echo -e "${GREEN}==========================================${NC}"
    echo -e "${GREEN}All tests completed successfully!${NC}"
    echo -e "${GREEN}Results: $RESULTS_DIR${NC}"
    echo -e "${GREEN}==========================================${NC}"
}

main
```

---

## 9. CI/CD流水线

### 9.1 GitHub Actions 完整配置

```yaml
### .github/workflows/embedded-ci.yml
### 完整的嵌入式 CI/CD 流水线

name: Embedded CI/CD Pipeline

on:
  push:
    branches: [main, develop]
    tags: ['v*']
  pull_request:
    branches: [main, develop]

### 环境变量
env:
  ARM_TOOLCHAIN_VERSION: 12.2.rel1
  BUILD_TYPE: Release
  # 容器镜像
  DEV_IMAGE: ghcr.io/${{ github.repository }}/embedded-dev:latest

jobs:
  # ==========================================================================
  # 代码检查
  # ==========================================================================
  lint:
    name: Code Linting
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4
        with:
          submodules: recursive
          fetch-depth: 0

      - name: Run clang-format check
        uses: jidicula/clang-format-action@v4.11.0
        with:
          clang-format-version: '15'
          check-path: 'src'
          fallback-style: 'LLVM'

      - name: Run cppcheck
        uses: deep5050/cppcheck-action@main
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          std: 'c11'
          platform: 'unix32'

  # ==========================================================================
  # 构建矩阵
  # ==========================================================================
  build:
    name: Build (${{ matrix.config.name }})
    runs-on: ubuntu-latest
    needs: lint

    strategy:
      fail-fast: false
      matrix:
        config:
          - name: "STM32F4-Discovery"
            board: stm32f4_discovery
            target: stm32f407
            cmake_args: "-DBOARD_STM32F4=ON"

          - name: "STM32F7-Nucleo"
            board: stm32f7_nucleo
            target: stm32f767
            cmake_args: "-DBOARD_STM32F7=ON"

          - name: "nRF52-DK"
            board: nrf52_dk
            target: nrf52832
            cmake_args: "-DBOARD_NRF52=ON"

    container:
      image: ghcr.io/${{ github.repository }}/embedded-dev:latest
      credentials:
        username: ${{ github.actor }}
        password: ${{ secrets.GITHUB_TOKEN }}

    steps:
      - name: Checkout code
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Cache ccache
        uses: actions/cache@v3
        with:
          path: /home/dev/.ccache
          key: ccache-${{ matrix.config.target }}-${{ github.ref_name }}-${{ github.sha }}
          restore-keys: |
            ccache-${{ matrix.config.target }}-${{ github.ref_name }}-
            ccache-${{ matrix.config.target }}-

      - name: Configure CMake
        run: |
          cmake -B build \
            -G Ninja \
            -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }} \
            -DCMAKE_C_COMPILER_LAUNCHER=ccache \
            -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
            ${{ matrix.config.cmake_args }}

      - name: Build firmware
        run: |
          cmake --build build --parallel $(nproc)

          # 显示构建产物信息
          echo "=== Build Artifacts ==="
          ls -lh build/*.elf build/*.bin build/*.hex 2>/dev/null || true

          # 显示固件大小
          arm-none-eabi-size build/*.elf

      - name: Upload artifacts
        uses: actions/upload-artifact@v4
        with:
          name: firmware-${{ matrix.config.target }}
          path: |
            build/*.elf
            build/*.bin
            build/*.hex
            build/*.map
          retention-days: 30

  # ==========================================================================
  # 测试
  # ==========================================================================
  test:
    name: Run Tests
    runs-on: ubuntu-latest
    needs: build

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Run unit tests
        uses: docker://ghcr.io/${{ github.repository }}/embedded-test:latest
        with:
          args: pytest tests/unit --cov=src --cov-report=xml

      - name: Upload coverage to Codecov
        uses: codecov/codecov-action@v3
        with:
          files: ./coverage.xml
          fail_ci_if_error: true

      - name: Run QEMU integration tests
        run: |
          docker-compose -f docker-compose.test.yml up --abort-on-container-exit qemu-test

  # ==========================================================================
  # 静态分析
  # ==========================================================================
  analyze:
    name: Static Analysis
    runs-on: ubuntu-latest
    needs: build

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Download build artifacts
        uses: actions/download-artifact@v4
        with:
          name: firmware-stm32f407
          path: build/

      - name: Run SonarCloud scan
        uses: SonarSource/sonarcloud-github-action@master
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
          SONAR_TOKEN: ${{ secrets.SONAR_TOKEN }}

      - name: Run CodeQL Analysis
        uses: github/codeql-action/init@v2
        with:
          languages: cpp

      - name: Autobuild
        uses: github/codeql-action/autobuild@v2

      - name: Perform CodeQL Analysis
        uses: github/codeql-action/analyze@v2

  # ==========================================================================
  # 安全扫描
  # ==========================================================================
  security:
    name: Security Scan
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Run Trivy vulnerability scanner
        uses: aquasecurity/trivy-action@master
        with:
          scan-type: 'fs'
          scan-ref: '.'
          format: 'sarif'
          output: 'trivy-results.sarif'

      - name: Upload Trivy results
        uses: github/codeql-action/upload-sarif@v2
        if: always()
        with:
          sarif_file: 'trivy-results.sarif'

      - name: Run Snyk security scan
        uses: snyk/actions/node@master
        continue-on-error: true
        env:
          SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}
        with:
          args: --severity-threshold=high

  # ==========================================================================
  # 文档构建
  # ==========================================================================
  docs:
    name: Build Documentation
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Generate Doxygen documentation
        uses: mattnotmitt/doxygen-action@v1.9.5
        with:
          working-directory: '.'
          doxyfile-path: './Doxyfile'

      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v3
        if: github.ref == 'refs/heads/main'
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: ./docs/html

  # ==========================================================================
  # 发布
  # ==========================================================================
  release:
    name: Create Release
    runs-on: ubuntu-latest
    needs: [build, test, analyze, security]
    if: startsWith(github.ref, 'refs/tags/v')

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Download all artifacts
        uses: actions/download-artifact@v4
        with:
          path: artifacts

      - name: Create Release
        uses: softprops/action-gh-release@v1
        with:
          files: artifacts/**/*
          generate_release_notes: true
          draft: false
          prerelease: ${{ contains(github.ref, 'rc') || contains(github.ref, 'beta') }}

  # ==========================================================================
  # 容器镜像构建与推送
  # ==========================================================================
  docker:
    name: Build and Push Docker Images
    runs-on: ubuntu-latest
    if: github.event_name != 'pull_request'

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Set up QEMU
        uses: docker/setup-qemu-action@v3

      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v3

      - name: Login to GitHub Container Registry
        uses: docker/login-action@v3
        with:
          registry: ghcr.io
          username: ${{ github.actor }}
          password: ${{ secrets.GITHUB_TOKEN }}

      - name: Extract metadata
        id: meta
        uses: docker/metadata-action@v5
        with:
          images: |
            ghcr.io/${{ github.repository }}/embedded-dev
            ghcr.io/${{ github.repository }}/embedded-test
          tags: |
            type=ref,event=branch
            type=ref,event=pr
            type=semver,pattern={{version}}
            type=semver,pattern={{major}}.{{minor}}

      - name: Build and push development image
        uses: docker/build-push-action@v5
        with:
          context: .
          file: ./Dockerfile.dev
          platforms: linux/amd64,linux/arm64
          push: true
          tags: ghcr.io/${{ github.repository }}/embedded-dev:latest
          cache-from: type=gha
          cache-to: type=gha,mode=max

      - name: Build and push test image
        uses: docker/build-push-action@v5
        with:
          context: .
          file: ./Dockerfile.test
          platforms: linux/amd64
          push: true
          tags: ghcr.io/${{ github.repository }}/embedded-test:latest
          cache-from: type=gha
          cache-to: type=gha,mode=max
```

### 9.2 GitLab CI 配置

```yaml
### .gitlab-ci.yml
### GitLab CI/CD 配置 for 嵌入式开发

### 全局变量
variables:
  ARM_TOOLCHAIN_VERSION: "12.2.rel1"
  DOCKER_DRIVER: overlay2
  DOCKER_TLS_CERTDIR: ""
  GIT_SUBMODULE_STRATEGY: recursive
  GIT_DEPTH: 0

### 使用容器执行
default:
  image: $CI_REGISTRY/embedded/dev:latest
  tags:
    - docker

### 流水线阶段
stages:
  - lint
  - build
  - test
  - analyze
  - deploy

### 缓存配置
.ccache_template: &ccache
  cache:
    key: "ccache-${CI_JOB_NAME}"
    paths:
      - .ccache/

### ============================================================================
### 代码检查阶段
### ============================================================================

clang-format:
  stage: lint
  image: silkeh/clang:15
  script:
    - find src -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror
  allow_failure: true

cppcheck:
  stage: lint
  image: pipelinecomponents/cppcheck:latest
  script:
    - cppcheck --enable=all --error-exitcode=1 --inline-suppr src/
  allow_failure: true

### ============================================================================
### 构建阶段
### ============================================================================

build:stm32f4:
  stage: build
  <<: *ccache
  variables:
    CCACHE_DIR: "$CI_PROJECT_DIR/.ccache"
    CMAKE_ARGS: "-DBOARD_STM32F4=ON"
  script:
    - cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release $CMAKE_ARGS
    - cmake --build build --parallel $(nproc)
    - arm-none-eabi-size build/firmware.elf
  artifacts:
    paths:
      - build/*.elf
      - build/*.bin
      - build/*.hex
      - build/*.map
    expire_in: 1 week

build:stm32f7:
  stage: build
  <<: *ccache
  variables:
    CCACHE_DIR: "$CI_PROJECT_DIR/.ccache"
    CMAKE_ARGS: "-DBOARD_STM32F7=ON"
  script:
    - cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release $CMAKE_ARGS
    - cmake --build build --parallel $(nproc)
  artifacts:
    paths:
      - build/*.elf
      - build/*.bin
    expire_in: 1 week

### ============================================================================
### 测试阶段
### ============================================================================

unit_tests:
  stage: test
  image: $CI_REGISTRY/embedded/test:latest
  script:
    - pytest tests/unit --cov=src --cov-report=xml --cov-report=html
  coverage: '/TOTAL.*\s+(\d+%)$/'
  artifacts:
    reports:
      coverage_report:
        coverage_format: cobertura
        path: coverage.xml
    paths:
      - htmlcov/
    expire_in: 1 week

integration_tests:
  stage: test
  image: $CI_REGISTRY/embedded/qemu:latest
  services:
    - docker:dind
  script:
    - docker-compose -f docker-compose.test.yml up --abort-on-container-exit

### ============================================================================
### 分析阶段
### ============================================================================

sonarqube:
  stage: analyze
  image:
    name: sonarsource/sonar-scanner-cli:latest
    entrypoint: [""]
  variables:
    SONAR_USER_HOME: "${CI_PROJECT_DIR}/.sonar"
    GIT_DEPTH: "0"
  cache:
    key: "${CI_JOB_NAME}"
    paths:
      - .sonar/cache
  script:
    - sonar-scanner
      -Dsonar.projectKey=${CI_PROJECT_NAME}
      -Dsonar.sources=src
      -Dsonar.host.url=${SONAR_HOST_URL}
      -Dsonar.login=${SONAR_TOKEN}
  allow_failure: true

code_quality:
  stage: analyze
  image: docker:stable
  services:
    - docker:stable-dind
  script:
    - docker run --rm
        --env CODE_QUALITY_REPORT=true
        --volume "$PWD:/code"
        --volume /var/run/docker.sock:/var/run/docker.sock
        "registry.gitlab.com/gitlab-org/ci-cd/codequality:latest" /code
  artifacts:
    reports:
      codequality: gl-code-quality-report.json
    expire_in: 1 week
  allow_failure: true

### ============================================================================
### 部署阶段
### ============================================================================

deploy:staging:
  stage: deploy
  script:
    - echo "Deploying to staging environment"
    - ./scripts/deploy-staging.sh
  environment:
    name: staging
    url: https://staging.example.com
  only:
    - develop

deploy:production:
  stage: deploy
  script:
    - echo "Deploying to production environment"
    - ./scripts/deploy-production.sh
  environment:
    name: production
    url: https://production.example.com
  only:
    - main
  when: manual

### ============================================================================
### 容器镜像构建
### ============================================================================

docker:build:
  stage: deploy
  image: docker:latest
  services:
    - docker:dind
  before_script:
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
  script:
    - docker build -t $CI_REGISTRY/embedded/dev:latest -f Dockerfile.dev .
    - docker build -t $CI_REGISTRY/embedded/test:latest -f Dockerfile.test .
    - docker push $CI_REGISTRY/embedded/dev:latest
    - docker push $CI_REGISTRY/embedded/test:latest
  only:
    - schedules
    - web
```

---

## 10. 安全扫描

### 10.1 Trivy 安全配置

```yaml
### trivy.yml
### Trivy 安全扫描配置

### 扫描配置
scan:
  # 扫描类型
  scanners:
    - vuln      # 漏洞扫描
    - misconfig # 配置错误
    - secret    # 密钥扫描

  # 漏洞数据库
  db:
    repository: ghcr.io/aquasecurity/trivy-db
    java-repository: ghcr.io/aquasecurity/trivy-java-db

  # 离线模式
  offline: false

### 严重性级别
severity:
  - HIGH
  - CRITICAL

### 忽略的文件
skip-files:
  - "**/*.test"
  - "**/testdata/**"
  - "**/vendor/**"

### 忽略的目录
skip-dirs:
  - ".git"
  - "build"
  - "docs"

### 报告格式
format: sarif
output: trivy-results.sarif

### 退出代码
exit-code: 1

### 忽略未修复的漏洞
ignore-unfixed: false
```

### 10.2 GitHub Actions 安全扫描工作流

```yaml
### .github/workflows/security-scan.yml
### 安全扫描专用工作流

name: Security Scan

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]
  schedule:
    # 每天凌晨运行
    - cron: '0 0 * * *'

jobs:
  # Trivy 漏洞扫描
  trivy:
    name: Trivy Scan
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Run Trivy filesystem scan
        uses: aquasecurity/trivy-action@master
        with:
          scan-type: 'fs'
          scan-ref: '.'
          format: 'sarif'
          output: 'trivy-fs-results.sarif'
          severity: 'HIGH,CRITICAL'
          exit-code: '0'

      - name: Run Trivy config scan
        uses: aquasecurity/trivy-action@master
        with:
          scan-type: 'config'
          scan-ref: '.'
          format: 'sarif'
          output: 'trivy-config-results.sarif'
          severity: 'HIGH,CRITICAL'

      - name: Upload results to GitHub Security
        uses: github/codeql-action/upload-sarif@v2
        if: always()
        with:
          sarif_file: 'trivy-fs-results.sarif'

      - name: Upload artifacts
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: trivy-results
          path: trivy-*.sarif

  # Snyk 扫描
  snyk:
    name: Snyk Scan
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Run Snyk to check for vulnerabilities
        uses: snyk/actions/node@master
        continue-on-error: true
        env:
          SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}
        with:
          args: --severity-threshold=high --sarif-file-output=snyk-results.sarif

      - name: Upload results to GitHub Security
        uses: github/codeql-action/upload-sarif@v2
        if: always()
        with:
          sarif_file: snyk-results.sarif

  # 固件安全扫描
  firmware-scan:
    name: Firmware Security Analysis
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Build firmware
        uses: docker://ghcr.io/${{ github.repository }}/embedded-dev:latest
        with:
          args: bash -c "cmake -B build && cmake --build build"

      - name: Run Binwalk analysis
        uses: docker://cybercamp/binwalk
        with:
          args: binwalk -B build/firmware.bin

      - name: Check for hardcoded secrets
        run: |
          # 扫描固件中的硬编码密钥
          strings build/firmware.elf | grep -i -E "(password|secret|key|token)" || true

  # 依赖扫描
  dependency-check:
    name: Dependency Check
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Run OWASP Dependency Check
        uses: dependency-check/Dependency-Check_Action@main
        with:
          project: 'embedded-firmware'
          path: '.'
          format: 'ALL'
          args: >
            --enableRetired
            --enableExperimental

      - name: Upload results
        uses: actions/upload-artifact@v4
        with:
          name: dependency-check-report
          path: reports/
```

### 10.3 安全扫描 Docker Compose

```yaml
### docker-compose.security.yml
### 安全扫描服务配置

version: '3.8'

services:
  # Trivy 扫描服务
  trivy-scan:
    image: aquasec/trivy:latest
    volumes:
      - .:/scan:ro
      - ./security-reports:/reports
    command: >
      fs /scan
        --severity HIGH,CRITICAL
        --format sarif
        --output /reports/trivy-results.sarif
    profiles:
      - security

  # 固件分析服务
  firmware-analysis:
    image: refirmlabs/binwalk:latest
    volumes:
      - ./build:/firmware:ro
      - ./security-reports:/reports
    command: >
      bash -c "
        binwalk -B /firmware/firmware.bin > /reports/binwalk.txt &&
        binwalk -e /firmware/firmware.bin -C /reports/extracted/
      "
    profiles:
      - security

  # 密钥扫描
  secret-scan:
    image: zricethezav/gitleaks:latest
    volumes:
      - .:/scan:ro
      - ./security-reports:/reports
    command: >
      detect /scan
        --verbose
        --report-format json
        --report-path /reports/gitleaks-report.json
    profiles:
      - security

  # SBOM 生成
  sbom-generator:
    image: anchore/syft:latest
    volumes:
      - .:/scan:ro
      - ./security-reports:/reports
    command: >
      /scan
        -o spdx-json=/reports/sbom.spdx.json
        -o cyclonedx-json=/reports/sbom.cyclonedx.json
    profiles:
      - security
```

---

## 11. 实际项目：完整容器化STM32开发环境

### 11.1 项目结构

```
stm32-containerized-project/
├── .devcontainer/
│   ├── devcontainer.json      # Dev Container 配置
│   ├── Dockerfile             # 开发容器镜像
│   └── post-create.sh         # 创建后脚本
├── .github/
│   └── workflows/
│       ├── ci.yml             # CI/CD 流水线
│       └── security.yml       # 安全扫描
├── docker/
│   ├── Dockerfile.dev         # 开发环境
│   ├── Dockerfile.build       # 构建环境
│   ├── Dockerfile.test        # 测试环境
│   └── docker-compose.yml     # 完整编排
├── scripts/
│   ├── build.sh               # 构建脚本
│   ├── flash.sh               # 烧录脚本
│   ├── debug.sh               # 调试脚本
│   └── test.sh                # 测试脚本
├── src/
│   ├── main.c
│   ├── startup_stm32f407xx.s
│   └── ...
├── tests/
│   ├── unit/                  # 单元测试
│   └── integration/           # 集成测试
├── CMakeLists.txt
├── Makefile
└── README.md
```

### 11.2 完整 Dockerfile

```dockerfile
### ============================================================================
### docker/Dockerfile.stm32-complete
### 完整的 STM32 容器化开发环境
### ============================================================================

FROM ubuntu:22.04 AS stm32-toolchain

### 构建参数
ARG ARM_VERSION=12.2.rel1
ARG STM32CUBEMX_VERSION=6.9.0
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=1000

### 环境变量
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Asia/Shanghai \
    ARM_VERSION=${ARM_VERSION} \
    PATH="/opt/gcc-arm-none-eabi/bin:/opt/stlink:/opt/openocd/bin:${PATH}"

LABEL maintainer="stm32-dev@company.com"
LABEL description="Complete STM32 Development Environment"
LABEL version="2.0.0"

### =============================================================================
## 阶段 1: 安装基础依赖
### =============================================================================
RUN apt-get update && apt-get install -y --no-install-recommends \
    # 基础工具
    build-essential \
    ca-certificates \
    curl \
    wget \
    git \
    git-lfs \
    ssh \
    # 构建工具
    cmake \
    ninja-build \
    make \
    meson \
    # 调试工具
    gdb-multiarch \
    minicom \
    picocom \
    screen \
    # Python 环境
    python3 \
    python3-pip \
    python3-venv \
    python3-dev \
    # USB 支持
    libusb-1.0-0-dev \
    libusb-dev \
    libftdi1-dev \
    libhidapi-dev \
    udev \
    # 库文件
    libncurses5-dev \
    libncursesw5-dev \
    libssl-dev \
    libffi-dev \
    # 工具
    doxygen \
    graphviz \
    clang-format \
    clang-tidy \
    cppcheck \
    # 压缩工具
    xz-utils \
    bzip2 \
    unzip \
    p7zip-full \
    # 其他
    vim \
    nano \
    less \
    tree \
    htop \
    file \
    jq \
    bc \
    xxd \
    # 清理
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

### =============================================================================
### 阶段 2: 安装 ARM GCC 工具链
### =============================================================================
RUN echo "Installing ARM GCC ${ARM_VERSION}..." && \
    mkdir -p /opt/gcc-arm-none-eabi && \
    curl -fsSL \
        "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_VERSION}/binrel/arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz" | \
    tar -xJC /opt/gcc-arm-none-eabi --strip-components=1 && \
    arm-none-eabi-gcc --version && \
    echo "ARM GCC installed successfully"

### =============================================================================
### 阶段 3: 安装 OpenOCD
### =============================================================================
RUN echo "Building OpenOCD from source..." && \
    cd /tmp && \
    git clone --depth 1 --branch v0.12.0 \
        https://github.com/openocd-org/openocd.git && \
    cd openocd && \
    ./bootstrap && \
    ./configure \
        --prefix=/opt/openocd \
        --enable-stlink \
        --enable-jlink \
        --enable-ftdi \
        --enable-cmsis-dap && \
    make -j$(nproc) && \
    make install && \
    cd / && \
    rm -rf /tmp/openocd && \
    openocd --version

### =============================================================================
### 阶段 4: 安装 ST-Link 工具
### =============================================================================
RUN echo "Installing ST-Link tools..." && \
    cd /tmp && \
    git clone --depth 1 --branch v1.7.0 \
        https://github.com/stlink-org/stlink.git && \
    cd stlink && \
    cmake -B build \
        -DCMAKE_INSTALL_PREFIX=/opt/stlink \
        -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --parallel && \
    cmake --install build && \
    cd / && \
    rm -rf /tmp/stlink

### =============================================================================
### 阶段 5: 安装 Python 工具
### =============================================================================
RUN pip3 install --no-cache-dir --upgrade pip && \
    pip3 install --no-cache-dir \
    pyserial \
    pyocd \
    mbed-cli \
    imgtool \
    intelhex \
    srecord \
    crcmod \
    pycryptodome \
    cbor2 \
    click

### =============================================================================
### 阶段 6: 配置用户和权限
### =============================================================================
RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME && \
    usermod -aG dialout $USERNAME && \
    usermod -aG plugdev $USERNAME && \
    echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME && \
    chmod 0440 /etc/sudoers.d/$USERNAME

### 创建工具目录
RUN mkdir -p /workspace && \
    chown -R $USERNAME:$USERNAME /workspace

### =============================================================================
### 阶段 7: 添加实用脚本
### =============================================================================
COPY --chown=$USERNAME:$USERNAME <<'EOF' /usr/local/bin/build-firmware
#!/bin/bash
### 固件构建脚本

set -e

BUILD_DIR=${BUILD_DIR:-build}
BUILD_TYPE=${BUILD_TYPE:-Debug}

echo "Building STM32 firmware..."
echo "Build type: $BUILD_TYPE"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake

ninja

echo "Build complete!"
echo "Firmware: $BUILD_DIR/firmware.bin"
arm-none-eabi-size firmware.elf
EOF

COPY --chown=$USERNAME:$USERNAME <<'EOF' /usr/local/bin/flash-stm32
#!/bin/bash
### STM32 烧录脚本

FIRMWARE=${1:-build/firmware.bin}
INTERFACE=${INTERFACE:-stlink}
TARGET=${TARGET:-stm32f4x}

echo "Flashing $FIRMWARE to STM32..."

openocd \
    -f interface/${INTERFACE}.cfg \
    -f target/${TARGET}.cfg \
    -c "program $FIRMWARE reset exit 0x08000000"

echo "Flash complete!"
EOF

COPY --chown=$USERNAME:$USERNAME <<'EOF' /usr/local/bin/debug-stm32
#!/bin/bash
### STM32 调试脚本

ELF_FILE=${1:-build/firmware.elf}
INTERFACE=${INTERFACE:-stlink}
TARGET=${TARGET:-stm32f4x}

echo "Starting debug session for $ELF_FILE..."

### 启动 OpenOCD 后台进程
openocd \
    -f interface/${INTERFACE}.cfg \
    -f target/${TARGET}.cfg &

OCD_PID=$!

### 等待 OpenOCD 启动
sleep 2

### 启动 GDB
arm-none-eabi-gdb "$ELF_FILE" \
    -ex "target remote localhost:3333" \
    -ex "monitor reset halt" \
    -ex "load" \
    -ex "break main" \
    -ex "continue"

### 清理
kill $OCD_PID 2>/dev/null
EOF

RUN chmod +x /usr/local/bin/build-firmware \
    /usr/local/bin/flash-stm32 \
    /usr/local/bin/debug-stm32

### =============================================================================
### 阶段 8: 设置工作目录
### =============================================================================
WORKDIR /workspace
USER $USERNAME

### 默认命令
CMD ["/bin/bash"]
```

### 11.3 Docker Compose 完整配置

```yaml
### docker/docker-compose.stm32.yml
### STM32 完整开发环境

version: '3.8'

services:
  # ==========================================================================
  # 主开发服务
  # ==========================================================================
  stm32-dev:
    build:
      context: ..
      dockerfile: docker/Dockerfile.stm32-complete
      args:
        USER_UID: ${USER_UID:-1000}
        USER_GID: ${USER_GID:-1000}
    image: stm32-dev:latest
    container_name: stm32-dev

    stdin_open: true
    tty: true

    # 权限设置
    privileged: true

    # 设备访问
    devices:
      - /dev/bus/usb:/dev/bus/usb
      - /dev/ttyUSB0:/dev/ttyUSB0
      - /dev/ttyACM0:/dev/ttyACM0

    # 卷挂载
    volumes:
      # 项目代码
      - ../:/workspace:cached
      # ccache 缓存
      - stm32-ccache:/home/developer/.ccache
      # 下载缓存
      - stm32-downloads:/home/developer/downloads
      # SSH 密钥
      - ~/.ssh:/home/developer/.ssh:ro
      # Git 配置
      - ~/.gitconfig:/home/developer/.gitconfig:ro
      # X11 转发（用于 GUI 工具）
      - /tmp/.X11-unix:/tmp/.X11-unix:rw

    # 环境变量
    environment:
      - DISPLAY=${DISPLAY:-:0}
      - CCACHE_DIR=/home/developer/.ccache
      - TERM=xterm-256color

    # 端口映射
    ports:
      - "3333:3333"      # GDB
      - "4444:4444"      # OpenOCD Telnet
      - "6666:6666"      # OpenOCD TCL

    # 网络
    networks:
      - stm32-net

    # 工作目录
    working_dir: /workspace

    # 命令
    command: /bin/bash

  # ==========================================================================
  # GDB 服务器服务
  # ==========================================================================
  gdb-server:
    image: stm32-dev:latest
    container_name: stm32-gdb-server

    privileged: true

    devices:
      - /dev/bus/usb:/dev/bus/usb

    ports:
      - "3333:3333"
      - "4444:4444"

    command: >
      openocd
      -f interface/stlink.cfg
      -f target/stm32f4x.cfg
      -c "gdb_port 3333"
      -c "telnet_port 4444"

    networks:
      - stm32-net

    restart: unless-stopped

  # ==========================================================================
  # 文档服务器
  # ==========================================================================
  docs:
    image: nginx:alpine
    container_name: stm32-docs

    volumes:
      - ../docs/html:/usr/share/nginx/html:ro

    ports:
      - "8080:80"

    networks:
      - stm32-net

  # ==========================================================================
  # 测试服务
  # ==========================================================================
  test:
    image: stm32-dev:latest
    container_name: stm32-test

    volumes:
      - ../:/workspace:ro
      - stm32-test-results:/workspace/test-results

    command: >
      bash -c "
        cd /workspace &&
        cmake -B build-test -G Ninja -DBUILD_TESTING=ON &&
        cmake --build build-test &&
        ctest --test-dir build-test --output-on-failure
      "

    profiles:
      - test

networks:
  stm32-net:
    driver: bridge
    ipam:
      config:
        - subnet: 172.30.0.0/16

volumes:
  stm32-ccache:
    driver: local
  stm32-downloads:
    driver: local
  stm32-test-results:
    driver: local
```

### 11.4 项目管理脚本

```bash
#!/bin/bash
### stm32-manager.sh
### STM32 项目管理脚本

set -e

### 配置
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_NAME="stm32-project"
COMPOSE_FILE="$SCRIPT_DIR/docker/docker-compose.stm32.yml"

### 颜色
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

### 显示帮助
show_help() {
    cat << EOF
STM32 项目管理脚本

Usage: $0 <command> [options]

Commands:
    setup       初始化开发环境
    start       启动开发容器
    stop        停止开发容器
    shell       进入开发容器 shell
    build       构建固件
    flash       烧录固件到设备
    debug       启动调试会话
    test        运行测试
    clean       清理构建产物
    docs        生成文档
    update      更新容器镜像
    status      查看环境状态

Options:
    -h, --help  显示此帮助信息

Examples:
    $0 setup                    # 初始化环境
    $0 build --release          # 发布构建
    $0 flash --target stm32f4   # 烧录到指定目标
EOF
}

### 初始化环境
cmd_setup() {
    echo -e "${YELLOW}Setting up STM32 development environment...${NC}"

    # 检查 Docker
    if ! command -v docker &> /dev/null; then
        echo -e "${RED}Docker is not installed!${NC}"
        echo "Please install Docker first: https://docs.docker.com/get-docker/"
        exit 1
    fi

    # 检查 Docker Compose
    if ! docker-compose --version &> /dev/null; then
        echo -e "${RED}Docker Compose is not installed!${NC}"
        exit 1
    fi

    # 创建必要目录
    mkdir -p build docs/html

    # 构建镜像
    echo -e "${YELLOW}Building container images...${NC}"
    docker-compose -f "$COMPOSE_FILE" build

    # 设置 udev 规则（Linux）
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo -e "${YELLOW}Setting up udev rules...${NC}"
        sudo cp "$SCRIPT_DIR/docker/udev/99-stm32.rules" /etc/udev/rules.d/ 2>/dev/null || true
        sudo udevadm control --reload-rules 2>/dev/null || true
    fi

    echo -e "${GREEN}Setup complete!${NC}"
    echo "Run '$0 start' to start the development environment"
}

### 启动环境
cmd_start() {
    echo -e "${YELLOW}Starting STM32 development environment...${NC}"

    export USER_UID=$(id -u)
    export USER_GID=$(id -g)

    docker-compose -f "$COMPOSE_FILE" up -d stm32-dev

    echo -e "${GREEN}Environment started!${NC}"
    echo "Run '$0 shell' to enter the container"
}

### 停止环境
cmd_stop() {
    echo -e "${YELLOW}Stopping environment...${NC}"
    docker-compose -f "$COMPOSE_FILE" down
    echo -e "${GREEN}Environment stopped.${NC}"
}

### 进入 shell
cmd_shell() {
    docker-compose -f "$COMPOSE_FILE" exec stm32-dev /bin/bash
}

### 构建固件
cmd_build() {
    local build_type="Debug"
    local target=""

    while [[ $# -gt 0 ]]; do
        case $1 in
            --release)
                build_type="Release"
                shift
                ;;
            --target)
                target="$2"
                shift 2
                ;;
            *)
                shift
                ;;
        esac
    done

    echo -e "${YELLOW}Building firmware (type: $build_type)...${NC}"

    local cmake_args=""
    if [[ -n "$target" ]]; then
        cmake_args="-DTARGET_BOARD=$target"
    fi

    docker-compose -f "$COMPOSE_FILE" exec -T stm32-dev bash -c "
        mkdir -p build &&
        cd build &&
        cmake .. -G Ninja \
            -DCMAKE_BUILD_TYPE=$build_type \
            -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake \
            $cmake_args &&
        ninja
    "

    echo -e "${GREEN}Build complete!${NC}"
}

### 烧录固件
cmd_flash() {
    local firmware="build/firmware.bin"
    local interface="stlink"

    while [[ $# -gt 0 ]]; do
        case $1 in
            --firmware)
                firmware="$2"
                shift 2
                ;;
            --interface)
                interface="$2"
                shift 2
                ;;
            *)
                shift
                ;;
        esac
    done

    echo -e "${YELLOW}Flashing $firmware...${NC}"

    docker-compose -f "$COMPOSE_FILE" exec stm32-dev flash-stm32 "$firmware"

    echo -e "${GREEN}Flash complete!${NC}"
}

### 调试
cmd_debug() {
    local elf_file="${1:-build/firmware.elf}"

    echo -e "${YELLOW}Starting debug session...${NC}"

    # 启动 GDB 服务器
    docker-compose -f "$COMPOSE_FILE" up -d gdb-server
    sleep 2

    # 启动 GDB 客户端
    docker-compose -f "$COMPOSE_FILE" exec stm32-dev bash -c "
        arm-none-eabi-gdb $elf_file \
            -ex 'target remote gdb-server:3333' \
            -ex 'monitor reset halt' \
            -ex 'load' \
            -ex 'break main'
    "
}

### 运行测试
cmd_test() {
    echo -e "${YELLOW}Running tests...${NC}"

    docker-compose -f "$COMPOSE_FILE" --profile test run --rm test

    echo -e "${GREEN}Tests complete!${NC}"
}

### 清理
cmd_clean() {
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"

    docker-compose -f "$COMPOSE_FILE" exec -T stm32-dev bash -c "
        rm -rf build/*
    "

    echo -e "${GREEN}Clean complete!${NC}"
}

### 生成文档
cmd_docs() {
    echo -e "${YELLOW}Generating documentation...${NC}"

    docker-compose -f "$COMPOSE_FILE" exec -T stm32-dev bash -c "
        cd /workspace &&
        doxygen Doxyfile
    "

    # 启动文档服务器
    docker-compose -f "$COMPOSE_FILE" up -d docs

    echo -e "${GREEN}Documentation generated!${NC}"
    echo "View at: http://localhost:8080"
}

### 更新镜像
cmd_update() {
    echo -e "${YELLOW}Updating container images...${NC}"

    docker-compose -f "$COMPOSE_FILE" pull
    docker-compose -f "$COMPOSE_FILE" build --no-cache

    echo -e "${GREEN}Update complete!${NC}"
}

### 查看状态
cmd_status() {
    echo -e "${BLUE}Container Status:${NC}"
    docker-compose -f "$COMPOSE_FILE" ps

    echo ""
    echo -e "${BLUE}Image Versions:${NC}"
    docker-compose -f "$COMPOSE_FILE" exec -T stm32-dev bash -c "
        echo 'ARM GCC:' && arm-none-eabi-gcc --version | head -n 1
        echo 'OpenOCD:' && openocd --version | head -n 1
        echo 'CMake:' && cmake --version | head -n 1
    "
}

### 主函数
main() {
    case "${1:-}" in
        setup) cmd_setup ;;
        start) cmd_start ;;
        stop) cmd_stop ;;
        shell) cmd_shell ;;
        build) shift; cmd_build "$@" ;;
        flash) shift; cmd_flash "$@" ;;
        debug) shift; cmd_debug "$@" ;;
        test) cmd_test ;;
        clean) cmd_clean ;;
        docs) cmd_docs ;;
        update) cmd_update ;;
        status) cmd_status ;;
        -h|--help|help) show_help ;;
        *)
            echo -e "${RED}Unknown command: ${1:-}${NC}"
            show_help
            exit 1
            ;;
    esac
}

main "$@"
```

### 11.5 使用示例

```bash
### 1. 初始化环境
./stm32-manager.sh setup

### 2. 启动开发环境
./stm32-manager.sh start

### 3. 进入开发容器
./stm32-manager.sh shell

### 4. 在容器内构建项目
build-firmware
### 或使用 CMake 直接构建
cmake -B build -G Ninja && cmake --build build

### 5. 烧录固件
./stm32-manager.sh flash

### 6. 启动调试
./stm32-manager.sh debug

### 7. 运行测试
./stm32-manager.sh test

### 8. 查看状态
./stm32-manager.sh status

### 9. 停止环境
./stm32-manager.sh stop
```

---

## 附录

### A. 常见问题解答

#### Q1: 如何解决 USB 设备权限问题？

在 Linux 系统上，需要配置 udev 规则：

```bash
### 创建 udev 规则文件
sudo tee /etc/udev/rules.d/99-stm32.rules << 'EOF'
### ST-Link V2
SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="3748", MODE="0666", GROUP="plugdev"
### ST-Link V2-1
SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="374b", MODE="0666", GROUP="plugdev"
### ST-Link V3
SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="374f", MODE="0666", GROUP="plugdev"
### CMSIS-DAP
SUBSYSTEM=="usb", ATTR{idVendor}=="0d28", ATTR{idProduct}=="0204", MODE="0666", GROUP="plugdev"
### J-Link
SUBSYSTEM=="usb", ATTR{idVendor}=="1366", ATTR{idProduct}=="0101", MODE="0666", GROUP="plugdev"
EOF

### 重新加载规则
sudo udevadm control --reload-rules
sudo udevadm trigger
```

#### Q2: 如何在 macOS 上使用 USB 设备？

macOS 不支持直接传递 USB 设备到容器。解决方案：

1. 使用远程 GDB 服务器
2. 使用虚拟化软件（如 VirtualBox）的 USB 透传
3. 使用网络调试器

#### Q3: 如何减小镜像大小？

```dockerfile
### 使用多阶段构建
FROM ubuntu:22.04 AS builder
### ... 构建步骤 ...

FROM ubuntu:22.04 AS runtime
COPY --from=builder /opt/toolchain /opt/toolchain
### ... 运行时配置 ...

### 清理不必要的文件
RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
```

### B. 参考资源

- [Docker 官方文档](https://docs.docker.com/)
- [VS Code Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers)
- [OpenOCD 文档](https://openocd.org/doc/html/)
- [ARM GCC 工具链](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain)
- [STM32 官方资源](https://www.st.com/en/microcontrollers-microprocessors.html)

---

*本文档最后更新: 2026年3月*
*版本: 1.0.0*


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
