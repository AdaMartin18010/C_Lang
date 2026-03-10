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
