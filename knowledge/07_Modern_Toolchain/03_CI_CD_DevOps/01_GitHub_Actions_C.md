
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

# GitHub Actions C项目CI/CD指南

> **层级定位**: 07 Modern Toolchain / 03 CI/CD DevOps
> **难度级别**: L3 应用
> **预估学习时间**: 4-6小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | GitHub Actions、工作流、矩阵构建、自动化测试 |
| **前置知识** | Git基础、YAML语法 |
| **后续延伸** | DevSecOps、高级部署策略 |
| **权威来源** | GitHub Actions官方文档 |

---

## 🎯 为什么需要CI/CD

### 传统开发流程的问题

```
开发 → 本地测试通过 → 提交 → 他人拉取 → 编译失败 → 回滚 → 混乱
   ↑___________________________________________________________|
```

### CI/CD解决的问题

```
开发 → 提交 → 自动构建测试 → 自动检查 → 自动部署 → 发布
   ↑_________________________________________________________|
                      ↓
               失败则阻止合并，保护主分支
```

### C语言项目的特殊需求

1. **多平台编译**: Linux/macOS/Windows
2. **多编译器测试**: GCC/Clang/MSVC
3. **内存安全检查**: Valgrind/Sanitizers
4. **代码质量**: 静态分析、格式化检查

---

## 🚀 快速开始

### 基础工作流模板

创建文件: `.github/workflows/ci.yml`

```yaml
name: C CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-linux:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake gcc clang

    - name: Configure
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release

    - name: Build
      run: cmake --build build --parallel

    - name: Test
      run: ctest --test-dir build --output-on-failure
```

---

## 📚 完整工作流示例

### 1. 多平台矩阵构建

```yaml
name: Multi-Platform Build

on: [push, pull_request]

jobs:
  build:
    strategy:
      fail-fast: false
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        compiler: [gcc, clang]
        exclude:
          - os: windows-latest
            compiler: clang
        include:
          - os: ubuntu-latest
            compiler: gcc
            cc: gcc
            cxx: g++
          - os: ubuntu-latest
            compiler: clang
            cc: clang
            cxx: clang++
          - os: macos-latest
            compiler: clang
            cc: clang
            cxx: clang++

    runs-on: ${{ matrix.os }}

    env:
      CC: ${{ matrix.cc }}
      CXX: ${{ matrix.cxx }}

    steps:
    - uses: actions/checkout@v4

    - name: Setup (Ubuntu)
      if: matrix.os == 'ubuntu-latest'
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake ninja-build

    - name: Setup (macOS)
      if: matrix.os == 'macos-latest'
      run: brew install cmake ninja

    - name: Setup (Windows)
      if: matrix.os == 'windows-latest'
      run: choco install cmake ninja

    - name: Configure
      run: cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

    - name: Build
      run: cmake --build build

    - name: Test
      run: ctest --test-dir build
```

### 2. 内存安全检查工作流

```yaml
name: Memory Safety Check

on: [push, pull_request]

jobs:
  sanitizers:
    runs-on: ubuntu-latest

    strategy:
      matrix:
        sanitizer: [address, undefined, thread]

    steps:
    - uses: actions/checkout@v4

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake clang

    - name: Configure with Sanitizer
      run: |
        cmake -B build \
          -DCMAKE_BUILD_TYPE=Debug \
          -DCMAKE_C_COMPILER=clang \
          -DCMAKE_CXX_FLAGS="-fsanitize=${{ matrix.sanitizer }} -fno-omit-frame-pointer -g" \
          -DCMAKE_LINKER_FLAGS="-fsanitize=${{ matrix.sanitizer }}"

    - name: Build
      run: cmake --build build

    - name: Test with Sanitizer
      run: ctest --test-dir build --output-on-failure
      env:
        ASAN_OPTIONS: detect_leaks=1

  valgrind:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Install Valgrind
      run: sudo apt-get install -y valgrind cmake

    - name: Configure
      run: cmake -B build -DCMAKE_BUILD_TYPE=Debug

    - name: Build
      run: cmake --build build

    - name: Run Valgrind
      run: |
        valgrind --leak-check=full --error-exitcode=1 \
          ./build/your_test_executable
```

### 3. 代码质量检查

```yaml
name: Code Quality

on: [push, pull_request]

jobs:
  formatting:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Install clang-format
      run: sudo apt-get install -y clang-format

    - name: Check formatting
      run: |
        find . -name '*.c' -o -name '*.h' | xargs clang-format -i
        git diff --exit-code

  static-analysis:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Install tools
      run: |
        sudo apt-get update
        sudo apt-get install -y cppcheck clang-tools cmake

    - name: Run cppcheck
      run: |
        cppcheck --enable=all --error-exitcode=1 \
          --suppress=missingIncludeSystem \
          -I include src/

    - name: Run Clang Static Analyzer
      run: |
        cmake -B build \
          -DCMAKE_C_COMPILER=clang \
          -DCMAKE_CXX_COMPILER=clang++
        scan-build cmake --build build

  code-coverage:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake gcovr lcov

    - name: Configure with coverage
      run: |
        cmake -B build \
          -DCMAKE_BUILD_TYPE=Debug \
          -DCMAKE_C_FLAGS="--coverage" \
          -DCMAKE_CXX_FLAGS="--coverage"

    - name: Build
      run: cmake --build build

    - name: Test
      run: ctest --test-dir build

    - name: Generate coverage report
      run: |
        lcov --capture --directory build --output-file coverage.info
        lcov --remove coverage.info '/usr/*' --output-file coverage.info

    - name: Upload coverage
      uses: codecov/codecov-action@v3
      with:
        files: ./coverage.info
```

### 4. 发布自动化

```yaml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  build-release:
    strategy:
      matrix:
        include:
          - os: ubuntu-latest
            asset_name: linux-amd64
          - os: macos-latest
            asset_name: macos-amd64
          - os: windows-latest
            asset_name: windows-amd64

    runs-on: ${{ matrix.os }}

    steps:
    - uses: actions/checkout@v4

    - name: Build
      run: |
        cmake -B build -DCMAKE_BUILD_TYPE=Release
        cmake --build build --config Release

    - name: Package (Unix)
      if: matrix.os != 'windows-latest'
      run: |
        tar czvf myapp-${{ matrix.asset_name }}.tar.gz -C build myapp

    - name: Package (Windows)
      if: matrix.os == 'windows-latest'
      run: |
        Compress-Archive -Path build/Release/myapp.exe -DestinationPath myapp-${{ matrix.asset_name }}.zip

    - name: Upload Release
      uses: softprops/action-gh-release@v1
      with:
        files: |
          myapp-*
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

---

## 🔧 高级技巧

### 缓存加速

```yaml
- name: Cache CMake build
  uses: actions/cache@v3
  with:
    path: build
    key: ${{ runner.os }}-cmake-${{ hashFiles('**/CMakeLists.txt') }}
    restore-keys: |
      ${{ runner.os }}-cmake-

- name: Cache vcpkg
  uses: actions/cache@v3
  with:
    path: vcpkg
    key: ${{ runner.os }}-vcpkg-${{ hashFiles('vcpkg.json') }}
```

### Docker构建

```yaml
build-docker:
  runs-on: ubuntu-latest

  steps:
  - uses: actions/checkout@v4

  - name: Build Docker image
    run: docker build -t myapp:${{ github.sha }} .

  - name: Run tests in container
    run: docker run myapp:${{ github.sha }} ./run_tests
```

### 条件执行

```yaml
- name: Deploy to staging
  if: github.ref == 'refs/heads/develop'
  run: ./deploy.sh staging

- name: Deploy to production
  if: github.ref == 'refs/heads/main'
  run: ./deploy.sh production
```

---

## 📁 模板库

查看 [05_CI_CD_Templates](./05_CI_CD_Templates/README.md) 目录获取即用模板:

- `basic-ci.yml` - 基础CI模板
- `multi-platform.yml` - 多平台构建
- `memory-safety.yml` - 内存安全检查
- `code-quality.yml` - 代码质量检查
- `release.yml` - 发布自动化

---

## ✅ 质量检查清单

- [ ] 工作流在提交时触发
- [ ] 多平台构建通过
- [ ] 测试自动运行
- [ ] 代码格式化检查
- [ ] 静态分析集成
- [ ] 安全检查(Sanitizers)
- [ ] 覆盖率报告
- [ ] 发布自动化

---

## 🔗 相关资源

- [GitHub Actions官方文档](https://docs.github.com/en/actions)
- [GitHub Actions C/C++模板](https://github.com/actions/starter-workflows/tree/main/ci)
- [Docker容器化指南](./02_Docker_C_Development.md)

---

**← [返回工具链主页](../README.md)**
**← [返回CI/CD目录](./README.md)**


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
