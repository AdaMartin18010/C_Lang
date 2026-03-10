# Xmake构建系统指南

> **层级定位**: 07 Modern Toolchain / 02 Build Systems
> **难度级别**: L2 应用
> **预估学习时间**: 3-4小时

---

## 📋 Xmake简介

Xmake是国产现代化的C/C++构建系统：

- **简洁配置**: Lua语法，简单直观
- **自动依赖**: 自动下载和构建依赖
- **跨平台**: Windows/macOS/Linux
- **包管理**: 内置xrepo包管理器

---

## 🚀 快速开始

### 安装

```bash
# 官方脚本
curl -fsSL https://xmake.io/shget.text | bash

# macOS
brew install xmake

# Windows (scoop)
scoop install xmake
```

### 最小项目

**xmake.lua**:

```lua
-- 项目声明
target("myapp")
    set_kind("binary")
    add_files("src/*.c")
    add_includedirs("include")
```

**构建**:

```bash
xmake              # 构建
xmake run          # 运行
xmake test         # 测试
xmake install      # 安装
```

---

## 📚 核心语法

### 项目配置

```lua
-- 项目声明
set_project("myproject")
set_version("1.0.0")
set_languages("c11")

-- 编译选项
set_warnings("all", "error")
set_optimize("fastest")

-- 调试/发布模式
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
elseif is_mode("release") then
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
end
```

### 目标定义

```lua
target("myapp")
    set_kind("binary")           -- 可执行文件
    add_files("src/main.c")
    add_files("src/utils/*.c")
    add_includedirs("include")
    add_links("m", "pthread")
    add_defines("VERSION=1.0")
```

### 包管理

```lua
-- 使用xrepo包
add_requires("zlib 1.2.11", {optional = true})
add_requires("libcurl", {system = false})

target("myapp")
    add_packages("zlib", "libcurl")
```

---

## 🔧 高级特性

### 包管理集成

```bash
# 添加依赖
xmake require --add zlib

# 搜索包
xmake repo -s zlib

# 使用第三方包
add_repositories("myrepo https://github.com/myrepo/xmake-repo.git")
```

### 交叉编译

```bash
# 查看支持的平台
xmake show -l platforms

# 交叉编译
xmake f -p android --ndk=~/android-ndk-r20b
xmake
```

---

**← [返回工具链主页](../README.md)**
