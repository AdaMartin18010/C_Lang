# CMake现代最佳实践 (3.15+)

> **层级定位**: 07 Modern Toolchain / 02 Build Systems
> **难度级别**: L3 进阶
> **预估学习时间**: 6-8小时

---

## 📋 为什么需要现代CMake

### 传统CMake vs 现代CMake

| 特性 | 传统CMake (<3.0) | 现代CMake (3.15+) |
|:-----|:-----------------|:------------------|
| 核心概念 | 变量导向 | **目标导向** |
| 依赖管理 | 全局变量 | `target_link_libraries` |
| 传递性 | 手动处理 | **自动传递** |
| 可读性 | 复杂难懂 | 清晰简洁 |

### 核心原则

```
现代CMake = 目标(Target) + 属性(Property) + 传递性(Transitivity)
```

---

## 🚀 快速开始

### 最小现代CMake项目

**CMakeLists.txt**:

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject VERSION 1.0.0 LANGUAGES C)

# 创建可执行文件目标
add_executable(myapp main.c)

# 设置C标准
target_compile_features(myapp PRIVATE c_std_11)

# 添加包含目录
target_include_directories(myapp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# 链接库
target_link_libraries(myapp PRIVATE m)

# 编译选项
target_compile_options(myapp PRIVATE -Wall -Wextra)
```

**构建**:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

---

## 📚 核心概念详解

### 1. 目标(Target)为中心

#### 创建目标

```cmake
# 可执行文件
add_executable(myapp src/main.c)

# 库文件
add_library(mylib STATIC src/mylib.c)
add_library(mylib_shared SHARED src/mylib.c)

# 接口库(头文件-only)
add_library(myheaders INTERFACE)
```

#### 目标属性设置

```cmake
# 设置C标准
target_compile_features(myapp PRIVATE c_std_11)

# 编译选项
target_compile_options(myapp PRIVATE -Wall -Wextra -Werror)

# 宏定义
target_compile_definitions(myapp PRIVATE DEBUG_MODE)

# 包含目录
target_include_directories(myapp PUBLIC include)

# 链接库
target_link_libraries(myapp PRIVATE m pthread)
```

#### 可见性关键字

| 关键字 | 作用 | 使用场景 |
|:-------|:-----|:---------|
| `PRIVATE` | 仅当前目标使用 | 实现细节依赖 |
| `PUBLIC` | 当前目标和依赖者都使用 | 接口一部分 |
| `INTERFACE` | 仅依赖者使用 | 头文件库 |

**示例**:

```cmake
add_library(mathlib src/math.c)
target_include_directories(mathlib PUBLIC include)
# mathlib的使用者会自动获得include路径

add_executable(app main.c)
target_link_libraries(app PRIVATE mathlib)
# app自动获得mathlib的PUBLIC属性
```

### 2. 项目结构最佳实践

#### 推荐目录结构

```
myproject/
├── CMakeLists.txt          # 根配置
├── cmake/                  # CMake模块
│   └── compiler_warnings.cmake
├── include/                # 公共头文件
│   └── myproject/
│       └── config.h
├── src/                    # 源文件
│   ├── main.c
│   └── utils.c
├── tests/                  # 测试
│   └── CMakeLists.txt
├── docs/                   # 文档
└── examples/               # 示例
    └── CMakeLists.txt
```

#### 根CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)

# 项目声明
project(MyProject
    VERSION 1.0.0
    DESCRIPTION "A sample C project"
    LANGUAGES C
)

# C标准设置
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 编译选项
add_compile_options(-Wall -Wextra -Wpedantic)

# 生成编译数据库(用于clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 子目录
add_subdirectory(src)
add_subdirectory(tests)
```

#### src/CMakeLists.txt

```cmake
# 收集源文件
set(SOURCES
    main.c
    utils.c
    math.c
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 包含目录
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        m
        pthread
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
```

### 3. CMake Presets (3.19+)

**CMakePresets.json**:

```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "default",
            "hidden": true,
            "generator": "Ninja",
            "cacheVariables": {
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        },
        {
            "name": "debug",
            "inherits": "default",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_C_FLAGS": "-g -O0 -fsanitize=address"
            }
        },
        {
            "name": "release",
            "inherits": "default",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "CMAKE_C_FLAGS": "-O3 -march=native"
            }
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
    ]
}
```

**使用**:

```bash
cmake --preset=debug
cmake --build --preset=debug
```

### 4. 工具链文件

**交叉编译工具链** (arm-toolchain.cmake):

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

**使用**:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake ..
```

### 5. 测试集成

```cmake
# 启用测试
enable_testing()

# 添加测试可执行文件
add_executable(test_math test_math.c)
target_link_libraries(test_math PRIVATE mathlib)

# 添加测试
add_test(NAME MathTest COMMAND test_math)

# 设置测试属性
set_tests_properties(MathTest PROPERTIES
    TIMEOUT 10
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
```

**运行测试**:

```bash
ctest --output-on-failure
```

---

## 🔧 高级技巧

### 条件编译

```cmake
# 检查编译器
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(myapp PRIVATE -Wno-unused-result)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(myapp PRIVATE -Wno-unused-command-line-argument)
endif()

# 检查平台
if(WIN32)
    target_compile_definitions(myapp PRIVATE WINDOWS)
elseif(APPLE)
    target_compile_definitions(myapp PRIVATE MACOS)
elseif(UNIX)
    target_compile_definitions(myapp PRIVATE LINUX)
endif()
```

### 查找依赖

```cmake
# 查找包
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

# 使用找到的包
target_include_directories(myapp PRIVATE ${LIBUSB_INCLUDE_DIRS})
target_link_libraries(myapp PRIVATE ${LIBUSB_LIBRARIES})
```

### 生成器表达式

```cmake
# 条件表达式
target_compile_options(myapp PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# 平台相关
 target_link_libraries(myapp PRIVATE
    $<$<PLATFORM_ID:Windows>:ws2_32>
    $<$<PLATFORM_ID:Linux>:pthread>
)
```

---

## ✅ 现代CMake检查清单

- [ ] 使用目标导向而非变量导向
- [ ] 正确使用PRIVATE/PUBLIC/INTERFACE
- [ ] 使用target_*命令而非全局设置
- [ ] 提供CMakePresets.json
- [ ] 启用CMAKE_EXPORT_COMPILE_COMMANDS
- [ ] 设置合适的C标准
- [ ] 添加测试
- [ ] 提供安装规则

---

## 📖 参考资源

- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
- [CMake官方文档](https://cmake.org/documentation/)
- [Effective Modern CMake](https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1)

---

**← [返回工具链主页](../README.md)**
**← [构建系统主页](./README.md)**


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
