# 项目模板


---

## 📑 目录

- [项目模板](#项目模板)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [1. 项目脚手架 (Project Scaffolding)](#1-项目脚手架-project-scaffolding)
    - [1.1 标准目录结构](#11-标准目录结构)
    - [1.2 项目模板生成器](#12-项目模板生成器)
  - [2. CMake模板](#2-cmake模板)
    - [2.1 现代CMake最佳实践模板](#21-现代cmake最佳实践模板)
    - [2.2 跨平台CMake配置](#22-跨平台cmake配置)
  - [3. 最佳实践模板](#3-最佳实践模板)
    - [3.1 头文件模板](#31-头文件模板)
    - [3.2 源文件模板](#32-源文件模板)
  - [4. CI/CD配置模板](#4-cicd配置模板)
    - [4.1 GitHub Actions工作流](#41-github-actions工作流)
  - [5. 使用指南](#5-使用指南)
    - [5.1 快速开始](#51-快速开始)
    - [5.2 项目维护命令](#52-项目维护命令)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

项目模板是标准化的项目脚手架，提供一致的目录结构、配置文件和代码规范，帮助开发团队快速启动新项目并保持一致性。良好的项目模板包含构建系统配置、测试框架、代码规范工具、CI/CD配置等，是现代C语言工程实践的基础设施。

---

## 1. 项目脚手架 (Project Scaffolding)

### 1.1 标准目录结构

```text
my_c_project/
├── .github/
│   └── workflows/              # GitHub Actions CI配置
│       ├── build.yml
│       └── release.yml
├── .vscode/
│   ├── settings.json           # VS Code编辑器配置
│   ├── tasks.json              # 构建任务
│   └── launch.json             # 调试配置
├── cmake/
│   ├── modules/                # CMake模块
│   │   ├── FindXXX.cmake
│   │   └── CompilerWarnings.cmake
│   └── options.cmake           # 构建选项
├── docs/
│   ├── Doxyfile                # 文档生成配置
│   ├── api/                    # API文档
│   └── design.md               # 设计文档
├── include/
│   └── project_name/
│       ├── module_a.h
│       └── module_b.h
├── src/
│   ├── module_a.c
│   ├── module_b.c
│   └── main.c
├── tests/
│   ├── CMakeLists.txt
│   ├── unit/
│   │   ├── test_module_a.c
│   │   └── test_module_b.c
│   ├── integration/
│   │   └── test_integration.c
│   └── fixtures/               # 测试数据
├── third_party/                # 外部依赖
│   └── .gitkeep
├── tools/
│   ├── format.sh               # 格式化脚本
│   └── static_analysis.sh      # 静态分析脚本
├── .clang-format               # 代码格式化配置
├── .clang-tidy                 # 静态分析配置
├── .editorconfig               # 编辑器通用配置
├── .gitignore                  # Git忽略规则
├── CMakeLists.txt              # 主CMake配置
├── LICENSE                     # 许可证
└── README.md                   # 项目说明
```

### 1.2 项目模板生成器

```python
#!/usr/bin/env python3
"""C项目脚手架生成器"""

import os
import argparse
from pathlib import Path

PROJECT_TEMPLATE = {
    'directories': [
        '.github/workflows',
        '.vscode',
        'cmake/modules',
        'docs/api',
        'include/{project_name}',
        'src',
        'tests/unit',
        'tests/integration',
        'tests/fixtures',
        'third_party',
        'tools',
    ],
    'files': {
        '.gitignore': '''# Build artifacts
/build/
/cmake-build-*/
/out/

# IDE
.idea/
*.swp
*.swo
*~
.vs/

# Compiled files
*.o
*.obj
*.a
*.lib
*.so
*.dll
*.dylib
*.exe

# Testing
Testing/
CTestTestfile.cmake

# Documentation
docs/html/
docs/latex/
''',
        '.editorconfig': '''root = true

[*]
charset = utf-8
end_of_line = lf
insert_final_newline = true
trim_trailing_whitespace = true

[*.{c,h}]
indent_style = space
indent_size = 4
max_line_length = 100

[*.md]
trim_trailing_whitespace = false

[*.yml]
indent_style = space
indent_size = 2
''',
        'CMakeLists.txt': '''cmake_minimum_required(VERSION 3.14)

# Project definition
project({project_name}
    VERSION 0.1.0
    DESCRIPTION "{description}"
    LANGUAGES C
)

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(ENABLE_SANITIZERS "Enable sanitizers in debug mode" ON)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include modules
list(APPEND CMAKE_MODULE_PATH "${{CMAKE_CURRENT_SOURCE_DIR}}/cmake/modules")

# Compiler warnings
if(ENABLE_WARNINGS)
    include(CompilerWarnings)
endif()

# Sanitizers
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

# Library
add_library(${{PROJECT_NAME}})

target_sources(${{PROJECT_NAME}}
    PRIVATE
        src/module_a.c
        src/module_b.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/${{PROJECT_NAME}}/module_a.h
            include/${{PROJECT_NAME}}/module_b.h
)

target_include_directories(${{PROJECT_NAME}}
    PUBLIC
        $<BUILD_INTERFACE:${{CMAKE_CURRENT_SOURCE_DIR}}/include>
        $<INSTALL_INTERFACE:include>
)

# Executable
add_executable(${{PROJECT_NAME}}_app src/main.c)
target_link_libraries(${{PROJECT_NAME}}_app PRIVATE ${{PROJECT_NAME}})

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    endif()
endif()

# Installation
include(GNUInstallDirs)
install(TARGETS ${{PROJECT_NAME}}
    EXPORT ${{PROJECT_NAME}}-targets
    FILE_SET HEADERS
    LIBRARY DESTINATION ${{CMAKE_INSTALL_LIBDIR}}
    ARCHIVE DESTINATION ${{CMAKE_INSTALL_LIBDIR}}
    RUNTIME DESTINATION ${{CMAKE_INSTALL_BINDIR}}
)
''',
        'include/{project_name}/module_a.h': '''#ifndef {PROJECT_NAME_UPPER}_MODULE_A_H
#define {PROJECT_NAME_UPPER}_MODULE_A_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {{
#endif

/**
 * @file module_a.h
 * @brief Module A public interface
 */

/**
 * @brief Initialize module A
 * @return 0 on success, negative error code on failure
 */
int {project_name}_module_a_init(void);

/**
 * @brief Cleanup module A
 */
void {project_name}_module_a_deinit(void);

/**
 * @brief Process data
 * @param input Input buffer
 * @param input_len Input length
 * @param output Output buffer
 * @param output_len Output buffer size
 * @return Number of bytes written, negative on error
 */
int {project_name}_module_a_process(
    const uint8_t *input,
    size_t input_len,
    uint8_t *output,
    size_t output_len
);

#ifdef __cplusplus
}}
#endif

#endif /* {PROJECT_NAME_UPPER}_MODULE_A_H */
''',
        'src/module_a.c': '''#include "{project_name}/module_a.h"
#include <string.h>

static int g_initialized = 0;

int {project_name}_module_a_init(void) {{
    if (g_initialized) {{
        return 0;
    }}

    /* 初始化模块资源 */
    /* 例如：打开设备、分配缓冲区、建立连接等 */

    g_initialized = 1;
    return 0;
}}

void {project_name}_module_a_deinit(void) {{
    if (!g_initialized) {{
        return;
    }}

    /* 释放模块资源 */
    /* 例如：关闭设备、释放缓冲区、断开连接等 */

    g_initialized = 0;
}}

int {project_name}_module_a_process(
    const uint8_t *input,
    size_t input_len,
    uint8_t *output,
    size_t output_len) {{

    if (!g_initialized) {{
        return -1;
    }}

    if (!input || !output) {{
        return -1;
    }}

    if (input_len > output_len) {{
        return -1;
    }}

    /* 处理数据：简单示例是复制输入到输出 */
    /* 实际应用中，这里应该是模块的核心处理逻辑 */
    memcpy(output, input, input_len);

    return (int)input_len;
}}
''',
        'src/main.c': '''#include <stdio.h>
#include <stdlib.h>
#include "{project_name}/module_a.h"

int main(int argc, char **argv) {{
    (void)argc;
    (void)argv;

    printf("{project_name} v0.1.0\\n");

    if ({project_name}_module_a_init() != 0) {{
        fprintf(stderr, "Failed to initialize module A\\n");
        return EXIT_FAILURE;
    }}

    /* 应用程序主逻辑 */
    /* 示例：读取输入、调用模块处理、输出结果 */
    printf("Module initialized successfully\n");

    /* 示例数据处理 */
    uint8_t input[] = "Hello, World!";
    uint8_t output[256] = {0};
    int result = {project_name}_module_a_process(
        input, sizeof(input), output, sizeof(output));

    if (result > 0) {
        printf("Processed %d bytes: %s\n", result, output);
    }

    {project_name}_module_a_deinit();
    return EXIT_SUCCESS;
}}
''',
        'tests/CMakeLists.txt': '''# Tests CMakeLists.txt

include(FetchContent)

# Fetch Unity test framework
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.5.2
)
FetchContent_MakeAvailable(unity)

# Test executable
add_executable({project_name}_tests
    unit/test_module_a.c
)

target_link_libraries({project_name}_tests
    PRIVATE
        {project_name}
        unity
)

# Add tests
add_test(NAME {project_name}_unit_tests COMMAND {project_name}_tests)
''',
        'tests/unit/test_module_a.c': '''#include "unity.h"
#include "{project_name}/module_a.h"

void setUp(void) {{
    {project_name}_module_a_init();
}}

void tearDown(void) {{
    {project_name}_module_a_deinit();
}}

void test_module_a_init_deinit(void) {{
    TEST_ASSERT_EQUAL(0, {project_name}_module_a_init());
    {project_name}_module_a_deinit();
}}

void test_module_a_process_null_input(void) {{
    uint8_t output[100];
    int result = {project_name}_module_a_process(NULL, 10, output, sizeof(output));
    TEST_ASSERT_LESS_THAN(0, result);
}}

void test_module_a_process_normal(void) {{
    uint8_t input[] = "Hello, World!";
    uint8_t output[100] = {{0}};

    int result = {project_name}_module_a_process(
        input, sizeof(input), output, sizeof(output));

    TEST_ASSERT_EQUAL(sizeof(input), result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(input, output, sizeof(input));
}}

int main(void) {{
    UNITY_BEGIN();

    RUN_TEST(test_module_a_init_deinit);
    RUN_TEST(test_module_a_process_null_input);
    RUN_TEST(test_module_a_process_normal);

    return UNITY_END();
}}
''',
        '.clang-format': '''BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: Empty
BreakBeforeBraces: Attach
PointerAlignment: Right
SortIncludes: true
IncludeBlocks: Preserve
SpaceAfterCStyleCast: false
SpacesInParentheses: false
''',
        '.clang-tidy': '''Checks: >
    bugprone-*,
    clang-analyzer-*,
    cppcoreguidelines-*,
    misc-*,
    modernize-*,
    performance-*,
    portability-*,
    readability-*

WarningsAsErrors: ''
HeaderFilterRegex: '.*'
AnalyzeTemporaryDtors: false
FormatStyle: file
''',
    }
}

def create_project(project_name: str, description: str, output_dir: str):
    """创建新项目"""
    project_path = Path(output_dir) / project_name
    project_path.mkdir(parents=True, exist_ok=True)

    # 创建目录
    for dir_template in PROJECT_TEMPLATE['directories']:
        dir_path = project_path / dir_template.format(project_name=project_name)
        dir_path.mkdir(parents=True, exist_ok=True)

    # 创建文件
    for file_template, content in PROJECT_TEMPLATE['files'].items():
        file_path = project_path / file_template.format(project_name=project_name)
        file_path.parent.mkdir(parents=True, exist_ok=True)

        formatted_content = content.format(
            project_name=project_name,
            PROJECT_NAME_UPPER=project_name.upper(),
            description=description
        )
        file_path.write_text(formatted_content)

    print(f"Project created at: {project_path}")
    print(f"cd {project_path} && mkdir build && cd build && cmake .. && make")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='C Project Scaffolder')
    parser.add_argument('name', help='Project name')
    parser.add_argument('-d', '--description', default='A C project',
                       help='Project description')
    parser.add_argument('-o', '--output', default='.',
                       help='Output directory')
    args = parser.parse_args()

    create_project(args.name, args.description, args.output)
```

---

## 2. CMake模板

### 2.1 现代CMake最佳实践模板

```cmake
# CMakeLists.txt - 现代CMake最佳实践示例
cmake_minimum_required(VERSION 3.14)

#[[
    现代CMake最佳实践:
    1. 使用 target_* 命令而非全局设置
    2. 明确声明 PUBLIC/PRIVATE/INTERFACE
    3. 使用生成器表达式
    4. 避免使用变量传递依赖
]]

project(ModernCMakeExample
    VERSION 1.0.0
    DESCRIPTION "现代CMake最佳实践示例"
    HOMEPAGE_URL "https://github.com/example/project"
    LANGUAGES C CXX
)

# 生成编译数据库供clangd使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 定义编译选项
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 选项定义
option(BUILD_SHARED_LIBS "构建共享库" OFF)
option(BUILD_TESTS "构建测试" ON)
option(BUILD_DOCS "构建文档" OFF)
option(ENABLE_WARNINGS_AS_ERRORS "将警告视为错误" OFF)
option(ENABLE_COVERAGE "启用代码覆盖率" OFF)

# 多配置生成器支持
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
endif()

# 编译器警告配置函数
function(set_target_warnings target)
    target_compile_options(${target} PRIVATE
        $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wcast-align
            -Wunused
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
        >
        $<$<C_COMPILER_ID:MSVC>:
            /W4
            /permissive-
        >
    )

    if(ENABLE_WARNINGS_AS_ERRORS)
        target_compile_options(${target} PRIVATE
            $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-Werror>
            $<$<C_COMPILER_ID:MSVC>:/WX>
        )
    endif()
endfunction()

# 库目标
add_library(math_utils)

target_sources(math_utils
    PRIVATE
        src/math_utils.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/math_utils.h
)

target_include_directories(math_utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_warnings(math_utils)

# 特性检测
include(CheckFunctionExists)
check_function_exists(sqrt HAVE_SQRT)
if(HAVE_SQRT)
    target_compile_definitions(math_utils PRIVATE HAVE_SQRT)
endif()

# 可执行文件目标
add_executable(calculator src/main.c)
target_link_libraries(calculator PRIVATE math_utils)
set_target_warnings(calculator)

# 测试
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装
include(GNUInstallDirs)
install(TARGETS math_utils calculator
    EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 导出配置
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# 包配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
```

### 2.2 跨平台CMake配置

```cmake
# cmake/Platform.cmake - 跨平台配置

# 平台检测
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    if(MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        add_definitions(-DNOMINMAX)
    endif()
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
elseif(UNIX)
    set(PLATFORM_LINUX TRUE)
endif()

# 架构检测
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_64BIT TRUE)
else()
    set(ARCH_32BIT TRUE)
endif()

# 编译器特性
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang 通用选项
    add_compile_options(
        -fno-strict-aliasing
        $<$<CONFIG:Debug>:-O0 -g3>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )

    # 链接时优化
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_SUPPORTED)
    if(IPO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    endif()

elseif(MSVC)
    # MSVC 选项
    add_compile_options(
        /utf-8
        $<$<CONFIG:Debug>:/Od /RTC1>
        $<$<CONFIG:Release>:/O2 /GL>
    )
endif()
```

---

## 3. 最佳实践模板

### 3.1 头文件模板

```c
/**
 * @file module_name.h
 * @brief 模块功能简述
 * @author Author Name
 * @date 2024
 * @copyright MIT License
 *
 * @details
 * 详细描述模块的功能、使用方法和注意事项。
 *
 * @section example_sec 使用示例
 * @code{.c}
 *     // 初始化模块
 *     if (module_init() != 0) {
 *         // 错误处理
 *     }
 *
 *     // 使用模块功能
 *     module_process(data, len);
 *
 *     // 清理资源
 *     module_cleanup();
 * @endcode
 */

#ifndef PROJECT_MODULE_NAME_H
#define PROJECT_MODULE_NAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* 版本信息 */
#define MODULE_NAME_VERSION_MAJOR 1
#define MODULE_NAME_VERSION_MINOR 0
#define MODULE_NAME_VERSION_PATCH 0
#define MODULE_NAME_VERSION "1.0.0"

/* 可见性属性 */
#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef MODULE_NAME_BUILD_SHARED
        #define MODULE_API __declspec(dllexport)
    #elif defined(MODULE_NAME_USE_SHARED)
        #define MODULE_API __declspec(dllimport)
    #else
        #define MODULE_API
    #endif
#elif __GNUC__ >= 4
    #define MODULE_API __attribute__((visibility("default")))
#else
    #define MODULE_API
#endif

/* 错误码定义 */
typedef enum {
    MODULE_OK = 0,
    MODULE_ERROR_INVALID_PARAM = -1,
    MODULE_ERROR_MEMORY = -2,
    MODULE_ERROR_IO = -3,
    MODULE_ERROR_NOT_INITIALIZED = -4,
    MODULE_ERROR_ALREADY_INITIALIZED = -5,
    MODULE_ERROR_NOT_SUPPORTED = -6
} module_error_t;

/* 模块配置 */
typedef struct {
    uint32_t buffer_size;
    uint32_t timeout_ms;
    bool enable_feature_x;
    void (*log_callback)(int level, const char *msg);
} module_config_t;

/* 模块句柄 */
typedef struct module_handle module_handle_t;

/**
 * @brief 获取默认配置
 * @param config 配置结构体指针
 * @return MODULE_OK 成功，否则返回错误码
 */
MODULE_API module_error_t module_get_default_config(module_config_t *config);

/**
 * @brief 初始化模块
 * @param config 配置指针，NULL使用默认配置
 * @param handle 输出句柄指针
 * @return MODULE_OK 成功，否则返回错误码
 */
MODULE_API module_error_t module_init(const module_config_t *config,
                                       module_handle_t **handle);

/**
 * @brief 清理模块资源
 * @param handle 模块句柄
 */
MODULE_API void module_cleanup(module_handle_t *handle);

/**
 * @brief 处理数据
 * @param handle 模块句柄
 * @param input 输入数据
 * @param input_len 输入长度
 * @param output 输出缓冲区
 * @param output_len 输出缓冲区大小
 * @param bytes_written 实际写入字节数
 * @return MODULE_OK 成功，否则返回错误码
 */
MODULE_API module_error_t module_process(module_handle_t *handle,
                                          const uint8_t *input,
                                          size_t input_len,
                                          uint8_t *output,
                                          size_t output_len,
                                          size_t *bytes_written);

/**
 * @brief 获取错误描述
 * @param error 错误码
 * @return 错误描述字符串
 */
MODULE_API const char *module_error_string(module_error_t error);

/**
 * @brief 获取模块版本
 * @return 版本字符串
 */
MODULE_API const char *module_version(void);

#ifdef __cplusplus
}
#endif

#endif /* PROJECT_MODULE_NAME_H */
```

### 3.2 源文件模板

```c
/**
 * @file module_name.c
 * @brief 模块实现
 */

#include "module_name.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* 内部数据结构 */
struct module_handle {
    module_config_t config;
    uint8_t *buffer;
    bool initialized;
    uint32_t ref_count;
};

/* 默认配置 */
static const module_config_t default_config = {
    .buffer_size = 4096,
    .timeout_ms = 5000,
    .enable_feature_x = false,
    .log_callback = NULL
};

/* 内部日志宏 */
#define LOG(handle, level, fmt, ...) do { \
    if ((handle) && (handle)->config.log_callback) { \
        char msg[256]; \
        snprintf(msg, sizeof(msg), fmt, ##__VA_ARGS__); \
        (handle)->config.log_callback(level, msg); \
    } \
} while(0)

module_error_t module_get_default_config(module_config_t *config)
{
    if (!config) {
        return MODULE_ERROR_INVALID_PARAM;
    }

    *config = default_config;
    return MODULE_OK;
}

module_error_t module_init(const module_config_t *config,
                            module_handle_t **handle)
{
    if (!handle) {
        return MODULE_ERROR_INVALID_PARAM;
    }

    *handle = calloc(1, sizeof(module_handle_t));
    if (!*handle) {
        return MODULE_ERROR_MEMORY;
    }

    /* 应用配置 */
    if (config) {
        (*handle)->config = *config;
    } else {
        (*handle)->config = default_config;
    }

    /* 分配缓冲区 */
    (*handle)->buffer = malloc((*handle)->config.buffer_size);
    if (!(*handle)->buffer) {
        free(*handle);
        *handle = NULL;
        return MODULE_ERROR_MEMORY;
    }

    (*handle)->initialized = true;
    (*handle)->ref_count = 1;

    LOG(*handle, 0, "Module initialized successfully");

    return MODULE_OK;
}

void module_cleanup(module_handle_t *handle)
{
    if (!handle) {
        return;
    }

    if (--handle->ref_count > 0) {
        return;
    }

    LOG(handle, 0, "Module cleanup");

    free(handle->buffer);
    free(handle);
}

module_error_t module_process(module_handle_t *handle,
                               const uint8_t *input,
                               size_t input_len,
                               uint8_t *output,
                               size_t output_len,
                               size_t *bytes_written)
{
    if (!handle || !input || !output || !bytes_written) {
        return MODULE_ERROR_INVALID_PARAM;
    }

    if (!handle->initialized) {
        return MODULE_ERROR_NOT_INITIALIZED;
    }

    if (input_len > output_len) {
        return MODULE_ERROR_INVALID_PARAM;
    }

    /* 实现数据处理逻辑 */
    /* 示例：简单复制输入数据到输出缓冲区 */
    (void)memcpy(output, input, input_len);
    *bytes_written = input_len;

    return MODULE_OK;
}

const char *module_error_string(module_error_t error)
{
    switch (error) {
        case MODULE_OK: return "Success";
        case MODULE_ERROR_INVALID_PARAM: return "Invalid parameter";
        case MODULE_ERROR_MEMORY: return "Memory allocation failed";
        case MODULE_ERROR_IO: return "I/O error";
        case MODULE_ERROR_NOT_INITIALIZED: return "Not initialized";
        case MODULE_ERROR_ALREADY_INITIALIZED: return "Already initialized";
        case MODULE_ERROR_NOT_SUPPORTED: return "Not supported";
        default: return "Unknown error";
    }
}

const char *module_version(void)
{
    return MODULE_NAME_VERSION;
}
```

---

## 4. CI/CD配置模板

### 4.1 GitHub Actions工作流

```yaml
# .github/workflows/ci.yml
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

env:
  BUILD_TYPE: Release

jobs:
  build-linux:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        compiler: [gcc, clang]

    steps:
    - uses: actions/checkout@v3

    - name: Configure CMake
      run: |
        cmake -B ${{github.workspace}}/build \
          -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}} \
          -DCMAKE_C_COMPILER=${{matrix.compiler}} \
          -DBUILD_TESTS=ON

    - name: Build
      run: cmake --build ${{github.workspace}}/build --config ${{env.BUILD_TYPE}}

    - name: Test
      working-directory: ${{github.workspace}}/build
      run: ctest -C ${{env.BUILD_TYPE}} --output-on-failure

  build-macos:
    runs-on: macos-latest
    steps:
    - uses: actions/checkout@v3
    - name: Configure
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}}
    - name: Build
      run: cmake --build build
    - name: Test
      run: ctest --test-dir build

  build-windows:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v3
    - name: Configure
      run: cmake -B build
    - name: Build
      run: cmake --build build --config ${{env.BUILD_TYPE}}
    - name: Test
      run: ctest --test-dir build -C ${{env.BUILD_TYPE}}

  static-analysis:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Run clang-tidy
      run: |
        cmake -B build -DCMAKE_C_CLANG_TIDY="clang-tidy"
        cmake --build build 2>&1 | tee clang-tidy-report.txt

    - name: Upload report
      uses: actions/upload-artifact@v3
      with:
        name: clang-tidy-report
        path: clang-tidy-report.txt
```

---

## 5. 使用指南

### 5.1 快速开始

```bash
# 1. 生成新项目
python3 project_scaffolder.py my_project -d "My awesome C project"

# 2. 进入项目目录
cd my_project

# 3. 创建构建目录
mkdir build && cd build

# 4. 配置
cmake .. -DCMAKE_BUILD_TYPE=Release

# 5. 构建
cmake --build . --parallel

# 6. 运行测试
ctest --output-on-failure

# 7. 运行程序
./my_project_app
```

### 5.2 项目维护命令

```bash
# 代码格式化
find src include tests -name "*.c" -o -name "*.h" | xargs clang-format -i

# 静态分析
clang-tidy src/*.c -- -I include

# 生成文档
cd docs && doxygen Doxyfile

# 代码覆盖率
cmake -B build -DENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build
lcov --capture --directory build --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

---

## 总结

项目模板是团队协作和工程效率的重要基础。通过标准化的目录结构、现代化的CMake配置、一致的代码风格和完善的CI/CD流程，可以显著提升C语言项目的开发效率和质量。建议根据团队实际需求定制模板，并持续迭代优化。


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
