# vcpkg包管理指南

> **层级定位**: 07 Modern Toolchain / 04 Package Management
> **难度级别**: L3 进阶
> **预估学习时间**: 3-4小时

---

## 📋 vcpkg简介

微软开源的C/C++包管理器：

- 跨平台支持（Windows/Linux/macOS）
- 与Visual Studio深度集成
- 源码编译安装，保证兼容性
- 版本锁定支持（manifest模式）
- 超过2000+个开源库

---

## 🚀 快速开始

### 安装

```bash
# 克隆仓库

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../README.md](../../README.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/README.md](../../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/README.md](../../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

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
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/README.md](../../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/README.md](../../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/README.md](../../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/README.md](../../04_Industrial_Scenarios/README.md) | 工业实践场景 |

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
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/README.md) |

---

git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # Linux/macOS
./bootstrap-vcpkg.bat # Windows
```

### 基础用法

```bash
# 安装包
./vcpkg install zlib
./vcpkg install curl[ssl]

# 列出已安装
./vcpkg list

# 更新
./vcpkg update
./vcpkg upgrade
```

### 与CMake集成

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 清单模式 (vcpkg.json)

```json
{
  "name": "myproject",
  "version": "1.0.0",
  "dependencies": [
    "zlib",
    {
      "name": "curl",
      "features": ["ssl"]
    }
  ]
}
```

---

## ⚖️ 经典模式 vs 清单模式

### 经典模式 (Classic Mode)

经典模式将包安装到vcpkg全局目录，所有项目共享同一套库：

```bash
# 全局安装，所有项目可用
vcpkg install boost fmt spdlog

# CMake自动查找
find_package(Boost REQUIRED)
find_package(fmt CONFIG REQUIRED)
```

**特点：**

- 安装位置：`vcpkg/installed/` 目录
- 版本由vcpkg仓库的portfile决定
- 适合小型项目或快速原型

**缺点：**

- 版本冲突：项目A需要Boost 1.78，项目B需要Boost 1.82
- 难以复现构建环境
- 删除项目后残留依赖

### 清单模式 (Manifest Mode)

清单模式通过 `vcpkg.json` 定义项目依赖，每个项目独立管理：

```json
{
  "name": "myapp",
  "version": "1.0.0",
  "dependencies": [
    "boost-system",
    "fmt",
    {
      "name": "openssl",
      "version>=": "3.0.0"
    }
  ]
}
```

**CMake集成（清单模式自动启用）：**

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyApp)

# 清单模式下自动处理依赖
find_package(Boost REQUIRED COMPONENTS system)
find_package(fmt CONFIG REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp Boost::system fmt::fmt)
```

**对比表：**

| 特性 | 经典模式 | 清单模式 |
|------|----------|----------|
| 依赖定义 | 命令行安装 | vcpkg.json |
| 版本控制 | 困难 | 内置支持 |
| 项目隔离 | ❌ 共享 | ✅ 独立 |
| 可复现构建 | ❌ 难保证 | ✅ 锁定文件 |
| CI/CD友好度 | 低 | 高 |

---

## 🔒 版本覆盖与Baseline

### 版本覆盖 (Version Override)

当需要特定版本的包时，使用 `overrides`：

```json
{
  "name": "myproject",
  "version": "1.0.0",
  "dependencies": ["fmt", "boost-system"],
  "overrides": [
    {
      "name": "fmt",
      "version": "8.1.1"
    },
    {
      "name": "boost-system",
      "version": "1.78.0"
    }
  ]
}
```

### Baseline（基线版本）

Baseline锁定整个vcpkg仓库的版本快照：

```json
{
  "name": "myproject",
  "version": "1.0.0",
  "builtin-baseline": "a1b2c3d4e5f6...",
  "dependencies": ["fmt", "spdlog"]
}
```

**获取baseline commit：**

```bash
# 查看当前vcpkg仓库的commit
cd vcpkg && git rev-parse HEAD

# 或使用特定日期版本
git log --oneline --before="2024-01-01" | head -1
```

### 锁定文件 (vcpkg-configuration.json)

```json
{
  "default-registry": {
    "kind": "git",
    "repository": "https://github.com/microsoft/vcpkg",
    "baseline": "a1b2c3d4e5f6..."
  }
}
```

---

## 📦 自定义Registry

### 什么是Registry

Registry是vcpkg包的来源仓库，可以是：

- 官方registry（GitHub: microsoft/vcpkg）
- 私有Git仓库
- 本地文件目录
- 网络文件服务器

### 配置多个Registry

```json
{
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/mycompany/vcpkg-registry",
      "baseline": "abc123...",
      "packages": ["mycompany-*"]
    },
    {
      "kind": "filesystem",
      "path": "/path/to/local/registry",
      "packages": ["internal-*"]
    }
  ]
}
```

### 创建私有Registry

**目录结构：**

```
my-registry/
├── versions/
│   ├── baseline.json
│   └── m-
│       └── mylib.json
└── ports/
    └── mylib/
        ├── portfile.cmake
        └── vcpkg.json
```

**portfile.cmake 示例：**

```cmake
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO mycompany/mylib
    REF v1.2.3
    SHA512 abcdef...
)

vcpkg_cmake_configure(SOURCE_PATH ${SOURCE_PATH})
vcpkg_cmake_install()
vcpkg_cmake_config_fixup()
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/mylib RENAME copyright)
```

**vcpkg.json (port)：**

```json
{
  "name": "mylib",
  "version": "1.2.3",
  "description": "My company's internal library",
  "dependencies": ["fmt"]
}
```

---

## 💾 二进制缓存配置

### 为什么需要二进制缓存

- 避免重复编译相同配置的包
- 加速CI/CD构建
- 节省开发者时间

### 本地缓存

```bash
# 设置环境变量
export VCPKG_BINARY_SOURCES="files,/path/to/cache,readwrite"

# Windows
set VCPKG_BINARY_SOURCES=files,C:\vcpkg-cache,readwrite
```

### NuGet缓存（推荐用于团队）

```bash
# 配置NuGet源
export VCPKG_BINARY_SOURCES="nuget,https://myfeed.pkgs.visualstudio.com/vcpkg,readwrite"

# 需要配置认证
vcpkg fetch nuget
./nuget sources add -Name MyFeed -Source https://myfeed.pkgs.visualstudio.com/vcpkg
./nuget setapikey <API_KEY> -Source https://myfeed.pkgs.visualstudio.com/vcpkg
```

### Azure Blob存储

```bash
export VCPKG_BINARY_SOURCES="x-azblob,https://mystorage.blob.core.windows.net/vcpkg-cache,sas_token"
```

### GitHub Actions缓存

```bash
export VCPKG_BINARY_SOURCES="x-gha,readwrite"
```

### 多源配置

```bash
# 组合多个缓存源
export VCPKG_BINARY_SOURCES="clear;x-gha,read;nuget,https://myfeed...,readwrite;files,/local/cache,readwrite"
```

---

## 🔄 CI/CD集成示例

### GitHub Actions

```yaml
name: Build with vcpkg

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]

    steps:
    - uses: actions/checkout@v4

    - name: Setup vcpkg
      uses: lukka/run-vcpkg@v11
      with:
        vcpkgGitCommitId: 'a1b2c3d4e5f6...'  # 锁定baseline

    - name: Configure CMake
      uses: lukka/run-cmake@v10
      with:
        cmakeListsTxtPath: '${{ github.workspace }}/CMakeLists.txt'
        configurePreset: 'ci'

    # 或使用纯shell方式
    - name: Build (Shell)
      run: |
        cmake -B build -S . \
          -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
        cmake --build build
```

### GitLab CI

```yaml
variables:
  VCPKG_BINARY_SOURCES: "clear;x-azblob,https://myblob.blob.core.windows.net/cache,$AZURE_SAS_TOKEN,readwrite"

stages:
  - build

build:linux:
  stage: build
  image: ubuntu:22.04
  before_script:
    - apt-get update && apt-get install -y cmake ninja-build git curl
    - git clone --depth 1 https://github.com/microsoft/vcpkg.git $VCPKG_ROOT
    - $VCPKG_ROOT/bootstrap-vcpkg.sh
  script:
    - cmake -B build -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
    - cmake --build build
  cache:
    key: vcpkg-cache
    paths:
      - $VCPKG_ROOT/installed/
```

### Azure Pipelines

```yaml
trigger:
  - main

pool:
  vmImage: 'ubuntu-latest'

variables:
  - name: VCPKG_BINARY_SOURCES
    value: 'nuget,$(VCPKG_NUGET_FEED),readwrite'

steps:
- task: Cache@2
  inputs:
    key: 'vcpkg | "$(Agent.OS)" | vcpkg.json'
    restoreKeys: |
      vcpkg | "$(Agent.OS)"
    path: $(VCPKG_ROOT)/installed
  displayName: 'Cache vcpkg packages'

- script: |
    git clone --depth 1 --branch 2024.03.25 https://github.com/microsoft/vcpkg.git $(VCPKG_ROOT)
    $(VCPKG_ROOT)/bootstrap-vcpkg.sh
  displayName: 'Setup vcpkg'

- task: CMake@1
  inputs:
    cmakeArgs: '-DCMAKE_TOOLCHAIN_FILE=$(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake'

- task: CMake@1
  inputs:
    cmakeArgs: '--build $(Build.SourcesDirectory)/build'
```

### Docker集成

```dockerfile
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    cmake ninja-build git curl zip unzip tar

# 安装vcpkg
ENV VCPKG_ROOT=/opt/vcpkg
RUN git clone --depth 1 https://github.com/microsoft/vcpkg.git $VCPKG_ROOT
RUN $VCPKG_ROOT/bootstrap-vcpkg.sh

WORKDIR /app
COPY vcpkg.json .

# 利用Docker缓存层
RUN $VCPKG_ROOT/vcpkg install --feature-flags=manifests

COPY . .
RUN cmake -B build -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake && \
    cmake --build build
```

---

## 📚 进阶技巧

### 条件依赖

```json
{
  "dependencies": [
    {
      "name": "openssl",
      "platform": "linux | osx"
    },
    {
      "name": "openssl-windows",
      "platform": "windows"
    }
  ]
}
```

### 可选特性

```json
{
  "name": "mylib",
  "features": {
    "tests": {
      "description": "Build tests",
      "dependencies": ["gtest"]
    },
    "benchmarks": {
      "description": "Build benchmarks",
      "dependencies": ["benchmark"]
    }
  }
}
```

启用特性：

```bash
vcpkg install mylib[tests,benchmarks]
```

### 使用vcpkg工具链功能

```cmake
# 自动链接
set(VCPKG_TARGET_TRIPLET "x64-linux-dynamic")

# 静态链接运行时库（MSVC）
set(VCPKG_CRT_LINKAGE static)
```

---

**← [返回工具链主页](../README.md)**


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
