# Conan C/C++包管理深度指南

> **层级定位**: 01 Core Knowledge System / 05 Engineering Layer
> **难度级别**: L3-L4
> **预估学习时间**: 6-8 小时

---

## 目录

- [Conan C/C++包管理深度指南](#conan-cc包管理深度指南)
  - [目录](#目录)
  - [1. Conan基础](#1-conan基础)
    - [1.1 为什么需要包管理](#11-为什么需要包管理)
    - [1.2 安装与配置](#12-安装与配置)
  - [2. 基本使用](#2-基本使用)
    - [2.1 conanfile.py](#21-conanfilepy)
    - [2.2 安装依赖](#22-安装依赖)
    - [2.3 与CMake集成](#23-与cmake集成)
  - [3. 高级配置](#3-高级配置)
    - [3.1 条件依赖](#31-条件依赖)
    - [3.2 自定义选项](#32-自定义选项)
    - [3.3 版本冲突解决](#33-版本冲突解决)
  - [4. 私有仓库](#4-私有仓库)
    - [4.1 Artifactory CE](#41-artifactory-ce)
    - [4.2 创建和上传包](#42-创建和上传包)
  - [5. CI/CD集成](#5-cicd集成)
    - [5.1 GitHub Actions](#51-github-actions)
    - [5.2 锁文件](#52-锁文件)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. Conan基础

### 1.1 为什么需要包管理

```
传统C/C++依赖管理痛点：
- 手动下载/编译依赖
- 不同平台配置复杂
- 版本冲突难解决
- CI/CD集成困难

Conan解决方案：
✓ 声明式依赖管理
✓ 二进制包管理（无需重复编译）
✓ 多配置支持（Debug/Release, 多编译器）
✓ 完全可复现构建
```

### 1.2 安装与配置

```bash
# 安装
pip install conan

# 版本确认
conan --version  # 2.x推荐

# 配置远程仓库
conan remote add conancenter https://center.conan.io
conan remote list

# 创建默认配置文件
conan profile detect --force

# 查看配置文件
cat ~/.conan2/profiles/default
```

---

## 2. 基本使用

### 2.1 conanfile.py

```python
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class MyProject(ConanFile):
    name = "myproject"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"

    # 依赖声明
    requires = [
        "fmt/10.1.1",
        "spdlog/1.12.0",
        "nlohmann_json/3.11.2",
    ]

    # 构建工具
    tool_requires = [
        "cmake/[>=3.25]",
    ]

    # 生成器
    generators = "CMakeDeps", "CMakeToolchain"

    exports_sources = "CMakeLists.txt", "src/*", "include/*"

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

### 2.2 安装依赖

```bash
# 安装依赖并生成构建文件
conan install . --output-folder=build --build=missing

# 选项说明
# --output-folder: 生成文件输出目录
# --build=missing: 如果二进制包不存在则构建
# --build=*:       强制重新构建所有
# -s build_type=Debug: 设置构建类型

# 完整示例
conan install . \
    --output-folder=build \
    --build=missing \
    -s build_type=Release \
    -s compiler.cppstd=20
```

### 2.3 与CMake集成

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.25)
project(MyProject CXX)

# C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Conan依赖
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# 可执行文件
add_executable(myapp src/main.cpp)

target_link_libraries(myapp PRIVATE
    fmt::fmt
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)
```

```bash
# 构建流程
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

---

## 3. 高级配置

### 3.1 条件依赖

```python
from conan import ConanFile

class MyProject(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "with_ssl": [True, False],
        "with_tests": [True, False],
    }
    default_options = {
        "shared": False,
        "with_ssl": True,
        "with_tests": False,
    }

    def requirements(self):
        self.requires("zlib/1.3")

        if self.options.with_ssl:
            self.requires("openssl/[>=1.1 <4]")

        if self.options.with_tests:
            self.requires("gtest/1.14.0")

    def configure(self):
        # 传递选项给依赖
        if self.options.shared:
            self.options["openssl"].shared = True
```

### 3.2 自定义选项

```python
from conan import ConanFile
from conan.tools.files import copy

class MyLibrary(ConanFile):
    name = "mylib"
    version = "1.0.0"

    options = {
        "log_level": ["debug", "info", "warn", "error"],
        "enable_feature_x": [True, False],
    }
    default_options = {
        "log_level": "info",
        "enable_feature_x": False,
    }

    def generate(self):
        # 生成配置文件
        tc = CMakeToolchain(self)
        tc.variables["MYLIB_LOG_LEVEL"] = self.options.log_level
        tc.variables["MYLIB_FEATURE_X"] = self.options.enable_feature_x
        tc.generate()
```

### 3.3 版本冲突解决

```python
# 覆盖依赖版本
class MyProject(ConanFile):
    requires = [
        "boost/1.83.0",  # 需要1.83
        "otherlib/1.0",  # 依赖boost/1.80.0
    ]

    def requirements(self):
        # 强制使用特定版本
        self.requires("boost/1.83.0", override=True)

        # 或者允许兼容版本
        self.requires("boost/[>=1.80.0 <1.90.0]")
```

---

## 4. 私有仓库

### 4.1 Artifactory CE

```bash
# 启动本地Artifactory (Docker)
docker run -d --name artifactory \
    -p 8081:8081 -p 8082:8082 \
    -v artifactory_data:/var/opt/jfrog/artifactory \
    releases-docker.jfrog.io/jfrog/artifactory-cpp-ce:latest

# 添加私有仓库
conan remote add myrepo http://localhost:8081/artifactory/api/conan/conan-local

# 登录
conan user -p mypassword -r myrepo myusername
```

### 4.2 创建和上传包

```python
# conanfile.py (库)
from conan import ConanFile
from conan.tools.cmake import CMake

class MyLib(ConanFile):
    name = "mylib"
    version = "1.2.3"
    ...

    def package_info(self):
        # 定义 consumers 如何使用
        self.cpp_info.libs = ["mylib"]
        self.cpp_info.includedirs = ["include"]

        if self.settings.os == "Linux":
            self.cpp_info.system_libs = ["pthread", "dl"]
```

```bash
# 创建包
conan create . --user=myteam --channel=stable

# 上传
conan upload mylib/1.2.3@myteam/stable -r myrepo --confirm

# 批量上传
conan upload "*" -r myrepo --confirm --all
```

---

## 5. CI/CD集成

### 5.1 GitHub Actions

```yaml
# .github/workflows/build.yml
name: Build with Conan

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Release, Debug]

    steps:
    - uses: actions/checkout@v3

    - name: Setup Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.11'

    - name: Install Conan
      run: |
        pip install conan
        conan profile detect --force

    - name: Configure Conan Remote
      run: |
        conan remote add myrepo ${{ secrets.CONAN_REPO_URL }}
        conan user -p ${{ secrets.CONAN_TOKEN }} -r myrepo ci-user

    - name: Install Dependencies
      run: |
        conan install . -s build_type=${{ matrix.build_type }} \
          --output-folder=build --build=missing

    - name: Build
      run: |
        cmake --preset conan-${{ matrix.build_type }}
        cmake --build --preset conan-${{ matrix.build_type }}

    - name: Test
      run: ctest --preset conan-${{ matrix.build_type }}

    - name: Create Package
      if: github.ref == 'refs/heads/main'
      run: conan create . --build=missing

    - name: Upload Package
      if: github.ref == 'refs/heads/main'
      run: conan upload "*" -r myrepo --confirm
```

### 5.2 锁文件

```bash
# 生成锁文件（锁定所有依赖版本）
conan lock create conanfile.py --lockfile-out=conan.lock

# 使用锁文件构建（确保可复现）
conan install . --lockfile=conan.lock

# 更新特定依赖
conan lock create conanfile.py --update=pkg1 --lockfile-out=conan.lock
```

---

## 关联导航

### 前置知识

- [构建系统](./02_Build_Systems/README.md)
- [CMake详解](../03_Modern_Toolchain/README.md)

### 后续延伸

- [CI/CD实践](../../../03_System_Technology_Domains/03_Embedded_Systems/06_CI_CD_for_Embedded.md)
- [依赖安全](../../../03_System_Technology_Domains/10_Security_Engineering/07_Dependency_Security.md)

### 参考

- Conan官方文档: <https://docs.conan.io/>
- ConanCenter: <https://conan.io/center/>
