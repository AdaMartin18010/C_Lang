# 现代C项目模板指南

> **层级定位**: 07 Modern Toolchain / 06 Project Templates
> **难度级别**: L2 应用
> **预估学习时间**: 2-3小时

---

## 📋 标准项目结构

```
myproject/
├── .github/workflows/       # CI/CD配置
├── .vscode/                 # VS Code配置
│   ├── settings.json
│   ├── tasks.json
│   └── launch.json
├── cmake/                   # CMake模块
│   └── FindXXX.cmake
├── docs/                    # 文档
├── include/                 # 公共头文件
│   └── myproject/
│       └── config.h
├── src/                     # 源文件
│   ├── main.c
│   └── utils.c
├── tests/                   # 测试代码
│   ├── test_main.c
│   └── CMakeLists.txt
├── third_party/             # 第三方依赖
├── .clang-format            # 格式化配置
├── .clang-tidy              # 静态分析配置
├── .gitignore
├── CMakeLists.txt           # 主CMake配置
├── conanfile.txt            # 依赖管理
├── LICENSE
└── README.md
```

---

## 🚀 CMake项目模板

### 完整CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)

project(MyProject
    VERSION 1.0.0
    DESCRIPTION "A modern C project"
    LANGUAGES C
)

# C标准

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

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 生成编译数据库
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 编译选项
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -O2
)

# 包含目录
include_directories(include)

# 源文件
file(GLOB SOURCES src/*.c)

# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE m)

# 测试
enable_testing()
add_subdirectory(tests)

# 安装
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
```

### 测试CMakeLists.txt

```cmake
enable_testing()

# 使用Unity测试框架
add_subdirectory(third_party/unity)

# 测试可执行文件
add_executable(test_myproject
    test_main.c
    test_utils.c
)

target_link_libraries(test_myproject PRIVATE unity)

# 添加测试
add_test(NAME MyTest COMMAND test_myproject)
```

---

## 🔧 配置文件

### .clang-format

```yaml
Language: C
BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 80
AllowShortFunctionsOnASingleLine: Empty
BreakBeforeBraces: Attach
PointerAlignment: Right
SortIncludes: true
IncludeBlocks: Regroup
```

### .gitignore

```gitignore
# Build
cmake-build-*/
build/
out/

# IDE
.vscode/
.idea/
*.iml

# OS
.DS_Store
Thumbs.db

# Compiled
*.o
*.a
*.so
*.exe

# Coverage
*.gcov
coverage/

# Conan
conan.lock
```

### conanfile.txt

```ini
[requires]
zlib/1.2.13

[generators]
CMakeDeps
CMakeToolchain

[options]
zlib:shared=False
```

---

## 📚 创建新项目步骤

```bash
# 1. 复制模板
cp -r project-template my-new-project
cd my-new-project

# 2. 修改项目名称
sed -i 's/MyProject/ActualProject/g' CMakeLists.txt

# 3. 安装依赖
conan install . --output-folder=build --build=missing

# 4. 构建
cmake --preset conan-release
cmake --build --preset conan-release

# 5. 测试
ctest --preset conan-release
```

---

## ✅ 模板使用清单

- [ ] 复制模板
- [ ] 修改项目名称
- [ ] 配置CI/CD
- [ ] 设置代码风格
- [ ] 添加初始代码
- [ ] 配置依赖管理
- [ ] 设置测试框架
- [ ] 编写README文档

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
