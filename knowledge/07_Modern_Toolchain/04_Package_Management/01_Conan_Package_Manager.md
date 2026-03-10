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

## 📚 高级配置

### conanfile.py

```python
from conan import ConanFile
from conan.tools.cmake import cmake_layout

class MyProject(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("zlib/1.2.13")
        self.requires("openssl/3.0.8")

    def layout(self):
        cmake_layout(self)
```

### 私有仓库

```bash
# 添加私有仓库
conan remote add myrepo https://myserver.com/conan

# 上传包
conan create . --user=myuser --channel=stable
conan upload mylib/1.0.0@myuser/stable -r=myrepo
```

---

**← [返回工具链主页](../README.md)**
