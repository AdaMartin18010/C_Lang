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
├── cmake/                   # CMake模块
├── docs/                    # 文档
├── include/                 # 公共头文件
│   └── myproject/
├── src/                     # 源文件
├── tests/                   # 测试代码
├── third_party/             # 第三方依赖
├── .clang-format            # 格式化配置
├── .gitignore
├── CMakeLists.txt           # 主CMake配置
├── LICENSE
└── README.md
```

---

## 🚀 CMake项目模板

### 完整CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)

project(MyProject
    VERSION 1.0.0
    DESCRIPTION "A modern C project"
    LANGUAGES C
)

# C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 生成编译数据库
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 编译选项
add_compile_options(-Wall -Wextra -Wpedantic)

# 子目录
add_subdirectory(src)
enable_testing()
add_subdirectory(tests)
```

---

## ✅ 模板使用清单

- [ ] 复制模板
- [ ] 修改项目名称
- [ ] 配置CI/CD
- [ ] 设置代码风格
- [ ] 添加初始代码

---

**← [返回工具链主页](../README.md)**
