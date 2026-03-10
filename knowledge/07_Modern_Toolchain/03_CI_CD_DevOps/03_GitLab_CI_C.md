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

build:
  stage: build
  image: gcc:13
  script:
    - mkdir build && cd build
    - cmake ..
    - make -j$(nproc)
  artifacts:
    paths:
      - build/

test:
  stage: test
  image: gcc:13
  dependencies:
    - build
  script:
    - cd build
    - ctest --output-on-failure
```

---

## 📚 高级配置

### 多平台构建

```yaml
.build_template: &build_def
  stage: build
  script:
    - mkdir build && cd build
    - cmake ..
    - cmake --build . --parallel
  artifacts:
    paths:
      - build/

build:linux:
  <<: *build_def
  image: gcc:13

build:windows:
  <<: *build_def
  image: mcr.microsoft.com/windows/servercore:ltsc2022
  tags:
    - windows
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
```

---

**← [返回工具链主页](../README.md)**
