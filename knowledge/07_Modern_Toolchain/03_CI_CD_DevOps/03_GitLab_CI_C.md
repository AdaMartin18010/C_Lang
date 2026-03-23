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
