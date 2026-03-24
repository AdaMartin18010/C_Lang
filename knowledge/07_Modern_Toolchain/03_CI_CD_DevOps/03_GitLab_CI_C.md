
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

# GitLab CI/CD C项目配置

> **层级定位**: 07 Modern Toolchain / 03 CI/CD DevOps
> **难度级别**: L3 应用
> **预估学习时间**: 3-4小时

---

## 📋 GitLab CI基础

GitLab CI使用`.gitlab-ci.yml`配置工作流。

---

## 🚀 快速开始

### 基础配置

**.gitlab-ci.yml**:

```yaml
stages:
  - build
  - test
  - deploy

variables:
  CC: gcc
  CXX: g++
  GIT_DEPTH: 10

build:
  stage: build
  image: gcc:13
  script:
    - mkdir build && cd build
    - cmake -DCMAKE_BUILD_TYPE=Release ..
    - cmake --build . --parallel $(nproc)
  artifacts:
    paths:
      - build/
    expire_in: 1 hour

test:
  stage: test
  image: gcc:13
  dependencies:
    - build
  script:
    - cd build
    - ctest --output-on-failure --parallel $(nproc)
  coverage: '/Total Coverage: \d+\.\d+%/'

deploy:
  stage: deploy
  image: alpine:latest
  dependencies:
    - build
  script:
    - echo "Deploying..."
  only:
    - main
```

---

## 📚 高级配置

### 多平台构建

```yaml
.build_template: &build_def
  stage: build
  script:
    - mkdir build && cd build
    - cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
    - cmake --build . --parallel
  artifacts:
    paths:
      - build/

build:linux:
  <<: *build_def
  image: gcc:13
  variables:
    BUILD_TYPE: Release

build:windows:
  <<: *build_def
  image: mcr.microsoft.com/windows/servercore:ltsc2022
  tags:
    - windows

build:macos:
  <<: *build_def
  tags:
    - macos
```

### 矩阵构建

```yaml
build:
  parallel:
    matrix:
      - CC: [gcc, clang]
        BUILD_TYPE: [Debug, Release]
  script:
    - mkdir build && cd build
    - cmake -DCMAKE_C_COMPILER=$CC -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
    - cmake --build .
```

### Docker构建

```yaml
docker-build:
  stage: build
  image: docker:latest
  services:
    - docker:dind
  script:
    - docker build -t $CI_REGISTRY_IMAGE:$CI_COMMIT_SHA .
    - docker push $CI_REGISTRY_IMAGE:$CI_COMMIT_SHA
  only:
    - main
```

---

## 🔧 缓存与优化

### 依赖缓存

```yaml
variables:
  CONAN_USER_HOME: "${CI_PROJECT_DIR}/.conan"

cache:
  key: ${CI_COMMIT_REF_SLUG}
  paths:
    - .conan/data
    - build/_deps

build:
  script:
    - conan install . --build=missing
    - cmake -B build
    - cmake --build build
```

### 增量构建

```yaml
build:
  cache:
    key: build-cache
    paths:
      - build/CMakeFiles
  script:
    - cmake -B build
    - cmake --build build
```

---

## 📊 覆盖率报告

```yaml
test:
  stage: test
  script:
    - cmake -B build -DENABLE_COVERAGE=ON
    - cmake --build build
    - ctest --test-dir build
    - lcov --capture --directory build --output-file coverage.info
    - lcov --remove coverage.info '/usr/*' --output-file coverage.info
  coverage: '/lines......: \d+\.\d+%/'
  artifacts:
    reports:
      coverage_report:
        coverage_format: cobertura
        path: coverage.xml
```

---

## ✅ GitLab CI检查清单

- [ ] 配置runner
- [ ] 编写.gitlab-ci.yml
- [ ] 设置缓存
- [ ] 配置多平台构建
- [ ] 集成测试
- [ ] 配置覆盖率
- [ ] 设置部署

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
