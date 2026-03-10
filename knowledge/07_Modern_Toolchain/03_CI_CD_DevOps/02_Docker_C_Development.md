# Docker C语言开发容器化指南

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

## 🚀 快速开始

### 基础C开发Dockerfile

**Dockerfile**:

```dockerfile
# 基础镜像
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
COPY . .

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

## ✅ 检查清单

- [ ] Dockerfile编写正确
- [ ] 镜像可以成功构建
- [ ] 容器可以运行
- [ ] 可以编译C程序
- [ ] 多阶段构建优化
- [ ] CI/CD集成

---

**← [返回工具链主页](../README.md)**
