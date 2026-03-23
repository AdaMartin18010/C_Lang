# CI/CD与DevOps (Continuous Integration & DevOps)

## 目录

- [CI/CD与DevOps (Continuous Integration \& DevOps)](#cicd与devops-continuous-integration--devops)
  - [目录](#目录)
  - [概述](#概述)
    - [CI/CD流程概览](#cicd流程概览)
  - [GitHub Actions](#github-actions)
    - [基础配置](#基础配置)
    - [高级配置示例](#高级配置示例)
    - [GitHub Actions核心概念](#github-actions核心概念)
  - [GitLab CI](#gitlab-ci)
    - [基础配置](#基础配置-1)
    - [GitLab CI高级特性](#gitlab-ci高级特性)
  - [Jenkins](#jenkins)
    - [Jenkinsfile 声明式流水线](#jenkinsfile-声明式流水线)
    - [Jenkins 脚本式流水线](#jenkins-脚本式流水线)
  - [自动化测试](#自动化测试)
    - [C语言测试框架对比](#c语言测试框架对比)
    - [Unity测试框架示例](#unity测试框架示例)
    - [内存测试集成](#内存测试集成)
    - [集成测试框架](#集成测试框架)
  - [代码质量门禁](#代码质量门禁)
    - [质量指标阈值设置](#质量指标阈值设置)
    - [质量门禁集成](#质量门禁集成)
    - [代码审查清单](#代码审查清单)
  - [C项目CI/CD最佳实践](#c项目cicd最佳实践)
    - [项目结构](#项目结构)
    - [完整的CMake配置](#完整的cmake配置)
    - [Docker构建环境](#docker构建环境)
    - [发布流程](#发布流程)
  - [参考资源](#参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 概述

CI/CD（持续集成/持续部署）是现代软件开发的核心实践，通过自动化构建、测试和部署流程，提高代码质量、加快交付速度。
对于C语言项目，CI/CD需要处理编译器差异、跨平台构建、内存检测等特殊挑战。

### CI/CD流程概览

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                         CI/CD完整流程图                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   开发者提交代码                                                            │
│        │                                                                    │
│        ▼                                                                    │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │                        持续集成 (CI)                                │   │
│   │  ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐          │   │
│   │  │ 拉取代码 │───→│ 编译构建 │───→│ 静态分析 │───→│ 单元测试 │          │   │
│   │  └─────────┘    └─────────┘    └─────────┘    └────┬────┘          │   │
│   │                                                    │                │   │
│   │                    ┌───────────────────────────────┘                │   │
│   │                    ▼                                                │   │
│   │               ┌─────────┐    ┌─────────┐                            │   │
│   │               │ 集成测试 │───→│ 内存检测 │                            │   │
│   │               └────┬────┘    └─────────┘                            │   │
│   └────────────────────┼────────────────────────────────────────────────┘   │
│                        │                                                    │
│               ┌────────┴────────┐                                           │
│               │                 │                                           │
│              通过               失败                                         │
│               │                 │                                           │
│               ▼                 ▼                                           │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │                       持续部署 (CD)                                 │   │
│   │  ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐          │   │
│   │  │ 打包发布 │───→│ 部署测试 │───→│ 生产部署 │───→│ 监控回滚 │          │   │
│   │  └─────────┘    └─────────┘    └─────────┘    └─────────┘          │   │
│   └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## GitHub Actions

### 基础配置

```yaml
# .github/workflows/build.yml
name: C Project CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

env:
  BUILD_TYPE: Release

jobs:
  build:
    runs-on: ${{ matrix.os }}

    strategy:
      fail-fast: false
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        compiler: [gcc, clang]
        exclude:
          - os: windows-latest
            compiler: clang

    steps:
    - name: Checkout code
      uses: actions/checkout@v4
      with:
        submodules: recursive

    - name: Configure CMake
      run: >
        cmake -B build
        -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}}
        -DCMAKE_C_COMPILER=${{matrix.compiler}}

    - name: Build
      run: cmake --build build --config ${{env.BUILD_TYPE}}

    - name: Test
      working-directory: build
      run: ctest -C ${{env.BUILD_TYPE}} --output-on-failure
```

### 高级配置示例

```yaml
# .github/workflows/ci-cd.yml
name: Complete CI/CD Pipeline

on:
  push:
    branches: [ '**' ]
    tags: [ 'v*' ]
  pull_request:
    branches: [ main ]

jobs:
  code-quality:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4

    - name: Install tools
      run: |
        sudo apt-get update
        sudo apt-get install -y cppcheck clang-format

    - name: Format check
      run: |
        find . -name "*.c" -o -name "*.h" | xargs clang-format -i
        git diff --exit-code

    - name: Static analysis with cppcheck
      run: |
        cppcheck --enable=all --error-exitcode=1 \
                 --suppress=missingIncludeSystem \
                 -I include src/

  build-matrix:
    needs: code-quality
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        include:
          - os: ubuntu-22.04
            compiler: gcc-12
            cc: gcc-12
          - os: ubuntu-22.04
            compiler: clang-14
            cc: clang-14
          - os: macos-13
            cc: clang

    steps:
    - uses: actions/checkout@v4

    - name: Build
      run: |
        export CC=${{ matrix.cc }}
        cmake -B build -DCMAKE_BUILD_TYPE=Release
        cmake --build build

  memory-check:
    needs: build-matrix
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4

    - name: Install Valgrind
      run: sudo apt-get install -y valgrind

    - name: Build with debug info
      run: |
        cmake -B build -DCMAKE_BUILD_TYPE=Debug
        cmake --build build

    - name: Run Valgrind
      run: |
        valgrind --leak-check=full --error-exitcode=1 \
                 ./build/myapp test
```

### GitHub Actions核心概念

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      GitHub Actions 架构图                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Workflow (工作流)                                                         │
│   ═════════════════                                                         │
│   定义在 .github/workflows/*.yml                                            │
│   由事件触发 (push, pull_request, schedule, manual)                         │
│                                                                             │
│        │                                                                    │
│        ▼                                                                    │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │  Job (任务) 1 - Build                                               │   │
│   │  runs-on: ubuntu-latest                                             │   │
│   │  ┌─────────┐    ┌─────────┐    ┌─────────┐                         │   │
│   │  │ Step 1  │───→│ Step 2  │───→│ Step 3  │                         │   │
│   │  │Checkout │    │ Setup   │    │ Build   │                         │   │
│   │  └─────────┘    └─────────┘    └─────────┘                         │   │
│   └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │  Job (任务) 2 - Test                                                │   │
│   │  needs: build                                                       │   │
│   │  ┌─────────┐    ┌─────────┐                                        │   │
│   │  │ Step 1  │───→│ Step 2  │                                        │   │
│   │  │Download │    │ Run     │                                        │   │
│   │  │Artifact │    │ Tests   │                                        │   │
│   │  └─────────┘    └─────────┘                                        │   │
│   └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│   关键特性:                                                                 │
│   • Matrix builds - 多平台/多编译器并行构建                                 │
│   • Artifacts - 构建产物传递                                                │
│   • Secrets - 敏感信息管理                                                  │
│   • Caching - 依赖缓存加速                                                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## GitLab CI

### 基础配置

```yaml
# .gitlab-ci.yml
stages:
  - build
  - test
  - deploy

variables:
  GIT_DEPTH: 10
  BUILD_DIR: build

# 缓存配置
cache:
  key: ${CI_COMMIT_REF_SLUG}
  paths:
    - ${BUILD_DIR}/CMakeFiles/

# 构建阶段
build:linux:
  stage: build
  image: gcc:12
  script:
    - apt-get update && apt-get install -y cmake ninja-build
    - cmake -B ${BUILD_DIR} -G Ninja -DCMAKE_BUILD_TYPE=Release
    - cmake --build ${BUILD_DIR}
  artifacts:
    paths:
      - ${BUILD_DIR}/myapp
    expire_in: 1 week

build:windows:
  stage: build
  tags:
    - windows
  script:
    - cmake -B ${BUILD_DIR} -G "Visual Studio 17 2022"
    - cmake --build ${BUILD_DIR} --config Release
  artifacts:
    paths:
      - ${BUILD_DIR}/Release/*.exe

# 测试阶段
test:unit:
  stage: test
  image: gcc:12
  needs:
    - job: build:linux
      artifacts: true
  script:
    - cd ${BUILD_DIR} && ctest --output-on-failure

test:valgrind:
  stage: test
  image: valgrind:latest
  needs:
    - job: build:linux
      artifacts: true
  script:
    - apt-get update && apt-get install -y valgrind
    - valgrind --leak-check=full --error-exitcode=1 ./${BUILD_DIR}/myapp test
  allow_failure: true

# 代码质量检查
code_quality:
  stage: test
  image: python:3.11
  script:
    - pip install lizard cppcheck-junit
    - lizard src/ -o complexity_report.xml
    - cppcheck --xml --enable=all src/ 2> cppcheck.xml
  artifacts:
    reports:
      junit: complexity_report.xml
    paths:
      - cppcheck.xml

# 部署阶段
deploy:staging:
  stage: deploy
  environment:
    name: staging
    url: https://staging.example.com
  script:
    - echo "Deploying to staging"
    - scp ${BUILD_DIR}/myapp user@staging-server:/opt/app/
  only:
    - develop

deploy:production:
  stage: deploy
  environment:
    name: production
    url: https://example.com
  script:
    - echo "Deploying to production"
    - scp ${BUILD_DIR}/myapp user@prod-server:/opt/app/
  only:
    - tags
  when: manual
```

### GitLab CI高级特性

```yaml
# .gitlab-ci.yml - 高级配置

# 引入模板
include:
  - template: Security/SAST.gitlab-ci.yml
  - template: Code-Quality.gitlab-ci.yml

# 工作流规则
workflow:
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH
    - if: $CI_COMMIT_TAG

# 并行矩阵构建
build:matrix:
  stage: build
  parallel:
    matrix:
      - PROVIDER: [aws, gcp]
        ARCH: [amd64, arm64]
        COMPILER: [gcc, clang]
  script:
    - echo "Building for ${PROVIDER}-${ARCH} with ${COMPILER}"
    - cmake -B build -DCMAKE_C_COMPILER=${COMPILER}
    - cmake --build build
  artifacts:
    name: "${CI_JOB_NAME}-${CI_COMMIT_REF_NAME}"
    paths:
      - build/myapp

# 动态子流水线
trigger-child:
  stage: deploy
  trigger:
    include:
      - local: path/to/child-pipeline.yml
    strategy: depend

# 父子流水线结构
# parent-pipeline.yml
stages:
  - prepare
  - trigger

generate-config:
  stage: prepare
  script:
    - ./generate-ci-config.sh > generated-config.yml
  artifacts:
    paths:
      - generated-config.yml

child-pipeline:
  stage: trigger
  trigger:
    include:
      - artifact: generated-config.yml
        job: generate-config
```

---

## Jenkins

### Jenkinsfile 声明式流水线

```groovy
// Jenkinsfile
pipeline {
    agent none

    environment {
        BUILD_DIR = 'build'
        CCACHE_DIR = '/var/cache/ccache'
    }

    options {
        buildDiscarder(logRotator(numToKeepStr: '10'))
        disableConcurrentBuilds()
        timeout(time: 30, unit: 'MINUTES')
    }

    stages {
        stage('Checkout') {
            agent any
            steps {
                checkout scm
                sh 'git submodule update --init --recursive'
            }
        }

        stage('Parallel Build') {
            parallel {
                stage('Build Linux GCC') {
                    agent {
                        docker {
                            image 'gcc:12'
                            args '-v ${CCACHE_DIR}:${CCACHE_DIR}'
                        }
                    }
                    steps {
                        sh '''
                            apt-get update && apt-get install -y cmake ccache
                            export PATH="/usr/lib/ccache:$PATH"
                            cmake -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=Release
                            cmake --build ${BUILD_DIR} -j$(nproc)
                        '''
                        stash includes: 'build/**', name: 'linux-build'
                    }
                }

                stage('Build Linux Clang') {
                    agent {
                        docker {
                            image 'silkeh/clang:15'
                        }
                    }
                    steps {
                        sh '''
                            apt-get update && apt-get install -y cmake
                            CC=clang CXX=clang++ cmake -B ${BUILD_DIR}
                            cmake --build ${BUILD_DIR} -j$(nproc)
                        '''
                    }
                }

                stage('Build Windows') {
                    agent {
                        label 'windows'
                    }
                    steps {
                        bat '''
                            cmake -B %BUILD_DIR% -G "Visual Studio 17 2022"
                            cmake --build %BUILD_DIR% --config Release
                        '''
                    }
                }
            }
        }

        stage('Test') {
            agent {
                docker {
                    image 'gcc:12'
                }
            }
            steps {
                unstash 'linux-build'
                sh '''
                    cd ${BUILD_DIR} && ctest --output-on-failure
                '''
            }
        }

        stage('Static Analysis') {
            agent {
                docker {
                    image 'cppcheck:latest'
                }
            }
            steps {
                sh '''
                    cppcheck --enable=all --xml --xml-version=2 \
                             --suppress=missingIncludeSystem \
                             -I include src/ 2> cppcheck.xml || true
                '''
                recordIssues(
                    tools: [cppCheck(pattern: 'cppcheck.xml')]
                )
            }
        }

        stage('Memory Check') {
            agent {
                docker {
                    image 'valgrind:latest'
                }
            }
            steps {
                unstash 'linux-build'
                sh '''
                    apt-get update && apt-get install -y libc6-dbg
                    valgrind --leak-check=full --xml=yes --xml-file=valgrind.xml \
                             ./${BUILD_DIR}/myapp test || true
                '''
                publishValgrind(
                    pattern: 'valgrind.xml'
                )
            }
        }

        stage('Deploy') {
            when {
                branch 'main'
            }
            agent any
            steps {
                unstash 'linux-build'
                sh '''
                    echo "Deploying to production"
                    scp ${BUILD_DIR}/myapp deploy@server:/opt/app/
                    ssh deploy@server 'sudo systemctl restart myapp'
                '''
            }
        }
    }

    post {
        always {
            cleanWs()
        }
        success {
            slackSend(color: 'good', message: "Build succeeded: ${env.JOB_NAME}")
        }
        failure {
            slackSend(color: 'danger', message: "Build failed: ${env.JOB_NAME}")
            mail to: 'team@example.com',
                 subject: "Build failed: ${env.JOB_NAME}",
                 body: "Check console output at ${env.BUILD_URL}"
        }
    }
}
```

### Jenkins 脚本式流水线

```groovy
// Jenkinsfile (Scripted)
node('linux && docker') {
    def image = null

    try {
        stage('Prepare') {
            checkout scm
            image = docker.build("c-builder:${env.BUILD_ID}", "-f Dockerfile.build .")
        }

        image.inside("-v ${env.CCACHE_DIR}:/ccache") {
            stage('Build') {
                sh '''
                    export CCACHE_DIR=/ccache
                    cmake -B build -DCMAKE_C_COMPILER_LAUNCHER=ccache
                    cmake --build build -j$(nproc)
                '''
            }

            stage('Test') {
                sh 'cd build && ctest --output-on-failure'
            }

            stage('Package') {
                sh '''
                    cd build
                    cpack -G DEB
                    cpack -G RPM
                    cpack -G TGZ
                '''
                archiveArtifacts artifacts: 'build/*.deb,build/*.rpm,build/*.tar.gz'
            }
        }

        stage('Deploy') {
            if (env.BRANCH_NAME == 'main') {
                sh '''
                    build/deploy.sh production
                '''
            }
        }

    } catch (Exception e) {
        currentBuild.result = 'FAILURE'
        throw e
    } finally {
        cleanWs()
    }
}
```

---

## 自动化测试

### C语言测试框架对比

| 框架 | 特点 | 适用场景 |
|------|------|----------|
| **Unity** | 轻量级，嵌入式友好 | 嵌入式/资源受限环境 |
| **CUnit** | 标准xUnit风格 | 通用单元测试 |
| **Check** | 多进程隔离测试 | 需要进程隔离的测试 |
| **CMocka** | 支持模拟函数 | 需要mock的测试 |
| **GoogleTest** | 功能丰富(C++但可测C) | 复杂测试需求 |

### Unity测试框架示例

```c
// test_math.c
#include "unity.h"
#include "math_utils.h"

void setUp(void) {
    // 每个测试前执行
}

void tearDown(void) {
    // 每个测试后执行
}

// 测试用例
void test_add_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
    TEST_ASSERT_EQUAL_INT(100, add(50, 50));
}

void test_add_negative_numbers(void) {
    TEST_ASSERT_EQUAL_INT(-5, add(-2, -3));
    TEST_ASSERT_EQUAL_INT(0, add(-5, 5));
}

void test_add_overflow(void) {
    int a = INT_MAX;
    int b = 1;
    // 检测溢出
    int result;
    int status = add_safe(a, b, &result);
    TEST_ASSERT_EQUAL_INT(ERROR_OVERFLOW, status);
}

void test_divide_by_zero(void) {
    // 测试除零保护
    TEST_ASSERT_EQUAL_INT(0, divide(10, 0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_negative_numbers);
    RUN_TEST(test_add_overflow);
    RUN_TEST(test_divide_by_zero);
    return UNITY_END();
}
```

### 内存测试集成

```cmake
# CMakeLists.txt - 测试配置
enable_testing()

# 单元测试
add_executable(test_unit test_math.c math_utils.c)
target_link_libraries(test_unit unity)
add_test(NAME UnitTests COMMAND test_unit)

# 内存测试 (Valgrind)
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_test(NAME MemoryTest
             COMMAND ${VALGRIND} --leak-check=full --error-exitcode=1 $<TARGET_FILE:test_unit>)
endif()

# 模糊测试 (AFL)
find_program(AFL_FUZZ afl-fuzz)
if(AFL_FUZZ)
    add_custom_target(fuzz
        COMMAND ${AFL_FUZZ} -i ${CMAKE_SOURCE_DIR}/testcases -o findings
                          ${CMAKE_BINARY_DIR}/fuzz_target @@
        DEPENDS fuzz_target
    )
endif()

# 覆盖率
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

    add_custom_target(coverage
        COMMAND ${CMAKE_MAKE_PROGRAM} test
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --html --html-details -o coverage.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
```

### 集成测试框架

```c
// test_integration.c - 集成测试示例
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "api_server.h"

// 测试数据库连接
test_database_connection() {
    db_config_t config = {
        .host = "localhost",
        .port = 5432,
        .database = "test_db",
        .user = "test_user",
        .password = "test_pass"
    };

    db_handle_t *db = db_connect(&config);
    ASSERT_NOT_NULL(db);

    // 测试CRUD操作
    db_execute(db, "BEGIN TRANSACTION");

    // 插入测试数据
    int result = db_execute(db,
        "INSERT INTO users (name, email) VALUES ('test', 'test@example.com')");
    ASSERT_EQUAL(1, result);  // 影响1行

    // 查询验证
    result_set_t *rs = db_query(db, "SELECT * FROM users WHERE name='test'");
    ASSERT_NOT_NULL(rs);
    ASSERT_TRUE(rs_next(rs));
    ASSERT_EQUAL_STRING("test@example.com", rs_get_string(rs, "email"));

    // 回滚，保持测试隔离
    db_execute(db, "ROLLBACK");

    db_disconnect(db);
    printf("Database integration test: PASSED\n");
}

// 测试API端点
test_api_endpoints() {
    // 启动测试服务器
    server_t *server = server_start("127.0.0.1", 0);  // 随机端口
    int port = server_get_port(server);

    // 测试GET请求
    http_response_t *resp = http_get(
        format("http://127.0.0.1:%d/api/health", port));
    ASSERT_EQUAL(200, resp->status);
    ASSERT_CONTAINS("healthy", resp->body);

    // 测试POST请求
    const char *json = "{\"name\":\"test\",\"value\":123}";
    resp = http_post(
        format("http://127.0.0.1:%d/api/data", port),
        json, "application/json");
    ASSERT_EQUAL(201, resp->status);

    http_response_free(resp);
    server_stop(server);
    printf("API integration test: PASSED\n");
}

int main() {
    printf("Running integration tests...\n");

    test_database_connection();
    test_api_endpoints();

    printf("All integration tests passed!\n");
    return 0;
}
```

---

## 代码质量门禁

### 质量指标阈值设置

```yaml
# .quality-gate.yml
metrics:
  # 代码复杂度
  complexity:
    max_function_complexity: 10
    max_file_complexity: 100
    warning_threshold: 8

  # 代码重复
  duplication:
    max_percentage: 3.0
    min_lines: 10

  # 代码覆盖率
  coverage:
    min_line_coverage: 80
    min_branch_coverage: 70
    min_function_coverage: 90

  # 代码风格
  style:
    max_line_length: 100
    indent_size: 4
    use_spaces: true

  # 文档
  documentation:
    min_public_api_documentation: 80
    require_file_header: true

# 静态分析规则
cppcheck:
  enable:
    - all
  suppress:
    - missingIncludeSystem
    - unusedFunction  # 库代码常有

# 安全检查
security:
  banned_functions:
    - strcpy
    - strcat
    - gets
    - sprintf
  required_checks:
    - buffer_overflow
    - format_string
    - integer_overflow
```

### 质量门禁集成

```groovy
// Jenkins 质量门禁
stage('Quality Gate') {
    steps {
        script {
            // SonarQube分析
            def scannerHome = tool 'SonarScanner';
            withSonarQubeEnv('SonarQube') {
                sh "${scannerHome}/bin/sonar-scanner"
            }

            // 等待质量门禁结果
            timeout(time: 10, unit: 'MINUTES') {
                waitForQualityGate abortPipeline: true
            }

            // 代码覆盖率检查
            def coverage = sh(
                script: 'gcovr --print-summary | grep "lines:" | awk \'{print $2}\' | tr -d \'%\'',
                returnStdout: true
            ).trim()

            if (coverage.toInteger() < 80) {
                error("Code coverage ${coverage}% is below threshold 80%")
            }

            // 复杂度检查
            def complexity = sh(
                script: 'lizard src/ | tail -1 | awk \'{print $3}\'',
                returnStdout: true
            ).trim()

            if (complexity.toInteger() > 10) {
                unstable("Maximum complexity ${complexity} exceeds threshold 10")
            }
        }
    }
}
```

### 代码审查清单

```markdown
## C代码审查清单

### 安全性
- [ ] 所有字符串操作使用安全版本 (strncpy, strncat)
- [ ] 所有输入都经过验证
- [ ] 没有缓冲区溢出风险
- [ ] 没有格式化字符串漏洞
- [ ] 敏感数据正确处理

### 内存管理
- [ ] 每个malloc都有对应的free
- [ ] 释放后将指针置NULL
- [ ] 没有使用已释放内存
- [ ] 没有内存泄漏

### 代码质量
- [ ] 函数长度不超过50行
- [ ] 圈复杂度不超过10
- [ ] 有意义的变量名
- [ ] 适当的注释

### 可移植性
- [ ] 不使用平台特定代码
- [ ] 正确处理字节序
- [ ] 对齐要求满足
```

---

## C项目CI/CD最佳实践

### 项目结构

```text
c-project/
├── .github/workflows/      # GitHub Actions配置
│   ├── ci.yml
│   └── release.yml
├── .gitlab-ci.yml          # GitLab CI配置
├── Jenkinsfile             # Jenkins配置
├── cmake/
│   ├── modules/
│   └── options.cmake
├── include/                # 头文件
├── src/                    # 源文件
├── tests/                  # 测试代码
│   ├── unit/
│   ├── integration/
│   └── fuzz/
├── docs/                   # 文档
├── scripts/                # 辅助脚本
│   ├── build.sh
│   ├── test.sh
│   └── deploy.sh
├── Dockerfile              # 构建环境
├── docker-compose.yml      # 本地开发环境
├── CMakeLists.txt          # 主构建配置
├── conanfile.txt           # 依赖管理
└── README.md
```

### 完整的CMake配置

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(MyProject VERSION 1.0.0 LANGUAGES C)

# 标准设置
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 选项
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" ON)
option(ENABLE_COVERAGE "Enable coverage" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(ENABLE_LTO "Enable link-time optimization" OFF)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# 静态分析
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    set(CMAKE_C_CPPCHECK ${CPPCHECK} --enable=all --suppress=missingIncludeSystem)
endif()

# 覆盖率
if(ENABLE_COVERAGE)
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
    link_libraries(gcov)
endif()

# Sanitizers
if(ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

# 子目录
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

### Docker构建环境

```dockerfile
# Dockerfile.build
FROM gcc:12

RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    ccache \
    valgrind \
    cppcheck \
    clang-format \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install conan gcovr

ENV CCACHE_DIR=/ccache
ENV PATH="/usr/lib/ccache:$PATH"

WORKDIR /build

ENTRYPOINT ["/bin/bash"]
```

```yaml
# docker-compose.yml
version: '3.8'

services:
  build:
    build:
      context: .
      dockerfile: Dockerfile.build
    volumes:
      - .:/workspace
      - ccache:/ccache
    working_dir: /workspace
    command: bash -c "cmake -B build && cmake --build build"

  test:
    extends: build
    command: bash -c "cd build && ctest --output-on-failure"

  valgrind:
    extends: build
    command: bash -c "valgrind --leak-check=full ./build/myapp test"

volumes:
  ccache:
```

### 发布流程

```yaml
# .github/workflows/release.yml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  build-and-release:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        include:
          - os: ubuntu-latest
            artifact_name: myapp-linux
            asset_name: myapp-linux-amd64
          - os: macos-latest
            artifact_name: myapp-macos
            asset_name: myapp-macos-amd64
          - os: windows-latest
            artifact_name: myapp.exe
            asset_name: myapp-windows-amd64.exe

    steps:
    - uses: actions/checkout@v4

    - name: Build
      run: |
        cmake -B build -DCMAKE_BUILD_TYPE=Release
        cmake --build build --config Release

    - name: Package
      run: |
        cd build
        cpack -G TGZ

    - name: Upload Release Asset
      uses: softprops/action-gh-release@v1
      with:
        files: build/*.tar.gz
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

---

## 参考资源

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [GitLab CI/CD Documentation](https://docs.gitlab.com/ee/ci/)
- [Jenkins Handbook](https://www.jenkins.io/doc/book/)
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [CTest Documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

---

*"自动化是质量的基石，CI/CD是持续交付的保障。"*


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
