# Docker C语言开发容器化指南


> **版本**: v1.0.0
> **最后更新**: 2026-03-25
> **作者**: C_Lang Team
> **难度**: L2 基础

---
> **层级定位**: 07 Modern Toolchain / 03 CI/CD DevOps
> **难度级别**: L3 应用
> **预估学习时间**: 4-6小时

---

## 📋 为什么容器化C开发

### 优势

| 优势 | 说明 |
|:-----|:-----|
| **环境一致** | 消除"在我机器上能跑" |
| **隔离性** | 不影响宿主机环境 |
| **可重现** | 精确的构建环境 |
| **便携性** | 跨平台一致体验 |
| **CI/CD友好** | 标准化构建环境 |

---


---

## 📑 目录

- [Docker C语言开发容器化指南](#docker-c语言开发容器化指南)
  - [📋 为什么容器化C开发](#-为什么容器化c开发)
    - [优势](#优势)
  - [📑 目录](#-目录)
  - [🚀 快速开始](#-快速开始)
    - [基础C开发Dockerfile](#基础c开发dockerfile)
  - [📚 多阶段构建](#-多阶段构建)
    - [优化镜像大小](#优化镜像大小)
  - [🔧 高级用法](#-高级用法)
    - [交叉编译容器](#交叉编译容器)
    - [开发容器 (Dev Container)](#开发容器-dev-container)
  - [🐳 Docker 多阶段构建深度解析](#-docker-多阶段构建深度解析)
    - [1. 多阶段构建优化策略](#1-多阶段构建优化策略)
    - [2. 交叉编译容器详解](#2-交叉编译容器详解)
    - [3. 多平台Docker构建](#3-多平台docker构建)
    - [4. 开发容器 (Dev Container) 高级配置](#4-开发容器-dev-container-高级配置)
    - [5. CI/CD中的Docker构建](#5-cicd中的docker构建)
    - [6. 调试与开发工具容器](#6-调试与开发工具容器)
    - [7. 安全加固容器](#7-安全加固容器)
  - [✅ 检查清单](#-检查清单)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 🚀 快速开始

### 基础C开发Dockerfile

**Dockerfile**:

```dockerfile
# 基础镜像

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

FROM gcc:13-bookworm

# 安装构建工具
RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    gdb \
    valgrind \
    && rm -rf /var/lib/apt/lists/*

# 工作目录
WORKDIR /workspace

# 默认命令
CMD ["/bin/bash"]
```

**构建和运行**:

```bash
# 构建镜像
docker build -t c-dev:latest .

# 运行容器
docker run -it \
    -v $(pwd):/workspace \
    c-dev:latest

# 在容器内构建
mkdir build && cd build
cmake ..
make
```

---

## 📚 多阶段构建

### 优化镜像大小

```dockerfile
# 构建阶段
FROM gcc:13 as builder

WORKDIR /build
COPY .

RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --parallel

# 运行阶段
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /build/build/myapp .

CMD ["./myapp"]
```

**构建**:

```bash
docker build -t myapp:latest .
docker run myapp:latest
```

---

## 🔧 高级用法

### 交叉编译容器

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    cmake \
    && rm -rf /var/lib/apt/lists/*

ENV CC=arm-linux-gnueabihf-gcc
ENV CXX=arm-linux-gnueabihf-g++

WORKDIR /workspace
```

### 开发容器 (Dev Container)

**.devcontainer/devcontainer.json**:

```json
{
    "name": "C Development",
    "image": "gcc:13-bookworm",
    "features": {
        "ghcr.io/devcontainers/features/cmake:1": {},
        "ghcr.io/devcontainers/features/github-cli:1": {}
    },
    "customizations": {
        "vscode": {
            "extensions": [
                "ms-vscode.cpptools",
                "ms-vscode.cmake-tools"
            ]
        }
    },
    "mounts": [
        "source=${localWorkspaceFolder},target=/workspace,type=bind"
    ],
    "workspaceFolder": "/workspace"
}
```

---

## 🐳 Docker 多阶段构建深度解析

### 1. 多阶段构建优化策略

**超精简生产镜像**:

```dockerfile
# =============================================================================
# 阶段1: 基础构建环境
# =============================================================================
FROM alpine:3.18 AS base-builder

RUN apk add --no-cache \
    gcc \
    musl-dev \
    cmake \
    ninja \
    git

WORKDIR /src

# =============================================================================
# 阶段2: 依赖缓存
# =============================================================================
FROM base-builder AS deps

# 先复制依赖配置，利用Docker缓存层
COPY CMakeLists.txt .
COPY cmake ./cmake

# 创建临时CMake项目来下载依赖
RUN mkdir -p deps_build && cd deps_build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --target fetch_deps 2>/dev/null || true

# =============================================================================
# 阶段3: 编译
# =============================================================================
FROM base-builder AS builder

# 从deps阶段复制缓存
COPY --from=deps /src/deps_build /src/deps_build

# 复制完整源码
COPY . .

# 构建
RUN cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_EXE_LINKER_FLAGS="-static -s" && \
    cmake --build build --parallel $(nproc)

# 剥离调试符号
RUN strip --strip-all build/myapp

# =============================================================================
# 阶段4: 最小运行时
# =============================================================================
FROM scratch AS production

# 从空镜像开始
COPY --from=builder /src/build/myapp /app/

# 非root用户运行（需要提前在builder阶段创建）
COPY --from=builder /etc/passwd /etc/passwd
USER 1000

EXPOSE 8080

ENTRYPOINT ["/app/myapp"]
```

**构建命令**:

```bash
# 构建生产镜像
docker build --target production -t myapp:prod .

# 构建开发镜像
docker build --target base-builder -t myapp:dev .

# 查看镜像大小对比
docker images | grep myapp
```

### 2. 交叉编译容器详解

**多架构交叉编译环境**:

```dockerfile
# Dockerfile.cross-compile
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# 安装多架构工具链
RUN dpkg --add-architecture armhf && \
    dpkg --add-architecture arm64 && \
    apt-get update && \
    apt-get install -y \
    # x86_64
    gcc g++ \
    # ARM 32-bit
    gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf \
    libc6-dev-armhf-cross \
    # ARM 64-bit
    gcc-aarch64-linux-gnu g++-aarch64-linux-gnu \
    libc6-dev-arm64-cross \
    # MIPS
    gcc-mips-linux-gnu g++-mips-linux-gnu \
    # 通用工具
    cmake ninja-build qemu-user-static \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# 创建架构配置脚本
RUN cat > /usr/local/bin/setup-cross-compile << 'EOF'
#!/bin/bash
ARCH=$1

case $ARCH in
    arm32|armhf)
        export CC=arm-linux-gnueabihf-gcc
        export CXX=arm-linux-gnueabihf-g++
        export AR=arm-linux-gnueabihf-ar
        export STRIP=arm-linux-gnueabihf-strip
        export CMAKE_SYSTEM_PROCESSOR=arm
        export CMAKE_SYSTEM_NAME=Linux
        ;;
    arm64|aarch64)
        export CC=aarch64-linux-gnu-gcc
        export CXX=aarch64-linux-gnu-g++
        export AR=aarch64-linux-gnu-ar
        export STRIP=aarch64-linux-gnu-strip
        export CMAKE_SYSTEM_PROCESSOR=aarch64
        export CMAKE_SYSTEM_NAME=Linux
        ;;
    mips)
        export CC=mips-linux-gnu-gcc
        export CXX=mips-linux-gnu-g++
        export CMAKE_SYSTEM_PROCESSOR=mips
        export CMAKE_SYSTEM_NAME=Linux
        ;;
    native|x64|*)
        # 使用默认gcc
        ;;
esac

export CROSS_COMPILE=${CC%-gcc}-
echo "Cross compile environment set for $ARCH"
echo "CC=$CC"
echo "CROSS_COMPILE=$CROSS_COMPILE"
EOF

RUN chmod +x /usr/local/bin/setup-cross-compile

WORKDIR /workspace
```

**交叉编译脚本**:

```bash
#!/bin/bash
# build-cross.sh

TARGET_ARCH=${1:-arm64}
BUILD_TYPE=${2:-Release}

docker run --rm \
    -v "$(pwd):/workspace" \
    -e ARCH="$TARGET_ARCH" \
    cross-compile-env:latest \
    bash -c "
        source /usr/local/bin/setup-cross-compile \$ARCH &&
        cmake -B build-\$ARCH -G Ninja \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DCMAKE_C_COMPILER=\$CC \
            -DCMAKE_CXX_COMPILER=\$CXX \
            -DCMAKE_SYSTEM_PROCESSOR=\$CMAKE_SYSTEM_PROCESSOR \
            -DCMAKE_SYSTEM_NAME=\$CMAKE_SYSTEM_NAME &&
        cmake --build build-\$ARCH --parallel
    "
```

**CMake交叉编译工具链文件**:

```dockerfile
# 在容器中自动生成工具链文件
RUN mkdir -p /opt/toolchains

# ARM 32-bit工具链文件
RUN cat > /opt/toolchains/armhf.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# ARM 64-bit工具链文件
RUN cat > /opt/toolchains/arm64.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF
```

### 3. 多平台Docker构建

**Buildx多平台构建**:

```dockerfile
# Dockerfile.multiplatform
FROM --platform=$BUILDPLATFORM gcc:13 AS builder

ARG TARGETPLATFORM
ARG BUILDPLATFORM
ARG TARGETARCH

WORKDIR /src
COPY . .

# 根据目标平台设置编译器标志
RUN case "$TARGETARCH" in \
    amd64) ARCH_FLAGS="-march=x86-64-v3" ;; \
    arm64) ARCH_FLAGS="-march=armv8-a+crc+crypto" ;; \
    arm) ARCH_FLAGS="-march=armv7-a -mfpu=neon" ;; \
    *) ARCH_FLAGS="" ;; \
    esac && \
    cmake -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_FLAGS="$ARCH_FLAGS" && \
    cmake --build build

FROM --platform=$TARGETPLATFORM debian:bookworm-slim

COPY --from=builder /src/build/myapp /usr/local/bin/
ENTRYPOINT ["/usr/local/bin/myapp"]
```

**构建脚本**:

```bash
#!/bin/bash
# build-multiplatform.sh

# 创建builder实例
docker buildx create --use --name multiplatform-builder || true

# 构建多平台镜像
docker buildx build \
    --platform linux/amd64,linux/arm64,linux/arm/v7 \
    -t myapp:latest \
    --push \
    -f Dockerfile.multiplatform .

# 或仅本地构建（单平台）
docker buildx build \
    --platform linux/amd64 \
    -t myapp:latest \
    --load \
    -f Dockerfile.multiplatform .
```

### 4. 开发容器 (Dev Container) 高级配置

**完整开发环境配置**:

```json
// .devcontainer/devcontainer.json
{
    "name": "C/C++ Development Environment",
    "image": "mcr.microsoft.com/devcontainers/cpp:1-debian-12",

    "features": {
        "ghcr.io/devcontainers/features/cmake:1": {
            "version": "3.28"
        },
        "ghcr.io/devcontainers/features/ninja:1": {},
        "ghcr.io/devcontainers/features/git:1": {},
        "ghcr.io/devcontainers/features/github-cli:1": {},
        "ghcr.io/devcontainers/features/docker-in-docker:2": {}
    },

    "customizations": {
        "vscode": {
            "extensions": [
                "ms-vscode.cpptools",
                "ms-vscode.cpptools-extension-pack",
                "ms-vscode.cmake-tools",
                "llvm-vs-code-extensions.vscode-clangd",
                "jeff-hykin.better-cpp-syntax",
                "vadimcn.vscode-lldb",
                "ms-vscode.hexeditor",
                "github.vscode-pull-request-github"
            ],
            "settings": {
                "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
                "C_Cpp.intelliSenseCacheSize": 5120,
                "C_Cpp.intelliSenseMemoryLimit": 4096,
                "cmake.configureOnOpen": true,
                "cmake.buildDirectory": "${workspaceFolder}/build",
                "cmake.generator": "Ninja",
                "cmake.parallelJobs": 0,
                "editor.formatOnSave": true,
                "[c]": {
                    "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd"
                },
                "files.watcherExclude": {
                    "**/build/**": true,
                    "**/.git/**": true
                }
            }
        }
    },

    "mounts": [
        "source=${localEnv:HOME}/.ssh,target=/home/vscode/.ssh,type=bind,consistency=cached",
        "source=${localEnv:HOME}/.gitconfig,target=/home/vscode/.gitconfig,type=bind,consistency=cached"
    ],

    "runArgs": [
        "--cap-add=SYS_PTRACE",
        "--security-opt", "seccomp=unconfined",
        "--network=host"
    ],

    "postCreateCommand": "git config --global --add safe.directory /workspaces/$(basename $PWD)",

    "remoteUser": "vscode"
}
```

**Docker Compose开发环境**:

```yaml
# .devcontainer/docker-compose.yml
version: '3.8'

services:
  dev:
    build:
      context: ..
      dockerfile: .devcontainer/Dockerfile
    volumes:
      - ..:/workspace:cached
      - build-cache:/workspace/build
      - ccache-data:/home/vscode/.ccache
    environment:
      - CCACHE_DIR=/home/vscode/.ccache
      - CCACHE_MAXSIZE=10G
    command: sleep infinity

  # 可选：本地数据库用于测试
  postgres:
    image: postgres:16
    environment:
      POSTGRES_USER: dev
      POSTGRES_PASSWORD: dev
      POSTGRES_DB: testdb
    volumes:
      - postgres-data:/var/lib/postgresql/data
    ports:
      - "5432:5432"

  # 可选：Redis缓存
  redis:
    image: redis:7-alpine
    ports:
      - "6379:6379"

volumes:
  build-cache:
  ccache-data:
  postgres-data:
```

### 5. CI/CD中的Docker构建

**GitHub Actions Docker构建**:

```yaml
# .github/workflows/docker-build.yml
name: Docker Build

on:
  push:
    branches: [main]
    tags: ['v*']
  pull_request:
    branches: [main]

env:
  REGISTRY: ghcr.io
  IMAGE_NAME: ${{ github.repository }}

jobs:
  build:
    runs-on: ubuntu-latest
    permissions:
      contents: read
      packages: write

    steps:
    - uses: actions/checkout@v4
      with:
        fetch-depth: 0

    - name: Set up QEMU
      uses: docker/setup-qemu-action@v3

    - name: Set up Docker Buildx
      uses: docker/setup-buildx-action@v3

    - name: Log in to Container Registry
      uses: docker/login-action@v3
      with:
        registry: ${{ env.REGISTRY }}
        username: ${{ github.actor }}
        password: ${{ secrets.GITHUB_TOKEN }}

    - name: Extract metadata
      id: meta
      uses: docker/metadata-action@v5
      with:
        images: ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}
        tags: |
          type=ref,event=branch
          type=ref,event=pr
          type=semver,pattern={{version}}
          type=semver,pattern={{major}}.{{minor}}
          type=semver,pattern={{major}}

    - name: Build and push
      uses: docker/build-push-action@v5
      with:
        context: .
        platforms: linux/amd64,linux/arm64
        push: ${{ github.event_name != 'pull_request' }}
        tags: ${{ steps.meta.outputs.tags }}
        labels: ${{ steps.meta.outputs.labels }}
        cache-from: type=gha
        cache-to: type=gha,mode=max
```

**GitLab CI Docker构建**:

```yaml
# .gitlab-ci.yml
variables:
  DOCKER_IMAGE: $CI_REGISTRY_IMAGE/myapp
  DOCKER_DRIVER: overlay2
  DOCKER_TLS_CERTDIR: ""

stages:
  - build
  - test
  - push

docker-build:
  stage: build
  image: docker:latest
  services:
    - docker:dind
  before_script:
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
  script:
    - docker build
        --build-arg BUILDKIT_INLINE_CACHE=1
        --cache-from $DOCKER_IMAGE:latest
        -t $DOCKER_IMAGE:$CI_COMMIT_SHA
        -t $DOCKER_IMAGE:latest
        .
    - docker push $DOCKER_IMAGE:$CI_COMMIT_SHA
  only:
    - main

docker-test:
  stage: test
  image: $DOCKER_IMAGE:$CI_COMMIT_SHA
  script:
    - cd /workspace && ctest --output-on-failure

docker-push-release:
  stage: push
  image: docker:latest
  services:
    - docker:dind
  before_script:
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
  script:
    - docker pull $DOCKER_IMAGE:$CI_COMMIT_SHA
    - docker tag $DOCKER_IMAGE:$CI_COMMIT_SHA $DOCKER_IMAGE:$CI_COMMIT_TAG
    - docker push $DOCKER_IMAGE:$CI_COMMIT_TAG
  only:
    - tags
```

### 6. 调试与开发工具容器

**带调试工具的开发容器**:

```dockerfile
FROM gcc:13-bookworm

# 安装开发工具
RUN apt-get update && apt-get install -y \
    # 构建工具
    cmake ninja-build meson \
    # 调试工具
    gdb lldb valgrind strace ltrace \
    # 静态分析
    clang-format clang-tidy cppcheck \
    # 代码覆盖
    lcov gcovr \
    # 性能分析
    linux-perf gprof \
    # 网络工具
    curl wget netcat-openbsd iputils-ping \
    # 编辑器
    vim nano \
    # 其他
    git tig htop tree \
    && rm -rf /var/lib/apt/lists/*

# 安装clang
RUN apt-get update && apt-get install -y \
    clang-17 llvm-17 \
    && rm -rf /var/lib/apt/lists/*

# 配置gdb
RUN cat > /root/.gdbinit << 'EOF'
set history save on
set history filename ~/.gdb_history
set history size 10000
set history remove-duplicates unlimited
set pagination off
set confirm off
EOF

# 配置valgrind抑制文件
RUN cat > /root/.valgrindrc << 'EOF'
--suppressions=/usr/lib/x86_64-linux-gnu/valgrind/default.supp
--leak-check=full
--show-leak-kinds=all
--track-origins=yes
EOF

# 安装常用脚本
RUN cat > /usr/local/bin/c-debug << 'EOF'
#!/bin/bash
# C调试助手脚本

case $1 in
    memcheck)
        shift
        valgrind --tool=memcheck --leak-check=full "$@"
        ;;
    cachegrind)
        shift
        valgrind --tool=cachegrind "$@"
        ;;
    callgrind)
        shift
        valgrind --tool=callgrind "$@"
        ;;
    profile)
        shift
        perf record -g "$@"
        perf report
        ;;
    coverage)
        shift
        gcovr -r . --html --html-details -o coverage.html
        ;;
    *)
        echo "Usage: c-debug {memcheck|cachegrind|callgrind|profile|coverage} <command>"
        ;;
esac
EOF

RUN chmod +x /usr/local/bin/c-debug

WORKDIR /workspace
```

### 7. 安全加固容器

**Distroless风格最小化容器**:

```dockerfile
# 使用自定义基础镜像构建静态链接二进制
FROM alpine:3.18 AS static-builder

RUN apk add --no-cache gcc musl-dev cmake ninja

WORKDIR /src
COPY . .

# 完全静态链接
RUN cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXE_LINKER_FLAGS="-static -s" && \
    cmake --build build

# 使用空镜像
FROM scratch

# 从builder复制静态二进制
COPY --from=static-builder /src/build/myapp /app/

# 如果没有TLS需求，甚至可以不需要CA证书
# COPY --from=static-builder /etc/ssl/certs/ca-certificates.crt /etc/ssl/certs/

# 创建非root用户（使用UID而不是用户名）
USER 65534:65534

EXPOSE 8080

ENTRYPOINT ["/app/myapp"]
```

**安全扫描**:

```dockerfile
# 在构建流程中添加安全扫描阶段
FROM aquasec/trivy:latest AS security-scan

COPY --from=production /app/myapp /scan/myapp

# 扫描二进制漏洞
RUN trivy fs --exit-code 1 --severity HIGH,CRITICAL /scan

# 扫描镜像
FROM production AS final
COPY --from=security-scan /scan/pass /tmp/
```

---

## ✅ 检查清单

- [ ] Dockerfile编写正确
- [ ] 镜像可以成功构建
- [ ] 容器可以运行
- [ ] 可以编译C程序
- [ ] 多阶段构建优化
- [ ] 交叉编译容器配置
- [ ] Dev Container设置
- [ ] CI/CD集成
- [ ] 安全扫描配置
- [ ] 镜像大小优化

---

**← [返回工具链主页](../readme.md)**


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
