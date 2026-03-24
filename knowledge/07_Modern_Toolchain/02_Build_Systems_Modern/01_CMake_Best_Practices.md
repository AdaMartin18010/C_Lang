# CMake现代最佳实践

> **层级定位**: 07_Modern_Toolchain > 02_Build_Systems_Modern
> **难度级别**: L3-L4 (中级到高级)
> **前置知识**: 基本CMake使用, C/C++项目结构
> **预估学习时间**: 2-3 小时

---

## 模块概述

CMake是现代C/C++项目的事实标准构建系统。
本指南介绍CMake 3.15+的现代用法，帮助你编写可维护、跨平台的构建脚本。

---

## 学习目标

1. **掌握现代CMake范式** - 目标导向、基于属性的方法
2. **组织项目结构** - 模块化、可重用的CMake代码
3. **管理依赖** - FetchContent、FindPackage
4. **支持跨平台** - Windows、macOS、Linux

---

## 一、现代CMake核心原则

### 1.1 旧式 vs 现代CMake

```cmake
# ❌ 旧式CMake (CMake 2.x风格)

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

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)
add_definitions(-DDEBUG)

add_executable(myapp src/main.c)
target_link_libraries(myapp pthread)

# ✅ 现代CMake (CMake 3.15+风格)
add_executable(myapp src/main.c)
target_include_directories(myapp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(myapp PRIVATE DEBUG)
target_link_libraries(myapp
    PRIVATE
        Threads::Threads
)
```

### 1.2 核心原则

| 原则 | 说明 | 示例 |
|:-----|:-----|:-----|
| **目标导向** | 一切围绕目标 | `add_executable()`, `add_library()` |
| **属性传播** | 使用`PRIVATE/PUBLIC/INTERFACE` | 控制依赖传递 |
| **避免全局设置** | 不使用全局`include_directories` | 使用`target_include_directories` |
| **显式优于隐式** | 明确指定所有内容 | 不依赖隐式行为 |

---

## 二、项目结构模板

### 2.1 推荐目录结构

```
my_project/
├── CMakeLists.txt              # 根CMake文件
├── CMakePresets.json           # 预设配置
├── cmake/
│   ├── FindMyDependency.cmake  # 自定义查找模块
│   └── custom_functions.cmake  # 自定义函数
├── include/
│   └── myproject/
│       └── header.h            # 公共头文件
├── src/
│   ├── CMakeLists.txt          # 源码CMake
│   ├── main.c                  # 可执行文件源码
│   └── internal.c              # 内部实现
├── lib/
│   └── CMakeLists.txt          # 库CMake
├── tests/
│   └── CMakeLists.txt          # 测试CMake
└── docs/
    └── CMakeLists.txt          # 文档CMake
```

### 2.2 根CMakeLists.txt

```cmake
# CMakeLists.txt - 项目根文件
cmake_minimum_required(VERSION 3.15)

# 项目定义
project(MyProject
    VERSION 1.0.0
    DESCRIPTION "A modern C project"
    LANGUAGES C
)

# C标准设置
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 包含模块路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# 选项
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)

# 编译选项
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wstrict-overflow
    -Werror=return-type
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# Sanitizers
if(ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

# 添加子目录
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装规则
include(GNUInstallDirs)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
```

### 2.3 库目标定义

```cmake
# lib/CMakeLists.txt

# 创建库
add_library(mylib STATIC)

# 源文件
target_sources(mylib
    PRIVATE
        mylib.c
        utils.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_SOURCE_DIR}/include
        FILES
            ${CMAKE_SOURCE_DIR}/include/mylib.h
            ${CMAKE_SOURCE_DIR}/include/utils.h
)

# 包含目录
target_include_directories(mylib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 编译定义
target_compile_definitions(mylib
    PUBLIC
        MYLIB_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        MYLIB_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PRIVATE
        MYLIB_INTERNAL
)

# 链接依赖
target_link_libraries(mylib
    PUBLIC
        Threads::Threads
    PRIVATE
        m  # math library
)

# 编译特性
target_compile_features(mylib PUBLIC c_std_17)

# 安装
install(TARGETS mylib
    EXPORT MyLibTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS
)

install(EXPORT MyLibTargets
    FILE MyLibTargets.cmake
    NAMESPACE MyLib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyLib
)
```

### 2.4 可执行文件定义

```cmake
# src/CMakeLists.txt

add_executable(myapp)

target_sources(myapp
    PRIVATE
        main.c
        cli.c
)

target_include_directories(myapp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(myapp
    PRIVATE
        mylib
)

# 设置可执行文件输出名称
set_target_properties(myapp PROPERTIES
    OUTPUT_NAME "myapp"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
```

---

## 三、依赖管理

### 3.1 FetchContent (推荐)

```cmake
# 使用FetchContent管理依赖
include(FetchContent)

# 声明依赖
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.5.2
    GIT_SHALLOW TRUE
)

# 使依赖可用
FetchContent_MakeAvailable(json unity)

# 使用
target_link_libraries(myapp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(tests PRIVATE unity)
```

### 3.2 FindPackage

```cmake
# 查找系统包
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# 使用pkg-config
pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)

# 创建导入目标(如果不存在)
if(NOT TARGET LibXml2::LibXml2)
    add_library(LibXml2::LibXml2 INTERFACE IMPORTED)
    set_target_properties(LibXml2::LibXml2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIBXML2_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${LIBXML2_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${LIBXML2_CFLAGS_OTHER}"
    )
endif()

# 使用
target_link_libraries(mylib PUBLIC LibXml2::LibXml2)
```

### 3.3 自定义Find模块

```cmake
# cmake/FindMyDependency.cmake

include(FindPackageHandleStandardArgs)

find_path(MYDEP_INCLUDE_DIR mydep.h
    PATHS
        ${MYDEP_ROOT}/include
        $ENV{MYDEP_ROOT}/include
        /usr/local/include
        /usr/include
)

find_library(MYDEP_LIBRARY
    NAMES mydep
    PATHS
        ${MYDEP_ROOT}/lib
        $ENV{MYDEP_ROOT}/lib
        /usr/local/lib
        /usr/lib
)

find_package_handle_standard_args(MyDependency
    REQUIRED_VARS
        MYDEP_LIBRARY
        MYDEP_INCLUDE_DIR
)

if(MyDependency_FOUND AND NOT TARGET MyDependency::MyDependency)
    add_library(MyDependency::MyDependency UNKNOWN IMPORTED)
    set_target_properties(MyDependency::MyDependency PROPERTIES
        IMPORTED_LOCATION "${MYDEP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MYDEP_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(MYDEP_INCLUDE_DIR MYDEP_LIBRARY)
```

---

## 四、测试集成

### 4.1 CTest配置

```cmake
# tests/CMakeLists.txt

# 测试可执行文件
add_executable(test_mylib
    test_main.c
    test_utils.c
)

target_link_libraries(test_mylib
    PRIVATE
        mylib
        unity
)

# 添加测试
add_test(NAME test_mylib COMMAND test_mylib)

# 测试属性
set_tests_properties(test_mylib PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)

# 内存检查(Valgrind)
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_test(NAME test_mylib_valgrind
        COMMAND ${VALGRIND} --leak-check=full --error-exitcode=1 $<TARGET_FILE:test_mylib>
    )
endif()
```

### 4.2 代码覆盖率

```cmake
# 启用覆盖率
option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-O0 -g --coverage)
        add_link_options(--coverage)

        find_program(GCOV gcov)
        find_program(LCOV lcov)
        find_program(GENHTML genhtml)

        if(LCOV AND GENHTML)
            add_custom_target(coverage
                COMMAND ${LCOV} --capture --directory . --output-file coverage.info
                COMMAND ${LCOV} --remove coverage.info '/usr/*' --output-file coverage.info
                COMMAND ${GENHTML} coverage.info --output-directory coverage_report
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating coverage report"
            )
        endif()
    endif()
endif()
```

---

## 五、跨平台支持

### 5.1 平台检测

```cmake
# 平台检测
if(WIN32)
    message(STATUS "Building for Windows")
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(APPLE)
    message(STATUS "Building for macOS")
    set(CMAKE_MACOSX_RPATH ON)
elseif(UNIX)
    message(STATUS "Building for Linux/Unix")
endif()

# 编译器检测
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    message(STATUS "Using GCC")
    add_compile_options(-fdiagnostics-color=always)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(STATUS "Using Clang")
elseif(MSVC)
    message(STATUS "Using MSVC")
    add_compile_options(/W4 /WX-)
endif()
```

### 5.2 条件编译

```cmake
# 功能检测
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckSymbolExists)

check_function_exists(strlcpy HAVE_STRLCPY)
check_include_file(sys/epoll.h HAVE_EPOLL)
check_symbol_exists(atomic_int_least32_t stdatomic.h HAVE_C11_ATOMICS)

# 生成配置头
configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/include/config.h
)

target_include_directories(mylib
    PUBLIC
        ${CMAKE_BINARY_DIR}/include
)
```

```c
// config.h.in
#ifndef CONFIG_H
#define CONFIG_H

#cmakedefine HAVE_STRLCPY
#cmakedefine HAVE_EPOLL
#cmakedefine HAVE_C11_ATOMICS

#define PROJECT_VERSION "@PROJECT_VERSION@"
#define PROJECT_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define PROJECT_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define PROJECT_VERSION_PATCH @PROJECT_VERSION_PATCH@

#endif
```

---

## 六、CMake预设

### 6.1 CMakePresets.json

```json
{
    "version": 6,
    "cmakeMinimumRequired": {
        "major": 3,
        "minor": 23,
        "patch": 0
    },
    "configurePresets": [
        {
            "name": "default",
            "hidden": true,
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/${presetName}",
            "cacheVariables": {
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        },
        {
            "name": "debug",
            "inherits": "default",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "ENABLE_SANITIZERS": "ON",
                "BUILD_TESTS": "ON"
            }
        },
        {
            "name": "release",
            "inherits": "default",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "CMAKE_INTERPROCEDURAL_OPTIMIZATION": "ON"
            }
        },
        {
            "name": "windows-debug",
            "inherits": "debug",
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Windows"
            },
            "architecture": {
                "value": "x64",
                "strategy": "external"
            },
            "toolchainFile": "${sourceDir}/cmake/toolchains/windows.cmake"
        }
    ],
    "buildPresets": [
        {
            "name": "debug",
            "configurePreset": "debug"
        },
        {
            "name": "release",
            "configurePreset": "release"
        }
    ],
    "testPresets": [
        {
            "name": "debug",
            "configurePreset": "debug",
            "output": {
                "outputOnFailure": true
            }
        }
    ]
}
```

### 6.2 使用预设

```bash
# 配置
cmake --preset debug

# 构建
cmake --build --preset debug

# 测试
cmake --build --preset debug --target test

# 或直接
cmake --preset release
cmake --build --preset release
```

---

## 七、常用命令速查

| 命令 | 说明 |
|:-----|:-----|
| `cmake -B build` | 配置项目 |
| `cmake --build build` | 构建项目 |
| `cmake --install build --prefix /usr/local` | 安装 |
| `cmake -DCMAKE_BUILD_TYPE=Debug` | Debug构建 |
| `cmake -DBUILD_TESTS=ON` | 启用测试 |
| `ctest --output-on-failure` | 运行测试 |

---

## 八、常见问题

### 问题1: 找不到头文件

**解决**: 使用 `target_include_directories` 而非 `include_directories`

### 问题2: 链接错误

**解决**: 确保使用 `target_link_libraries` 并正确设置 `PUBLIC/PRIVATE/INTERFACE`

### 问题3: 编译器标志不生效

**解决**: 使用生成器表达式 `$<$<CONFIG:Debug>:-O0>`

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
**质量等级**: L4 (深化)
