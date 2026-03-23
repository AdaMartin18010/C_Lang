# Conan包管理器指南

> **层级定位**: 07 Modern Toolchain / 04 Package Management
> **难度级别**: L3 进阶
> **预估学习时间**: 4-6小时

---

## 📋 Conan简介

Conan是C/C++的跨平台包管理器：

- 二进制包管理
- 多版本支持
- 跨平台
- 私有仓库支持

---


---

## 📑 目录

- [Conan包管理器指南](#conan包管理器指南)
  - [📋 Conan简介](#-conan简介)
  - [📑 目录](#-目录)
  - [🚀 快速开始](#-快速开始)
    - [安装](#安装)
    - [基础用法](#基础用法)
  - [📊 conanfile.txt vs conanfile.py 对比](#-conanfiletxt-vs-conanfilepy-对比)
    - [何时使用conanfile.txt](#何时使用conanfiletxt)
    - [何时使用conanfile.py](#何时使用conanfilepy)
  - [📚 高级配置](#-高级配置)
    - [conanfile.py 完整示例](#conanfilepy-完整示例)
    - [私有仓库](#私有仓库)
  - [⚙️ 选项和配置](#️-选项和配置)
    - [设置（Settings）vs 选项（Options）](#设置settingsvs-选项options)
    - [配置（Conf）系统](#配置conf系统)
    - [常用配置项](#常用配置项)
  - [🌐 交叉编译配置](#-交叉编译配置)
    - [创建交叉编译Profile](#创建交叉编译profile)
    - [使用交叉编译Profile](#使用交叉编译profile)
    - [主机（Host）vs 构建（Build）Profile](#主机hostvs-构建buildprofile)
    - [常用交叉编译场景](#常用交叉编译场景)
  - [🏢 Artifactory集成](#-artifactory集成)
    - [设置JFrog Artifactory](#设置jfrog-artifactory)
    - [仓库布局最佳实践](#仓库布局最佳实践)
    - [CI/CD自动化上传](#cicd自动化上传)
    - [包推广（Promotion）](#包推广promotion)
  - [🔧 CI/CD最佳实践](#-cicd最佳实践)
    - [GitHub Actions 集成](#github-actions-集成)
    - [GitLab CI 集成](#gitlab-ci-集成)
    - [Jenkins Pipeline](#jenkins-pipeline)
    - [缓存优化策略](#缓存优化策略)
  - [📖 参考资源](#-参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 🚀 快速开始

### 安装

```bash
pip install conan
```

### 基础用法

**conanfile.txt**:

```ini
[requires]
zlib/1.2.13
libcurl/8.0.1

[generators]
CMakeDeps
CMakeToolchain
```

**使用**:

```bash
conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

---

## 📊 conanfile.txt vs conanfile.py 对比

| 特性 | conanfile.txt | conanfile.py |
|------|--------------|--------------|
| 格式 | 简单的INI格式 | Python脚本 |
| 复杂度 | 适合简单项目 | 适合复杂项目 |
| 条件依赖 | 不支持 | 支持 |
| 自定义逻辑 | 不支持 | 支持 |
| 选项配置 | 有限支持 | 完整支持 |
| 构建自定义 | 不支持 | 支持 |

### 何时使用conanfile.txt

- 简单的应用程序项目
- 只需要声明式依赖管理
- 不需要条件逻辑或自定义构建步骤

### 何时使用conanfile.py

- 需要创建可重用的库包
- 需要条件依赖（如不同平台的依赖不同）
- 需要自定义构建逻辑
- 需要配置选项（如是否启用某些功能）

---

## 📚 高级配置

### conanfile.py 完整示例

```python
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake

class MyProject(ConanFile):
    name = "myproject"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    # 定义选项
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_ssl": [True, False]
    }

    # 默认选项值
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_ssl": True
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def requirements(self):
        self.requires("zlib/1.2.13")
        if self.options.with_ssl:
            self.requires("openssl/3.0.8")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
```

### 私有仓库

```bash
# 添加私有仓库

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

conan remote add myrepo https://myserver.com/conan

# 查看远程仓库列表
conan remote list

# 上传包
conan create . --user=myuser --channel=stable
conan upload mylib/1.0.0@myuser/stable -r=myrepo

# 搜索远程仓库的包
conan search "*" -r=myrepo
```

---

## ⚙️ 选项和配置

### 设置（Settings）vs 选项（Options）

| 类别 | Settings | Options |
|------|----------|---------|
| 定义位置 | `~/.conan2/settings.yml` | 在conanfile.py中定义 |
| 值来源 | 主机环境（编译器、OS等） | 包作者定义 |
| 典型值 | os, compiler, arch, build_type | shared, fPIC, with_xxx |
| 影响 | 包ID计算（不同设置=不同包） | 包ID计算（不同选项=不同包） |

### 配置（Conf）系统

```bash
# 命令行配置
conan install . -c tools.cmake.cmaketoolchain:generator=Ninja

# 配置文件（profile）
```ini
[settings]
arch=x86_64
build_type=Release
compiler=gcc
compiler.version=11
compiler.libcxx=libstdc++11
os=Linux

[options]
*:shared=True
*:fPIC=True

[conf]
tools.cmake.cmaketoolchain:generator=Ninja
tools.build:jobs=4
tools.system.package_manager:mode=install
tools.system.package_manager:sudo=True
```

### 常用配置项

```ini
# CMake相关
tools.cmake.cmaketoolchain:generator=Ninja
tools.cmake.cmake_program=/path/to/cmake

# 构建相关
tools.build:skip_test=True
tools.build:jobs=8
tools.build:compiler_executables={"c": "/usr/bin/gcc", "cpp": "/usr/bin/g++"}

# 系统包管理器
tools.system.package_manager:mode=install
tools.system.package_manager:sudo=True
```

---

## 🌐 交叉编译配置

### 创建交叉编译Profile

**arm-linux-gnueabihf**:

```ini
[settings]
arch=armv7
build_type=Release
compiler=gcc
compiler.version=11
compiler.libcxx=libstdc++11
os=Linux

[buildenv]
CC=arm-linux-gnueabihf-gcc
CXX=arm-linux-gnueabihf-g++
AR=arm-linux-gnueabihf-ar
STRIP=arm-linux-gnueabihf-strip

[conf]
tools.cmake.cmaketoolchain:generator=Ninja
tools.build:compiler_executables={"c": "arm-linux-gnueabihf-gcc", "cpp": "arm-linux-gnueabihf-g++"}
```

### 使用交叉编译Profile

```bash
# 安装依赖时使用交叉编译profile
conan install . --profile:host=arm-profile --profile:build=default --build=missing

# 构建时指定profile
cmake --preset conan-release-armv7
cmake --build --preset conan-release-armv7
```

### 主机（Host）vs 构建（Build）Profile

```bash
# host profile: 目标运行环境
# build profile: 当前构建环境

conan install . \
    --profile:host=arm-profile \
    --profile:build=default \
    --build=missing
```

### 常用交叉编译场景

| 目标平台 | arch | compiler |
|---------|------|----------|
| ARM Linux | armv7/armv8 | gcc |
| Android ARM64 | armv8 | clang |
| iOS ARM64 | armv8 | apple-clang |
| Raspberry Pi | armv6/armv7 | gcc |
| Windows ARM | armv8 | msvc |

---

## 🏢 Artifactory集成

### 设置JFrog Artifactory

```bash
# 添加Artifactory远程
conan remote add artifactory https://artifactory.company.com/artifactory/api/conan/conan-local

# 设置认证
conan remote login artifactory username -p password

# 或使用环境变量
export CONAN_LOGIN_USERNAME=username
export CONAN_PASSWORD=password
```

### 仓库布局最佳实践

```
Artifactory
├── conan-local      # 开发中的包
├── conan-stable     # 稳定版本
├── conan-thirdparty # 第三方包缓存
└── conan-virtual    # 虚拟聚合仓库
```

### CI/CD自动化上传

```bash
# 创建并上传包
conan create . --user=ci --channel=dev
conan upload "mylib/*" -r=artifactory --confirm

# 根据分支自动选择channel
if [ "$BRANCH" == "main" ]; then
    CHANNEL=stable
else
    CHANNEL=dev
fi
conan create . --user=ci --channel=$CHANNEL
conan upload "mylib/*@ci/$CHANNEL" -r=artifactory --confirm
```

### 包推广（Promotion）

```bash
# 从dev提升到stable
conan download mylib/1.0.0@ci/dev -r=artifactory
conan upload mylib/1.0.0@ci/stable -r=artifactory
```

---

## 🔧 CI/CD最佳实践

### GitHub Actions 集成

```yaml
name: Conan Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Release, Debug]

    steps:
    - uses: actions/checkout@v3

    - name: Setup Conan
      uses: conan-io/setup-conan@v1
      with:
        version: 2.0.0

    - name: Configure Conan
      run: |
        conan profile detect --force
        conan remote add myrepo ${{ secrets.ARTIFACTORY_URL }}
        conan remote login myrepo ${{ secrets.ARTIFACTORY_USER }} -p ${{ secrets.ARTIFACTORY_PASS }}

    - name: Cache Conan packages
      uses: actions/cache@v3
      with:
        path: ~/.conan2/p
        key: conan-${{ hashFiles('conanfile.py') }}

    - name: Install dependencies
      run: conan install . --build=missing -s build_type=${{ matrix.build_type }}

    - name: Build
      run: |
        cmake --preset conan-${{ matrix.build_type == 'Release' && 'release' || 'debug' }}
        cmake --build --preset conan-${{ matrix.build_type == 'Release' && 'release' || 'debug' }}

    - name: Test
      run: ctest --preset conan-${{ matrix.build_type == 'Release' && 'release' || 'debug' }}

    - name: Upload package
      if: github.ref == 'refs/heads/main'
      run: |
        conan create . --user=ci --channel=stable
        conan upload "*" -r=myrepo --confirm
```

### GitLab CI 集成

```yaml
stages:
  - build
  - test
  - deploy

variables:
  CONAN_USER_HOME: "$CI_PROJECT_DIR/.conan"

conan-build:
  stage: build
  image: conanio/gcc11
  cache:
    key: conan-cache
    paths:
      - .conan/p
  before_script:
    - conan profile detect --force
    - conan remote add gitlab $CI_API_V4_URL/packages/conan
  script:
    - conan install . --build=missing
    - cmake --preset conan-release
    - cmake --build --preset conan-release
  artifacts:
    paths:
      - build/
```

### Jenkins Pipeline

```groovy
pipeline {
    agent any
    environment {
        CONAN_USER_HOME = "${WORKSPACE}/.conan"
    }
    stages {
        stage('Install') {
            steps {
                sh 'conan profile detect --force'
                sh 'conan install . --build=missing'
            }
        }
        stage('Build') {
            steps {
                sh 'cmake --preset conan-release'
                sh 'cmake --build --preset conan-release'
            }
        }
        stage('Upload') {
            when { branch 'main' }
            steps {
                sh 'conan create . --user=ci --channel=stable'
                sh 'conan upload "*" -r=artifactory --confirm'
            }
        }
    }
}
```

### 缓存优化策略

```bash
# 在CI中配置缓存路径
export CONAN_HOME=/cache/conan2

# 预下载常用包
conan install . --build=missing -s build_type=Release
conan install . --build=missing -s build_type=Debug

# 仅上传自建包，不上传第三方
conan upload "mylib/*" -r=artifactory --confirm
```

---

## 📖 参考资源

- [官方文档](https://docs.conan.io/)
- [ConanCenter](https://conan.io/center/)
- [迁移到Conan 2.0指南](https://docs.conan.io/1/conan2.html)

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
