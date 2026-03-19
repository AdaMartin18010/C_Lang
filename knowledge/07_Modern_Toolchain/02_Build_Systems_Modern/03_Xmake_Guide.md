# Xmake构建系统完全指南

> **层级定位**: 07 Modern Toolchain / 02 Build Systems
> **难度级别**: L2 应用 - L3 高级
> **预估学习时间**: 6-8小时
> **版本要求**: Xmake 2.8.0+

---

## 📑 目录

- [Xmake构建系统完全指南](#xmake构建系统完全指南)
  - [📑 目录](#-目录)
  - [1. Xmake简介与安装](#1-xmake简介与安装)
    - [1.1 Xmake简介](#11-xmake简介)
    - [1.2 安装Xmake](#12-安装xmake)
      - [1.2.1 快速安装脚本](#121-快速安装脚本)
      - [1.2.2 包管理器安装](#122-包管理器安装)
      - [1.2.3 源码编译安装](#123-源码编译安装)
      - [1.2.4 验证安装](#124-验证安装)
    - [1.3 第一个Xmake项目](#13-第一个xmake项目)
      - [1.3.1 创建项目结构](#131-创建项目结构)
      - [1.3.2 编写源文件](#132-编写源文件)
      - [1.3.3 编写xmake.lua](#133-编写xmakelua)
      - [1.3.4 构建和运行](#134-构建和运行)
  - [2. 基础语法与概念](#2-基础语法与概念)
    - [2.1 项目声明](#21-项目声明)
      - [2.1.1 set\_project - 项目名称](#211-set_project---项目名称)
      - [2.1.2 set\_version - 版本管理](#212-set_version---版本管理)
      - [2.1.3 项目级配置](#213-项目级配置)
    - [2.2 目标定义](#22-目标定义)
      - [2.2.1 target - 定义构建目标](#221-target---定义构建目标)
      - [2.2.2 set\_kind - 目标类型](#222-set_kind---目标类型)
      - [2.2.3 目标依赖关系](#223-目标依赖关系)
    - [2.3 源文件管理](#23-源文件管理)
      - [2.3.1 add\_files - 添加源文件](#231-add_files---添加源文件)
      - [2.3.2 按语言添加文件](#232-按语言添加文件)
      - [2.3.3 条件添加文件](#233-条件添加文件)
    - [2.4 头文件管理](#24-头文件管理)
      - [2.4.1 add\_includedirs - 添加头文件搜索路径](#241-add_includedirs---添加头文件搜索路径)
      - [2.4.2 add\_headerfiles - 安装头文件](#242-add_headerfiles---安装头文件)
    - [2.5 链接配置](#25-链接配置)
      - [2.5.1 add\_links - 链接库](#251-add_links---链接库)
      - [2.5.2 add\_linkdirs - 库搜索路径](#252-add_linkdirs---库搜索路径)
      - [2.5.3 add\_syslinks - 系统链接库](#253-add_syslinks---系统链接库)
    - [2.6 预处理定义](#26-预处理定义)
      - [2.6.1 add\_defines - 添加宏定义](#261-add_defines---添加宏定义)
      - [2.6.2 add\_undefines - 取消定义](#262-add_undefines---取消定义)
  - [3. 构建配置](#3-构建配置)
    - [3.1 编译选项](#31-编译选项)
      - [3.1.1 set\_warnings - 警告级别](#311-set_warnings---警告级别)
      - [3.1.2 set\_optimize - 优化级别](#312-set_optimize---优化级别)
      - [3.1.3 set\_languages - 语言标准](#313-set_languages---语言标准)
      - [3.1.4 set\_symbols - 调试符号](#314-set_symbols---调试符号)
      - [3.1.5 set\_strip - 去除符号](#315-set_strip---去除符号)
      - [3.1.6 add\_cflags/add\_cxflags - 编译器选项](#316-add_cflagsadd_cxflags---编译器选项)
    - [3.2 构建模式](#32-构建模式)
      - [3.2.1 标准构建模式配置](#321-标准构建模式配置)
      - [3.2.2 切换构建模式](#322-切换构建模式)
      - [3.2.3 自定义构建模式](#323-自定义构建模式)
    - [3.3 交叉编译](#33-交叉编译)
      - [3.3.1 查看支持的平台](#331-查看支持的平台)
      - [3.3.2 Android交叉编译](#332-android交叉编译)
      - [3.3.3 iOS交叉编译](#333-ios交叉编译)
      - [3.3.4 MinGW交叉编译](#334-mingw交叉编译)
      - [3.3.5 通用交叉编译](#335-通用交叉编译)
      - [3.3.6 工具链配置](#336-工具链配置)
    - [3.4 构建目录管理](#34-构建目录管理)
  - [4. 包管理集成](#4-包管理集成)
    - [4.1 xrepo包管理器简介](#41-xrepo包管理器简介)
    - [4.2 基本包操作](#42-基本包操作)
    - [4.3 在项目中使用包](#43-在项目中使用包)
      - [4.3.1 add\_requires - 声明依赖](#431-add_requires---声明依赖)
      - [4.3.2 常用包配置选项](#432-常用包配置选项)
      - [4.3.3 多包声明](#433-多包声明)
    - [4.4 常用包示例](#44-常用包示例)
      - [4.4.1 网络库](#441-网络库)
      - [4.4.2 图像处理](#442-图像处理)
      - [4.4.3 GUI框架](#443-gui框架)
      - [4.4.4 数据库](#444-数据库)
      - [4.4.5 测试框架](#445-测试框架)
    - [4.5 第三方仓库](#45-第三方仓库)
      - [4.5.1 添加自定义仓库](#451-添加自定义仓库)
      - [4.5.2 创建私有包](#452-创建私有包)
    - [4.6 包的高级用法](#46-包的高级用法)
      - [4.6.1 条件依赖](#461-条件依赖)
      - [4.6.2 包别名](#462-包别名)
      - [4.6.3 外部包集成](#463-外部包集成)
  - [5. 高级特性](#5-高级特性)
    - [5.1 规则系统 (Rules)](#51-规则系统-rules)
      - [5.1.1 内置规则](#511-内置规则)
      - [5.1.2 自定义规则](#512-自定义规则)
      - [5.1.3 规则回调函数](#513-规则回调函数)
    - [5.2 自定义选项 (Options)](#52-自定义选项-options)
      - [5.2.1 定义选项](#521-定义选项)
      - [5.2.2 命令行配置选项](#522-命令行配置选项)
    - [5.3 任务系统 (Tasks)](#53-任务系统-tasks)
      - [5.3.1 内置任务](#531-内置任务)
      - [5.3.2 自定义任务](#532-自定义任务)
    - [5.4 插件开发](#54-插件开发)
      - [5.4.1 创建插件](#541-创建插件)
    - [5.5 条件编译](#55-条件编译)
    - [5.6 脚本块](#56-脚本块)
  - [6. 实际项目示例](#6-实际项目示例)
    - [6.1 完整的多文件项目](#61-完整的多文件项目)
    - [6.2 与第三方库集成示例](#62-与第三方库集成示例)
      - [6.2.1 OpenGL项目](#621-opengl项目)
      - [6.2.2 Qt项目](#622-qt项目)
      - [6.2.3 Python扩展模块](#623-python扩展模块)
    - [6.3 跨平台配置示例](#63-跨平台配置示例)
    - [6.4 大型项目配置](#64-大型项目配置)
  - [7. 与其他构建系统对比](#7-与其他构建系统对比)
    - [7.1 Xmake vs Makefile](#71-xmake-vs-makefile)
    - [7.2 Xmake vs CMake](#72-xmake-vs-cmake)
    - [7.3 Xmake vs Meson](#73-xmake-vs-meson)
    - [7.4 迁移指南](#74-迁移指南)
      - [从Makefile迁移](#从makefile迁移)
      - [从CMake迁移](#从cmake迁移)
  - [8. 常见问题与解决方案](#8-常见问题与解决方案)
    - [8.1 安装问题](#81-安装问题)
      - [Q: Windows上安装后无法找到xmake命令](#q-windows上安装后无法找到xmake命令)
      - [Q: Linux上编译安装失败](#q-linux上编译安装失败)
    - [8.2 配置问题](#82-配置问题)
      - [Q: 找不到编译器](#q-找不到编译器)
      - [Q: 包安装失败](#q-包安装失败)
    - [8.3 构建问题](#83-构建问题)
      - [Q: 头文件找不到](#q-头文件找不到)
      - [Q: 链接错误](#q-链接错误)
      - [Q: Windows下LNK2019未解析外部符号](#q-windows下lnk2019未解析外部符号)
    - [8.4 性能优化](#84-性能优化)
      - [加速构建](#加速构建)
      - [减少重新配置时间](#减少重新配置时间)
    - [8.5 调试技巧](#85-调试技巧)
    - [8.6 最佳实践](#86-最佳实践)
      - [项目结构建议](#项目结构建议)
      - [配置模式建议](#配置模式建议)
      - [版本控制建议](#版本控制建议)
    - [8.7 获取更多帮助](#87-获取更多帮助)
  - [📚 附录](#-附录)
    - [A. 完整配置参考](#a-完整配置参考)
    - [B. 常用命令速查表](#b-常用命令速查表)

---

## 1. Xmake简介与安装

### 1.1 Xmake简介

Xmake是一个基于Lua的跨平台构建工具，专为C/C++项目设计，具有以下核心特性：

| 特性 | 说明 |
|------|------|
| **简洁语法** | 基于Lua的配置语法，比Makefile/CMake更简洁直观 |
| **自动依赖** | 内置包管理器xrepo，自动下载、构建依赖库 |
| **跨平台** | 支持Windows、macOS、Linux、Android、iOS、BSD等 |
| **智能构建** | 自动检测头文件依赖，支持增量编译 |
| **IDE支持** | 生成VS、VSCode、CLion、Xcode等项目文件 |
| **插件生态** | 丰富的插件系统，支持自定义扩展 |

**为什么选择Xmake？**

```
Make/CMake的问题：
- Makefile语法复杂，难以维护
- CMake脚本冗长，学习曲线陡峭
- 包管理依赖困难，需要手动处理

Xmake的解决方案：
- Lua语法简洁，易于阅读和维护
- 内置包管理，一行命令添加依赖
- 智能化构建，自动处理大部分配置
```

### 1.2 安装Xmake

#### 1.2.1 快速安装脚本

```bash
# 使用官方安装脚本（推荐）
curl -fsSL https://xmake.io/shget.text | bash

# 国内镜像加速
curl -fsSL https://xmake.io/shget.text | bash -s -- --china

# PowerShell (Windows)
Invoke-Expression (Invoke-Webrequest 'https://xmake.io/psget.text' -UseBasicParsing).Content
```

#### 1.2.2 包管理器安装

**macOS:**

```bash
# Homebrew
brew install xmake

# MacPorts
sudo port install xmake
```

**Windows:**

```powershell
# Scoop
scoop install xmake

# Winget
winget install xmake

# Chocolatey
choco install xmake
```

**Linux:**

```bash
# Ubuntu/Debian
sudo add-apt-repository ppa:xmake-io/xmake
sudo apt update
sudo apt install xmake

# Arch Linux
sudo pacman -S xmake

# Fedora
sudo dnf install xmake

# Alpine
apk add xmake
```

#### 1.2.3 源码编译安装

```bash
# 克隆仓库
git clone https://github.com/xmake-io/xmake.git
cd xmake

# 编译安装
./configure
make
sudo make install

# 或者使用xmake自举
cd core
xmake
```

#### 1.2.4 验证安装

```bash
# 检查版本
xmake --version
# 输出类似：xmake v2.8.5+20231101

# 查看帮助
xmake --help

# 查看详细帮助
xmake man
```

### 1.3 第一个Xmake项目

#### 1.3.1 创建项目结构

```
myproject/
├── xmake.lua          # 构建配置文件
├── src/
│   └── main.c         # 源文件
└── include/
    └── hello.h        # 头文件
```

#### 1.3.2 编写源文件

**include/hello.h:**

```c
#ifndef HELLO_H
#define HELLO_H

#include <stdio.h>

void say_hello(const char* name);

#endif
```

**src/main.c:**

```c
#include "hello.h"

void say_hello(const char* name) {
    printf("Hello, %s!\n", name);
}

int main(int argc, char** argv) {
    const char* name = (argc > 1) ? argv[1] : "World";
    say_hello(name);
    return 0;
}
```

#### 1.3.3 编写xmake.lua

```lua
-- 项目声明
set_project("myproject")
set_version("1.0.0")

-- 目标定义
target("hello")
    set_kind("binary")
    add_files("src/*.c")
    add_includedirs("include")
```

#### 1.3.4 构建和运行

```bash
# 配置项目（检测编译器和平台）
xmake f -c

# 构建项目
xmake

# 运行程序
xmake run
# 输出：Hello, World!

# 带参数运行
xmake run hello Alice
# 输出：Hello, Alice!
```

---

## 2. 基础语法与概念

### 2.1 项目声明

#### 2.1.1 set_project - 项目名称

```lua
-- 设置项目名称（必需）
set_project("myapp")

-- 设置项目版本
set_version("1.2.3")

-- 设置版本并指定构建类型
set_version("1.2.3", {build = "%Y%m%d%H%M"})
```

#### 2.1.2 set_version - 版本管理

```lua
-- 基本版本设置
set_version("2.0.0")

-- 带构建号的版本
set_version("2.0.0", {build = "%Y%m%d"})
-- 生成版本：2.0.0.20231101

-- 从文件读取版本
set_version("1.0.0", {build = "${git rev-parse --short HEAD}"})
```

#### 2.1.3 项目级配置

```lua
-- 设置最小Xmake版本
set_xmakever("2.8.0")

-- 设置项目描述
set_description("A sample project using Xmake")

-- 设置项目主页
set_homepage("https://github.com/user/myapp")

-- 设置许可证
set_license("MIT")
```

### 2.2 目标定义

#### 2.2.1 target - 定义构建目标

```lua
-- 定义一个名为myapp的目标
target("myapp")
    -- 目标配置...

-- 定义多个目标
target("app1")
    -- app1配置...

target("app2")
    -- app2配置...
```

#### 2.2.2 set_kind - 目标类型

```lua
-- 可执行文件
target("myapp")
    set_kind("binary")
    add_files("src/main.c")

-- 静态库
target("mylib")
    set_kind("static")
    add_files("src/lib/*.c")

-- 动态库/共享库
target("myshared")
    set_kind("shared")
    add_files("src/shared/*.c")

-- 对象库（仅编译，不链接）
target("myobjects")
    set_kind("object")
    add_files("src/obj/*.c")

-- 头文件库（仅头文件，无编译）
target("myheader")
    set_kind("headeronly")
    add_headerfiles("include/*.h")
```

#### 2.2.3 目标依赖关系

```lua
-- 定义库目标
target("mylib")
    set_kind("static")
    add_files("src/lib/*.c")

-- 定义可执行文件，依赖mylib
target("myapp")
    set_kind("binary")
    add_files("src/main.c")
    add_deps("mylib")  -- 声明依赖
```

### 2.3 源文件管理

#### 2.3.1 add_files - 添加源文件

```lua
target("myapp")
    set_kind("binary")

    -- 添加单个文件
    add_files("src/main.c")

    -- 使用通配符
    add_files("src/*.c")
    add_files("src/**/*.c")  -- 递归子目录

    -- 添加多个模式
    add_files("src/*.c", "src/*.cpp")

    -- 排除特定文件
    add_files("src/*.c|test.c")  -- 添加所有.c但排除test.c
    add_files("src/*.c|exclude/*.c")
```

#### 2.3.2 按语言添加文件

```lua
target("mixed")
    set_kind("binary")

    -- C源文件
    add_files("src/*.c")

    -- C++源文件
    add_files("src/*.cpp", "src/*.cc")

    -- Objective-C (macOS/iOS)
    add_files("src/*.m", "src/*.mm")

    -- CUDA
    add_files("src/*.cu")

    -- ASM
    add_files("src/*.asm", "src/*.S")
```

#### 2.3.3 条件添加文件

```lua
target("platform_app")
    set_kind("binary")
    add_files("src/common/*.c")

    -- Windows特定文件
    if is_plat("windows") then
        add_files("src/platform/win32/*.c")
        add_files("src/*.rc")  -- 资源文件
    end

    -- Linux特定文件
    if is_plat("linux") then
        add_files("src/platform/linux/*.c")
    end

    -- macOS特定文件
    if is_plat("macosx") then
        add_files("src/platform/macos/*.c", "src/platform/macos/*.m")
    end
```

### 2.4 头文件管理

#### 2.4.1 add_includedirs - 添加头文件搜索路径

```lua
target("myapp")
    set_kind("binary")
    add_files("src/*.c")

    -- 添加头文件搜索路径
    add_includedirs("include")
    add_includedirs("third_party/include")

    -- 添加系统头文件路径
    add_includedirs("/usr/local/include")

    -- 添加带访问级别的头文件路径
    add_includedirs("include", {public = true})  -- 公开，传递依赖
    add_includedirs("internal", {interface = true})  -- 仅接口
```

#### 2.4.2 add_headerfiles - 安装头文件

```lua
target("mylib")
    set_kind("static")
    add_files("src/*.c")
    add_includedirs("include")

    -- 指定要安装的头文件
    add_headerfiles("include/*.h")

    -- 保留目录结构安装
    add_headerfiles("include/(mylib/*.h)")
    -- 安装到：prefix/include/mylib/*.h

    -- 安装到指定前缀
    add_headerfiles("include/*.h", {prefixdir = "mylib"})
```

### 2.5 链接配置

#### 2.5.1 add_links - 链接库

```lua
target("myapp")
    set_kind("binary")
    add_files("src/*.c")

    -- 链接系统库
    add_links("pthread", "dl", "m")

    -- 链接Windows库
    if is_plat("windows") then
        add_links("kernel32", "user32", "gdi32")
    end

    -- 链接macOS框架
    if is_plat("macosx") then
        add_frameworks("Foundation", "Cocoa")
    end
```

#### 2.5.2 add_linkdirs - 库搜索路径

```lua
target("myapp")
    set_kind("binary")
    add_files("src/*.c")

    -- 添加库搜索路径
    add_linkdirs("lib")
    add_linkdirs("third_party/lib")

    -- 按平台添加
    if is_plat("windows") then
        add_linkdirs("lib/windows")
    elseif is_plat("linux") then
        add_linkdirs("lib/linux")
    end

    add_links("mylib")
```

#### 2.5.3 add_syslinks - 系统链接库

```lua
target("network_app")
    set_kind("binary")
    add_files("src/*.c")

    -- 添加系统链接库（会自动处理平台差异）
    add_syslinks("pthread")
    add_syslinks("socket", "nsl")  -- Solaris需要
```

### 2.6 预处理定义

#### 2.6.1 add_defines - 添加宏定义

```lua
target("myapp")
    set_kind("binary")
    add_files("src/*.c")

    -- 定义宏
    add_defines("VERSION=1.0")
    add_defines("DEBUG")
    add_defines("PLATFORM=\"Linux\"")

    -- 条件定义
    if is_mode("debug") then
        add_defines("DEBUG", "_DEBUG")
    else
        add_defines("NDEBUG")
    end

    -- 按平台定义
    add_defines("WINDOWS", {tools = "gcc", plat = "windows"})
```

#### 2.6.2 add_undefines - 取消定义

```lua
target("myapp")
    set_kind("binary")
    add_files("src/*.c")

    -- 取消预定义宏
    add_undefines("UNICODE")
```

---

## 3. 构建配置

### 3.1 编译选项

#### 3.1.1 set_warnings - 警告级别

```lua
-- 设置警告级别
set_warnings("none")        -- 关闭所有警告
set_warnings("less")        -- 少量警告
set_warnings("more")        -- 更多警告
set_warnings("all")         -- 所有警告
set_warnings("allextra")    -- 所有警告+额外警告
set_warnings("error")       -- 警告视为错误

-- 组合使用
set_warnings("all", "error")  -- 所有警告且视为错误
```

#### 3.1.2 set_optimize - 优化级别

```lua
-- 优化级别
set_optimize("none")        -- 无优化（调试友好）
set_optimize("fast")        -- 快速优化
set_optimize("faster")      -- 更快优化
set_optimize("fastest")     -- 最快优化（推荐发布版）
set_optimize("smallest")    -- 最小体积
set_optimize("aggressive")  -- 激进优化
```

#### 3.1.3 set_languages - 语言标准

```lua
-- C语言标准
set_languages("c89")
set_languages("c99")
set_languages("c11")
set_languages("c17")
set_languages("c23")
set_languages("gnu99")
set_languages("gnu11")

-- C++语言标准
set_languages("cxx98")
set_languages("cxx11")
set_languages("cxx14")
set_languages("cxx17")
set_languages("cxx20")
set_languages("cxx23")
set_languages("gnuxx11")
set_languages("gnuxx14")
set_languages("gnuxx17")
set_languages("gnuxx20")

-- 为特定目标设置
target("modern_cpp")
    set_kind("binary")
    set_languages("cxx20")
    add_files("src/*.cpp")
```

#### 3.1.4 set_symbols - 调试符号

```lua
-- 调试符号设置
set_symbols("none")         -- 无调试符号
set_symbols("debug")        -- 生成调试符号
set_symbols("hidden")       -- 隐藏符号（发布版推荐）
```

#### 3.1.5 set_strip - 去除符号

```lua
-- 去除符号信息
set_strip("none")           -- 不去除
set_strip("debug")          -- 仅去除调试符号
set_strip("all")            -- 去除所有符号
```

#### 3.1.6 add_cflags/add_cxflags - 编译器选项

```lua
target("myapp")
    set_kind("binary")
    add_files("src/*.c")

    -- GCC/Clang特定选项
    add_cflags("-Wall", "-Wextra", "-pedantic")
    add_cflags("-fPIC")  -- 位置无关代码
    add_cflags("-fvisibility=hidden")  -- 默认隐藏符号

    -- C++特定选项
    add_cxflags("-fno-rtti")           -- 禁用RTTI
    add_cxflags("-fno-exceptions")     -- 禁用异常
    add_cxflags("-std=c++17")          -- C++17标准

    -- 链接器选项
    add_ldflags("-Wl,--gc-sections")   -- 去除未使用段
    add_shflags("-shared")             -- 共享库选项

    -- 条件选项
    add_cflags("-fsanitize=address", {tools = "gcc", modes = "debug"})
```

### 3.2 构建模式

#### 3.2.1 标准构建模式配置

```lua
-- 项目顶部设置默认模式
set_modes("debug", "release", "releasedbg")

-- 详细的模式配置
if is_mode("debug") then
    -- 调试模式配置
    set_symbols("debug")
    set_optimize("none")
    set_strip("none")
    add_defines("DEBUG", "_DEBUG")

    -- 开启Address Sanitizer（可选）
    -- add_cflags("-fsanitize=address", "-fno-omit-frame-pointer")
    -- add_ldflags("-fsanitize=address")

elseif is_mode("release") then
    -- 发布模式配置（最小体积）
    set_symbols("hidden")
    set_optimize("smallest")
    set_strip("all")
    add_defines("NDEBUG")
    add_cflags("-fomit-frame-pointer")

elseif is_mode("releasedbg") then
    -- 带调试信息的发布模式
    set_symbols("debug")
    set_optimize("fastest")
    set_strip("none")
    add_defines("NDEBUG")

elseif is_mode("minsizerel") then
    -- 最小体积发布
    set_symbols("hidden")
    set_optimize("smallest")
    set_strip("all")
    add_defines("NDEBUG")
end
```

#### 3.2.2 切换构建模式

```bash
# 切换到调试模式
xmake f -m debug
xmake

# 切换到发布模式
xmake f -m release
xmake

# 查看当前模式
xmake f -v
```

#### 3.2.3 自定义构建模式

```lua
-- 定义自定义模式
set_modes("profile", "coverage")

-- Profile模式
if is_mode("profile") then
    set_symbols("debug")
    set_optimize("fast")
    add_defines("PROFILING")
    add_cflags("-pg")  -- gprof性能分析
    add_ldflags("-pg")
end

-- 代码覆盖率模式
if is_mode("coverage") then
    set_symbols("debug")
    set_optimize("none")
    add_cflags("--coverage")
    add_ldflags("--coverage")
end
```

### 3.3 交叉编译

#### 3.3.1 查看支持的平台

```bash
# 列出所有支持的平台
xmake show -l platforms

# 常见平台列表
- windows
- linux
- macosx
- android
- iphoneos
- mingw
- cross (通用交叉编译)
```

#### 3.3.2 Android交叉编译

```bash
# 使用Android NDK
xmake f -p android --ndk=~/Android/Sdk/ndk/25.1.8937393
xmake

# 指定ABI
xmake f -p android --ndk=~/ndk --arch=arm64-v8a

# 指定API级别
xmake f -p android --ndk=~/ndk --android_sdk=21
```

**xmake.lua配置:**

```lua
target("android_app")
    set_kind("shared")  -- Android使用共享库
    add_files("src/*.cpp")

    if is_plat("android") then
        add_cxflags("-fPIC")
        add_shflags("-shared")
        add_syslinks("log", "android")
    end
```

#### 3.3.3 iOS交叉编译

```bash
# iOS设备
xmake f -p iphoneos
xmake

# iOS模拟器
xmake f -p iphoneos --target=simulator
xmake

# 指定架构
xmake f -p iphoneos --arch=arm64
```

#### 3.3.4 MinGW交叉编译

```bash
# Windows上编译Linux程序（或使用MinGW-w64）
xmake f -p mingw
xmake

# 指定MinGW路径
xmake f -p mingw --mingw=/usr/x86_64-w64-mingw32
```

#### 3.3.5 通用交叉编译

```bash
# 使用交叉编译工具链
xmake f -p cross --sdk=/opt/cross-toolchain
xmake

# 指定交叉编译器
xmake f -p cross --cross=i686-pc-linux-gnu- --bin=/opt/cross/bin
```

#### 3.3.6 工具链配置

```lua
-- 在xmake.lua中定义自定义工具链
toolchain("my_cross_compiler")
    set_kind("standalone")
    set_sdkdir("/opt/cross-toolchain")
    set_bindir("/opt/cross-toolchain/bin")
    set_toolset("cc", "arm-linux-gnueabihf-gcc")
    set_toolset("cxx", "arm-linux-gnueabihf-g++")
    set_toolset("ld", "arm-linux-gnueabihf-ld")
    set_toolset("ar", "arm-linux-gnueabihf-ar")
    set_toolset("strip", "arm-linux-gnueabihf-strip")

-- 使用自定义工具链
target("embedded_app")
    set_kind("binary")
    set_toolchains("my_cross_compiler")
    add_files("src/*.c")
```

### 3.4 构建目录管理

```bash
# 指定构建输出目录
xmake f -o build

# 指定构建模式特定目录
xmake f -m debug -o build/debug
xmake f -m release -o build/release

# 清理构建
xmake clean

# 完全清理（包括配置）
xmake clean -a

# 重新配置
xmake f -c
```

---

## 4. 包管理集成

### 4.1 xrepo包管理器简介

Xrepo是Xmake内置的包管理器，支持：

- 超过1000+常用C/C++库
- 自动下载、编译、安装
- 多版本管理
- 跨平台支持

### 4.2 基本包操作

```bash
# 搜索包
xrepo search zlib
xrepo search "png*"

# 查看包信息
xrepo info zlib

# 安装包到全局仓库
xrepo install zlib
xrepo install zlib 1.2.13

# 安装并指定配置
xrepo install "zlib {configs={shared=true}}"

# 卸载包
xrepo remove zlib

# 列出已安装包
xrepo list

# 更新包仓库
xrepo update-repo
```

### 4.3 在项目中使用包

#### 4.3.1 add_requires - 声明依赖

```lua
-- 基本用法
add_requires("zlib")

-- 指定版本
add_requires("zlib 1.2.13")
add_requires("zlib >=1.2.0")
add_requires("zlib ~>1.2")  -- 兼容1.2.x的最新版本

-- 指定配置
add_requires("zlib", {configs = {shared = true}})
add_requires("openssl", {configs = {shared = true, vs_runtime = "MD"}})

-- 系统库优先
add_requires("zlib", {system = true})

-- 可选依赖
add_requires("zlib", {optional = true})

target("myapp")
    set_kind("binary")
    add_files("src/*.c")
    add_packages("zlib")  -- 链接zlib
```

#### 4.3.2 常用包配置选项

```lua
-- 静态库 vs 动态库
add_requires("zlib", {configs = {shared = false}})  -- 静态库
add_requires("zlib", {configs = {shared = true}})   -- 动态库

-- 调试版本
add_requires("zlib", {configs = {debug = true}})

-- C运行时库（MSVC）
add_requires("zlib", {configs = {vs_runtime = "MD"}})   -- 动态运行时
add_requires("zlib", {configs = {vs_runtime = "MT"}})   -- 静态运行时

-- PIC位置无关代码
add_requires("zlib", {configs = {pic = true}})

-- 多线程选项
add_requires("openssl", {configs = {threads = true}})
```

#### 4.3.3 多包声明

```lua
-- 声明多个依赖
add_requires("zlib", "libpng", "libjpeg")

-- 分别配置
add_requires("zlib 1.2.13")
add_requires("libpng ~>1.6", {configs = {shared = true}})
add_requires("libjpeg-turbo", {optional = true})

target("image_processor")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("zlib", "libpng", "libjpeg-turbo")
```

### 4.4 常用包示例

#### 4.4.1 网络库

```lua
-- HTTP客户端
add_requires("libcurl")

-- WebSocket
add_requires("websocketpp")
add_requires("uwebsockets")

-- gRPC
add_requires("grpc")

-- Boost.Asio
add_requires("boost", {configs = {asio = true}})
```

#### 4.4.2 图像处理

```lua
-- 图像格式
add_requires("libpng")
add_requires("libjpeg-turbo")
add_requires("libwebp")
add_requires("giflib")

-- 图像处理库
add_requires("stb")
add_requires("opencv")
```

#### 4.4.3 GUI框架

```lua
-- Qt
add_requires("qt5base", "qt5widgets")

-- Dear ImGui
add_requires("imgui")

-- GLFW
add_requires("glfw")

-- SDL
add_requires("sdl2")
```

#### 4.4.4 数据库

```lua
-- SQLite
add_requires("sqlite3")

-- Redis客户端
add_requires("hiredis")

-- MySQL
add_requires("mysql")
```

#### 4.4.5 测试框架

```lua
-- Google Test
add_requires("gtest")

-- Catch2
add_requires("catch2")

-- doctest
add_requires("doctest")

target("tests")
    set_kind("binary")
    add_files("tests/*.cpp")
    add_packages("gtest")
    add_tests("default")
```

### 4.5 第三方仓库

#### 4.5.1 添加自定义仓库

```lua
-- 添加Git仓库
add_repositories("myrepo https://github.com/user/xmake-repo.git")

-- 添加本地仓库
add_repositories("local-repo /path/to/local/repo")

-- 指定分支
add_repositories("myrepo https://github.com/user/xmake-repo.git dev")

-- 使用自定义仓库的包
add_requires("myrepo::mylib")
```

#### 4.5.2 创建私有包

**xmake.lua (包定义):**

```lua
package("mylib")
    set_homepage("https://github.com/user/mylib")
    set_description("My custom library")
    set_license("MIT")

    add_urls("https://github.com/user/mylib/archive/$(version).tar.gz")
    add_versions("1.0.0", "sha256_hash_here")

    -- 依赖
    add_deps("cmake")

    -- 安装逻辑
    on_install(function (package)
        local configs = {}
        configs["shared"] = package:config("shared")
        import("package.tools.cmake").install(package, configs)
    end)

    -- 测试
    on_test(function (package)
        assert(package:has_cfuncs("mylib_init", {includes = "mylib.h"}))
    end)
```

### 4.6 包的高级用法

#### 4.6.1 条件依赖

```lua
-- 平台特定依赖
if is_plat("windows") then
    add_requires("detours")
end

if is_plat("linux") then
    add_requires("libalsa")
end

-- 模式特定依赖
if is_mode("debug") then
    add_requires("debugbreak")
end
```

#### 4.6.2 包别名

```lua
-- 为包设置别名
add_requires("zlib", {alias = "compression"})

target("myapp")
    add_packages("compression")  -- 使用别名
```

#### 4.6.3 外部包集成

```lua
-- 集成系统包
add_requires("apt::zlib1g-dev", {alias = "zlib"})
add_requires("brew::zlib", {alias = "zlib"})
add_requires("vcpkg::zlib", {alias = "zlib"})
add_requires("conan::zlib/1.2.13", {alias = "zlib"})

target("myapp")
    add_packages("zlib")
```

---

## 5. 高级特性

### 5.1 规则系统 (Rules)

规则用于自定义构建过程，可以对特定文件应用自定义处理。

#### 5.1.1 内置规则

```lua
target("shader_app")
    set_kind("binary")
    add_files("src/*.cpp")

    -- 使用GLSL编译规则
    add_rules("utils.glsl2spv", {bin2c = true})
    add_files("shaders/*.vert", "shaders/*.frag")

    -- 使用Qt规则
    add_rules("qt.console")
    add_files("src/*.cpp")
```

#### 5.1.2 自定义规则

```lua
-- 定义自定义规则
rule("protobuf_cpp")
    -- 设置扩展名关联
    set_extensions(".proto")

    -- 编译前执行
    before_build_file(function (target, sourcefile)
        -- 生成输出文件名
        local outputdir = path.join(target:autogendir(), "rules", "protobuf")
        local grpc_cpp = path.join(outputdir, path.basename(sourcefile) .. ".pb.cc")
        local grpc_h = path.join(outputdir, path.basename(sourcefile) .. ".pb.h")

        -- 创建输出目录
        if not os.isdir(outputdir) then
            os.mkdir(outputdir)
        end

        -- 执行protoc
        os.exec("protoc --cpp_out=%s %s", outputdir, sourcefile)

        -- 添加生成的文件到编译
        target:add("files", grpc_cpp)
        target:add("includedirs", outputdir)
    end)

-- 使用规则
target("myproto")
    set_kind("static")
    add_rules("protobuf_cpp")
    add_files("proto/*.proto")
```

#### 5.1.3 规则回调函数

```lua
rule("custom_rule")
    -- 加载时
    on_load(function (target)
        print("Rule loaded for target: " .. target:name())
    end)

    -- 配置时
    on_config(function (target)
        print("Configuring target: " .. target:name())
    end)

    -- 编译文件前
    before_build_file(function (target, sourcefile)
        print("Building file: " .. sourcefile)
    end)

    -- 编译文件后
    after_build_file(function (target, sourcefile)
        print("File built: " .. sourcefile)
    end)

    -- 链接前
    before_link(function (target)
        print("Before link")
    end)

    -- 链接后
    after_link(function (target)
        print("After link")
    end)

    -- 清理时
    on_clean(function (target)
        print("Cleaning target: " .. target:name())
    end)
```

### 5.2 自定义选项 (Options)

#### 5.2.1 定义选项

```lua
-- 定义选项
option("enable_tests")
    set_default(false)
    set_showmenu(true)
    set_description("Enable unit tests")
    add_defines("ENABLE_TESTS")

option("use_cuda")
    set_default(false)
    set_showmenu(true)
    set_description("Enable CUDA support")

    -- 选项启用时的配置
    add_defines("USE_CUDA")
    add_links("cudart")
    add_includedirs("/usr/local/cuda/include")
    add_linkdirs("/usr/local/cuda/lib64")

option("cuda_path")
    set_default("/usr/local/cuda")
    set_showmenu(true)
    set_description("CUDA installation path")

-- 使用选项
if has_config("enable_tests") then
    target("unit_tests")
        set_kind("binary")
        add_files("tests/*.cpp")
        add_packages("gtest")
end

if has_config("use_cuda") then
    target("cuda_kernel")
        set_kind("object")
        add_files("src/*.cu")
end
```

#### 5.2.2 命令行配置选项

```bash
# 查看所有选项
xmake f --help

# 启用选项
xmake f --enable_tests=y
xmake f --enable_tests=true

# 禁用选项
xmake f --enable_tests=n

# 设置路径选项
xmake f --cuda_path=/opt/cuda-11.8

# 查看当前配置
xmake f -v
```

### 5.3 任务系统 (Tasks)

#### 5.3.1 内置任务

```bash
# 运行项目
xmake run [target] [args...]

# 安装项目
xmake install
xmake install -o /usr/local

# 打包项目
xmake package

# 卸载
xmake uninstall

# 生成IDE项目文件
xmake project -k vs2022      # Visual Studio 2022
xmake project -k cmake       # CMakeLists.txt
xmake project -k make        # Makefile
xmake project -k compile_commands  # compile_commands.json
```

#### 5.3.2 自定义任务

```lua
-- 定义自定义任务
task("hello")
    set_category("plugin")
    on_run(function ()
        print("Hello from custom task!")
        print("Xmake version: " .. xmake.version())
    end)

-- 带参数的任务
task("deploy")
    set_category("plugin")
    on_run(function ()
        import("core.base.option")

        -- 获取命令行参数
        local server = option.get("server") or "localhost"
        local port = option.get("port") or "8080"

        print("Deploying to " .. server .. ":" .. port)

        -- 执行部署命令
        os.exec("scp build/myapp user@%s:/opt/app/", server)
        os.exec("ssh user@%s 'sudo systemctl restart myapp'", server)
    end)

    set_menu {
        usage = "xmake deploy [options]",
        description = "Deploy application to server",
        options = {
            {'s', "server", "kv", "localhost", "Target server address"},
            {'p', "port", "kv", "8080", "Server port"}
        }
    }
```

执行自定义任务:

```bash
xmake hello
xmake deploy --server=192.168.1.100 --port=3000
```

### 5.4 插件开发

#### 5.4.1 创建插件

**插件目录结构:**

```
~/.xmake/plugins/
└── myplugin/
    ├── xmake.lua      # 插件定义
    └── main.lua       # 插件入口
```

**main.lua:**

```lua
import("core.base.option")
import("core.project.project")

function main()
    -- 获取参数
    local targetname = option.get("target")

    -- 获取项目信息
    print("Project name: " .. project.name())
    print("Project dir: " .. os.projectdir())

    -- 遍历所有目标
    for _, target in ipairs(project.targets()) do
        print("Target: " .. target:name())
        print("  Kind: " .. target:get("kind"))
        print("  Files: " .. table.concat(target:sourcefiles(), ", "))
    end

    -- 执行自定义操作
    if targetname then
        local target = project.target(targetname)
        if target then
            print("\nAnalyzing target: " .. targetname)
            -- 分析目标...
        end
    end
end
```

**xmake.lua:**

```lua
task("analyze")
    set_category("plugin")
    on_run("main")
    set_menu {
        usage = "xmake analyze [options]",
        description = "Analyze project targets",
        options = {
            {'t', "target", "kv", nil, "Target name to analyze"}
        }
    }
```

### 5.5 条件编译

```lua
-- 平台判断
if is_plat("windows") then
    add_defines("WINDOWS")
elseif is_plat("linux") then
    add_defines("LINUX")
elseif is_plat("macosx") then
    add_defines("MACOS")
end

-- 架构判断
if is_arch("x86_64", "x64") then
    add_defines("ARCH_64BIT")
elseif is_arch("i386", "x86") then
    add_defines("ARCH_32BIT")
elseif is_arch("arm64", "aarch64") then
    add_defines("ARCH_ARM64")
end

-- 模式判断
if is_mode("debug") then
    add_defines("DEBUG")
else
    add_defines("RELEASE")
end

-- 主机平台判断
if is_host("windows") then
    -- Windows上的构建主机
end

-- 检查配置
if has_config("use_cuda") then
    add_files("src/*.cu")
end

-- 检查选项
if has_option("enable_tests") then
    -- ...
end
```

### 5.6 脚本块

```lua
target("script_example")
    set_kind("binary")
    add_files("src/*.c")

    -- 加载时脚本
    on_load(function (target)
        print("Loading target: " .. target:name())
        -- 动态修改目标配置
        target:add("defines", "DYNAMIC_DEFINE")
    end)

    -- 配置时脚本
    on_config(function (target)
        print("Configuring target")
    end)

    -- 编译前
    before_build(function (target)
        print("Before build")
        -- 生成版本信息文件
        local git_hash = os.iorun("git rev-parse --short HEAD"):trim()
        io.writefile("src/version.h",
            "#define GIT_HASH \"" .. git_hash .. "\"\\n")
    end)

    -- 编译后
    after_build(function (target)
        print("Build completed: " .. target:targetfile())
        -- 复制资源文件
        os.cp("assets/*", path.directory(target:targetfile()))
    end)

    -- 自定义运行
    on_run(function (target)
        print("Running: " .. target:targetfile())
        os.execv(target:targetfile(), {"arg1", "arg2"})
    end)

    -- 自定义安装
    on_install(function (target)
        os.cp(target:targetfile(), "/usr/local/bin/")
        os.cp("assets/", "/usr/local/share/myapp/")
    end)

    -- 自定义卸载
    on_uninstall(function (target)
        os.rm("/usr/local/bin/" .. target:name())
        os.rm("/usr/local/share/myapp/")
    end)

    -- 自定义打包
    on_package(function (target)
        local basename = target:name() .. "-" .. os.host()
        os.exec("tar -czf %s.tar.gz %s", basename, target:targetfile())
    end)

    -- 自定义清理
    on_clean(function (target)
        print("Custom clean")
        os.rm("generated/*")
    end)
```

---

## 6. 实际项目示例

### 6.1 完整的多文件项目

**项目结构:**

```
myproject/
├── xmake.lua
├── include/
│   ├── utils.h
│   └── config.h.in
├── src/
│   ├── main.c
│   ├── utils.c
│   ├── math/
│   │   ├── vector.c
│   │   └── matrix.c
│   └── io/
│       ├── file.c
│       └── network.c
├── tests/
│   ├── test_utils.c
│   └── test_math.c
└── docs/
    └── README.md
```

**xmake.lua:**

```lua
-- 项目声明
set_project("myproject")
set_version("1.0.0", {build = "%Y%m%d"})
set_xmakever("2.8.0")

-- 构建模式
set_modes("debug", "release", "coverage")

-- 依赖
add_requires("zlib")
if is_mode("debug") then
    add_requires("cmocka")
end

-- 默认编译选项
set_languages("c11")
set_warnings("all", "error")

-- 调试模式配置
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    add_defines("DEBUG", "_DEBUG")
    set_policy("build.sanitizer.address", true)
elseif is_mode("release") then
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
    add_defines("NDEBUG")
elseif is_mode("coverage") then
    set_symbols("debug")
    set_optimize("none")
    add_cflags("--coverage")
    add_ldflags("--coverage")
end

-- 工具库目标
target("mylib")
    set_kind("static")
    add_files("src/utils.c")
    add_files("src/math/*.c")
    add_files("src/io/*.c")
    add_includedirs("include", {public = true})
    add_packages("zlib", {public = true})

    -- 版本配置头文件
    set_configdir("include")
    set_configvar("VERSION", "1.0.0")
    set_configvar("GIT_COMMIT", os.iorun("git rev-parse --short HEAD"):trim())
    add_configfiles("include/config.h.in")

-- 可执行文件目标
target("myapp")
    set_kind("binary")
    add_files("src/main.c")
    add_deps("mylib")

    if is_plat("windows") then
        add_files("src/win32/*.c")
        add_syslinks("ws2_32")
    elseif is_plat("linux") then
        add_files("src/linux/*.c")
        add_syslinks("pthread", "dl")
    end

    -- 构建后复制资源
    after_build(function (target)
        os.cp("assets", path.directory(target:targetfile()))
    end)

-- 测试目标
if is_mode("debug") then
    target("test_runner")
        set_kind("binary")
        add_files("tests/*.c")
        add_deps("mylib")
        add_packages("cmocka")
        add_tests("default")
end

-- 安装规则
target("install_files")
    set_kind("phony")
    on_install(function (target)
        os.cp("docs", path.join(target:installdir(), "share", "myproject"))
    end)
```

**include/config.h.in:**

```c
#ifndef CONFIG_H
#define CONFIG_H

#define PROJECT_VERSION "${VERSION}"
#define PROJECT_GIT_COMMIT "${GIT_COMMIT}"
#define PROJECT_BUILD_DATE __DATE__ " " __TIME__

#endif
```

### 6.2 与第三方库集成示例

#### 6.2.1 OpenGL项目

```lua
add_requires("glfw", "glad", "glm")

target("gl_app")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("glfw", "glad", "glm")

    if is_plat("windows") then
        add_syslinks("opengl32", "gdi32", "user32", "shell32")
    elseif is_plat("linux") then
        add_syslinks("GL", "X11", "pthread", "dl")
    elseif is_plat("macosx") then
        add_frameworks("OpenGL", "Cocoa", "IOKit", "CoreFoundation")
    end
```

#### 6.2.2 Qt项目

```lua
add_rules("qt.widgetapp")
add_requires("qt5base", "qt5widgets", "qt5core")

target("qt_demo")
    add_rules("qt.moc")
    set_kind("binary")

    add_files("src/*.cpp")
    add_files("src/*.h")
    add_files("resources/*.qrc")
    add_files("ui/*.ui")

    add_packages("qt5base", "qt5widgets", "qt5core")
    add_includedirs("include")
```

#### 6.2.3 Python扩展模块

```lua
add_requires("python 3.x")

target("mymodule")
    set_kind("shared")
    set_prefixname("")  -- 移除lib前缀
    set_extension(".pyd")  -- Windows使用.pyd

    add_files("src/*.c")
    add_packages("python")

    if is_plat("windows") then
        set_extension(".pyd")
    else
        set_extension(".so")
    end
```

### 6.3 跨平台配置示例

```lua
-- 跨平台音频播放器
set_project("audio_player")
set_version("2.0.0")

add_requires("miniaudio")  -- 跨平台音频库

target("audio_player")
    set_kind("binary")
    add_files("src/main.c")
    add_files("src/audio/*.c")
    add_packages("miniaudio")

    -- 平台特定配置
    if is_plat("windows") then
        add_files("src/platform/win32/*.c")
        add_syslinks("ole32", "avrt", "winmm")
        add_defines("UNICODE", "_UNICODE")
        add_cxflags("/utf-8")  -- UTF-8源文件

        -- Windows资源文件
        add_files("resources/*.rc")

    elseif is_plat("linux") then
        add_files("src/platform/linux/*.c")
        add_syslinks("pthread", "dl", "m", "asound")

        -- 检查ALSA
        if not os.isfile("/usr/include/alsa/asoundlib.h") then
            print("Warning: ALSA development headers not found")
            print("Install: sudo apt-get install libasound2-dev")
        end

    elseif is_plat("macosx") then
        add_files("src/platform/macos/*.c", "src/platform/macos/*.m")
        add_frameworks("CoreAudio", "AudioUnit", "CoreFoundation")
        add_cflags("-fobjc-arc")  -- ARC for Objective-C

    elseif is_plat("android") then
        set_kind("shared")  -- Android需要共享库
        add_files("src/platform/android/*.c")
        add_syslinks("OpenSLES", "log", "android")
        add_cflags("-fPIC")
        add_shflags("-shared")
    end

    -- 通用配置
    set_languages("c11")
    add_includedirs("include")

    -- 安装配置
    if is_plat("linux", "macosx") then
        on_install(function (target)
            os.cp(target:targetfile(), "/usr/local/bin/")
        end)
    end
```

### 6.4 大型项目配置

```lua
-- 大型项目：游戏引擎
set_project("game_engine")
set_version("0.1.0")

-- 子目录包含
includes("engine", "editor", "tools", "tests")

-- 全局选项
option("with_editor")
    set_default(true)
    set_showmenu(true)
    set_description("Build editor")

option("with_tests")
    set_default(false)
    set_showmenu(true)
    set_description("Build unit tests")

-- 全局依赖
add_requires("spdlog")
add_requires("glm")
add_requires("stb")
add_requires("glfw", {configs = {shared = true}})
```

**engine/xmake.lua:**

```lua
target("engine_core")
    set_kind("static")
    add_files("core/*.cpp")
    add_includedirs("include", {public = true})
    add_packages("spdlog", "glm", {public = true})

target("engine_render")
    set_kind("static")
    add_files("render/*.cpp")
    add_deps("engine_core")
    add_packages("glfw")

    if is_plat("windows") then
        add_syslinks("d3d11", "dxgi")
    end

target("engine")
    set_kind("shared")
    add_deps("engine_core", "engine_render")
```

**editor/xmake.lua:**

```lua
if has_config("with_editor") then
    add_requires("imgui")

    target("editor")
        set_kind("binary")
        add_files("*.cpp")
        add_deps("engine")
        add_packages("imgui")
end
```

---

## 7. 与其他构建系统对比

### 7.1 Xmake vs Makefile

| 特性 | Makefile | Xmake |
|------|----------|-------|
| 语法复杂度 | 复杂，需要掌握make规则 | 简单，Lua脚本 |
| 跨平台 | 需要条件判断 | 原生支持 |
| 依赖管理 | 手动处理 | 内置xrepo |
| 增量编译 | 需要手动配置 | 自动检测 |
| 学习曲线 | 陡峭 | 平缓 |

**Makefile示例:**

```makefile
CC=gcc
CFLAGS=-Wall -O2
TARGET=myapp
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
 $(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
 $(CC) $(CFLAGS) -c $< -o $@

clean:
 rm -f $(OBJS) $(TARGET)
```

**Xmake等价配置:**

```lua
target("myapp")
    set_kind("binary")
    set_warnings("all")
    set_optimize("fast")
    add_files("src/*.c")
```

### 7.2 Xmake vs CMake

| 特性 | CMake | Xmake |
|------|-------|-------|
| 配置语法 | 专用脚本语言 | Lua |
| 代码量 | 较多 | 精简 |
| 包管理 | 需要find_package | 内置xrepo |
| 生成IDE项目 | 支持 | 支持 |
| 社区生态 | 非常成熟 | 快速增长 |
| 构建速度 | 较慢（需先生成） | 直接构建 |

**CMake示例:**

```cmake
cmake_minimum_required(VERSION 3.10)
project(myapp VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 查找包
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

# 添加可执行文件
add_executable(myapp src/main.c)

target_include_directories(myapp PRIVATE include)

target_link_libraries(myapp
    PRIVATE
        ZLIB::ZLIB
        PNG::PNG
)

# 安装
install(TARGETS myapp DESTINATION bin)
```

**Xmake等价配置:**

```lua
add_requires("zlib", "libpng")

target("myapp")
    set_kind("binary")
    set_languages("c11")
    add_files("src/*.c")
    add_includedirs("include")
    add_packages("zlib", "libpng")
```

### 7.3 Xmake vs Meson

| 特性 | Meson | Xmake |
|------|-------|-------|
| 配置语法 | 自定义DSL | Lua |
| 可读性 | 好 | 好 |
| 包管理 | wrap子项目 | 内置xrepo |
| 构建后端 | Ninja | 内置 |
| 跨编译 | 配置较复杂 | 简单 |

### 7.4 迁移指南

#### 从Makefile迁移

```bash
# 1. 保留原有Makefile作为参考
# 2. 创建xmake.lua，分析Makefile中的：
#    - 编译器选项 (CC, CFLAGS)
#    - 源文件列表 (SRCS)
#    - 链接库 (LDFLAGS, LIBS)
#    - 包含路径 (CPPFLAGS)

# 3. 编写xmake.lua
set_toolchain("gcc", "clang")
set_languages("gnu11")

# 4. 逐步测试
xmake f -c
xmake
```

#### 从CMake迁移

```bash
# 1. 使用xmake的cmake导入（临时方案）
includes("cmake.lua")

# 2. 或者手动转换
# 分析CMakeLists.txt中的关键配置：
# - project() -> set_project()
# - add_executable() -> target() + set_kind("binary")
# - add_library() -> target() + set_kind("static/shared")
# - target_link_libraries() -> add_packages(), add_links()
# - find_package() -> add_requires()
```

---

## 8. 常见问题与解决方案

### 8.1 安装问题

#### Q: Windows上安装后无法找到xmake命令

```powershell
# 解决方案1：添加环境变量
[Environment]::SetEnvironmentVariable(
    "Path",
    $env:Path + ";C:\Program Files\xmake",
    "User"
)

# 解决方案2：使用 scoop 安装会自动配置PATH
scoop install xmake
```

#### Q: Linux上编译安装失败

```bash
# 检查依赖
sudo apt-get install build-essential libreadline-dev

# 或者使用预编译版本
curl -fsSL https://xmake.io/shget.text | bash
```

### 8.2 配置问题

#### Q: 找不到编译器

```bash
# 指定编译器
xmake f --cc=gcc --cxx=g++

# 或者使用工具链
xmake f --toolchain=clang

# 查看可用工具链
xmake show -l toolchains
```

#### Q: 包安装失败

```bash
# 详细输出查看错误
xmake f -vD

# 更新包仓库
xrepo update-repo

# 清除包缓存重试
xrepo remove --all
xmake f -c

# 使用系统库
add_requires("zlib", {system = true})
```

### 8.3 构建问题

#### Q: 头文件找不到

```lua
-- 确保添加正确的包含路径
target("myapp")
    add_includedirs("include")           -- 相对路径
    add_includedirs("/usr/local/include") -- 绝对路径
    add_includedirs("$(buildir)/generated") -- 使用变量

    -- 传递包含路径给依赖目标
    add_includedirs("include", {public = true})
```

#### Q: 链接错误

```lua
-- 确保链接顺序正确（依赖库在后）
target("myapp")
    add_deps("mylib")      -- 先链接内部库
    add_packages("zlib")    -- 再链接包
    add_links("pthread")    -- 最后系统库

    -- 或者强制链接顺序
    add_linkorders("mylib", "zlib", "pthread")
```

#### Q: Windows下LNK2019未解析外部符号

```lua
-- 检查运行时库设置
add_requires("mylib", {configs = {vs_runtime = "MD"}})

-- 确保与项目一致
set_runtimes("MD")  -- 或 "MT", "MDd", "MTd"
```

### 8.4 性能优化

#### 加速构建

```bash
# 使用并行构建
xmake -j8

# 启用ccache
xmake f --ccache=y

# 启用distcc分布式编译
xmake f --distcc=y

# 使用预编译头
set_pcxxheader("src/pch.h")
```

#### 减少重新配置时间

```lua
-- 缓存配置结果
set_configvar("CACHE_VAR", "value")

-- 使用option缓存检测
option("has_feature")
    add_csnippets("has_feature", "int main(){return 0;}")
```

### 8.5 调试技巧

```bash
# 详细输出
xmake -v

# 调试输出
xmake -vD

# 查看构建命令
xmake -r --dry-run

# 查看目标详情
xmake show -t mytarget

# 查看编译数据库
xmake project -k compile_commands
```

### 8.6 最佳实践

#### 项目结构建议

```
myproject/
├── xmake.lua           # 根配置
├── xmake.lua.template  # 配置模板
├── src/
│   ├── xmake.lua      # 子目录配置（可选）
│   ├── *.cpp
│   └── *.h
├── tests/
│   └── xmake.lua
├── third_party/       # 本地第三方库
└── build/            # 构建输出（.gitignore）
```

#### 配置模式建议

```lua
-- 始终定义模式
set_modes("debug", "release", "releasedbg")

-- 使用option进行功能开关
option("enable_feature_x")
    set_default(false)
    set_showmenu(true)

-- 清晰的依赖声明
add_requires("dep1", {optional = true})
add_requires("dep2", {system = true})

-- 完善的文档注释
--[[
    目标：核心引擎库
    依赖：zlib, opengl
    平台：Windows, Linux, macOS
--]]
target("engine")
    -- ...
```

#### 版本控制建议

**.gitignore:**

```gitignore
# Xmake
.xmake/
build/
vsxmake*

# 生成的文件
include/config.h
src/version.cpp

# IDE
.vscode/
.idea/
*.sln
*.vcxproj*
```

### 8.7 获取更多帮助

```bash
# 查看帮助
xmake --help
xmake <command> --help

# 官方文档
# https://xmake.io/#/zh-cn/

# 中文手册
# https://xmake.io/#/zh-cn/manual/project_scope

# 社区支持
# GitHub Issues: https://github.com/xmake-io/xmake/issues
# Telegram: https://t.me/tbooxorg
# Discord: https://discord.gg/MwY8xFQ
```

---

## 📚 附录

### A. 完整配置参考

```lua
-- 完整的xmake.lua模板
set_project("template")
set_version("1.0.0")
set_xmakever("2.8.0")
set_description("Project description")
set_license("MIT")

-- 模式
set_modes("debug", "release", "releasedbg")

-- 依赖
add_requires("zlib")

-- 全局选项
set_languages("c11", "cxx17")
set_warnings("all", "error")

-- 模式配置
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    add_defines("DEBUG")
elseif is_mode("release") then
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
    add_defines("NDEBUG")
end

-- 目标
target("template")
    set_kind("binary")
    add_files("src/*.c")
    add_includedirs("include")
    add_packages("zlib")
```

### B. 常用命令速查表

| 命令 | 说明 |
|------|------|
| `xmake` | 构建项目 |
| `xmake f -c` | 重新配置 |
| `xmake -r` | 强制重建 |
| `xmake clean` | 清理构建 |
| `xmake run` | 运行目标 |
| `xmake test` | 运行测试 |
| `xmake install` | 安装项目 |
| `xmake package` | 打包项目 |
| `xmake require` | 管理依赖 |
| `xmake show` | 查看信息 |
| `xmake create` | 创建项目模板 |

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
