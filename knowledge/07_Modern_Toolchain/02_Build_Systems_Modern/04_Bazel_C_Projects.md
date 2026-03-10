# Bazel企业级C项目构建

> **层级定位**: 07 Modern Toolchain / 02 Build Systems
> **难度级别**: L4 专家
> **预估学习时间**: 8-12小时

---

## 📋 Bazel简介

Bazel是Google开源的构建工具：

- **可扩展**: 支持超大规模代码库
- **快速**: 增量构建，远程缓存
- **可靠**: 沙盒构建，可重现
- **多语言**: C/C++, Java, Go等

### 适用场景

- 大型代码库
- 多语言项目
- 需要远程缓存/执行
- 企业级项目

---

## 🚀 快速开始

### 安装

```bash
# macOS
brew install bazel

# Linux
npm install -g @bazel/bazelisk

# Windows
choco install bazel
```

### 最小项目

**WORKSPACE** (项目根目录):

```text
workspace(name = "myproject")
```

**BUILD** (src目录):

```python
cc_binary(
    name = "myapp",
    srcs = ["main.c"],
)
```

**构建**:

```bash
bazel build //src:myapp
bazel run //src:myapp
```

---

## 📚 核心概念

### BUILD文件规则

```python
# 可执行文件
cc_binary(
    name = "app",
    srcs = ["main.c", "utils.c"],
    hdrs = ["utils.h"],
    copts = ["-Wall", "-O2"],
    linkopts = ["-lm"],
    deps = [":mylib"],
)

# 静态库
cc_library(
    name = "mylib",
    srcs = ["lib.c"],
    hdrs = ["lib.h"],
    visibility = ["//visibility:public"],
)

# 测试
cc_test(
    name = "lib_test",
    srcs = ["lib_test.c"],
    deps = [":mylib", "@unity//:unity"],
)
```

### 外部依赖

**WORKSPACE**:

```python
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "zlib",
    urls = ["https://zlib.net/zlib-1.2.13.tar.gz"],
    sha256 = "...",
    build_file = "//third_party:zlib.BUILD",
)
```

---

## 🔧 高级特性

### 远程缓存

```bash
# 启用远程缓存
bazel build --remote_cache=grpc://cache.example.com:9092 //...
```

### Bazel配置

**.bazelrc**:

```text
build --jobs=auto
build --copt=-Wall
build --copt=-Werror

test --test_output=errors
test --test_verbose_timeout_warnings
```

---

**← [返回工具链主页](../README.md)**
