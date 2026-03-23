# Emscripten 高级使用指南

## 目录

- [Emscripten 高级使用指南](#emscripten-高级使用指南)
  - [目录](#目录)
  - [1. Emscripten 安装和配置](#1-emscripten-安装和配置)
    - [1.1 系统要求](#11-系统要求)
    - [1.2 完整安装流程](#12-完整安装流程)
      - [1.2.1 Windows 安装](#121-windows-安装)
      - [1.2.2 macOS 安装](#122-macos-安装)
      - [1.2.3 Linux 安装](#123-linux-安装)
      - [1.2.4 Docker 安装](#124-docker-安装)
    - [1.3 多版本管理](#13-多版本管理)
    - [1.4 配置环境变量](#14-配置环境变量)
    - [1.5 验证安装和诊断](#15-验证安装和诊断)
  - [2. 编译选项详解](#2-编译选项详解)
    - [2.1 优化级别详解](#21-优化级别详解)
    - [2.2 内存模型选项](#22-内存模型选项)
    - [2.3 导出配置选项](#23-导出配置选项)
    - [2.4 环境相关选项](#24-环境相关选项)
    - [2.5 调试和分析选项](#25-调试和分析选项)
  - [3. EMSCRIPTEN\_KEEPALIVE 宏](#3-emscripten_keepalive-宏)
    - [3.1 宏的工作原理](#31-宏的工作原理)
    - [3.2 基本使用模式](#32-基本使用模式)
    - [3.3 编译和调用](#33-编译和调用)
  - [4. JavaScript 胶水代码生成](#4-javascript-胶水代码生成)
    - [4.1 胶水代码结构](#41-胶水代码结构)
    - [4.2 自定义胶水代码](#42-自定义胶水代码)
    - [4.3 模块化实例化](#43-模块化实例化)
  - [5. 与 JavaScript 的交互](#5-与-javascript-的交互)
    - [5.1 数据传递模式](#51-数据传递模式)
    - [5.2 性能优化技巧](#52-性能优化技巧)
  - [6. 文件系统模拟](#6-文件系统模拟)
    - [6.1 Emscripten 文件系统概述](#61-emscripten-文件系统概述)
    - [6.2 文件系统操作](#62-文件系统操作)
    - [6.3 预加载和嵌入文件](#63-预加载和嵌入文件)
  - [7. 多线程支持](#7-多线程支持)
    - [7.1 Pthreads 配置](#71-pthreads-配置)
    - [7.2 多线程 C 代码](#72-多线程-c-代码)
    - [7.3 JavaScript 多线程控制](#73-javascript-多线程控制)
  - [8. 完整示例项目](#8-完整示例项目)
    - [8.1 图像处理应用](#81-图像处理应用)
    - [8.2 音频处理应用](#82-音频处理应用)
    - [8.3 游戏物理引擎](#83-游戏物理引擎)
  - [9. 高级调试技巧](#9-高级调试技巧)
    - [9.1 调试配置](#91-调试配置)
    - [9.2 浏览器调试](#92-浏览器调试)
    - [9.3 日志和追踪](#93-日志和追踪)
  - [10. 性能优化最佳实践](#10-性能优化最佳实践)
    - [10.1 编译优化总结](#101-编译优化总结)
    - [10.2 运行时优化](#102-运行时优化)
    - [10.3 性能对比基准](#103-性能对比基准)
    - [10.4 优化检查清单](#104-优化检查清单)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. Emscripten 安装和配置

### 1.1 系统要求

| 组件 | 最低版本 | 推荐版本 |
|------|----------|----------|
| 操作系统 | Windows 10 / macOS 10.14 / Ubuntu 18.04 | 最新稳定版 |
| Python | 3.6 | 3.9+ |
| Git | 2.20 | 2.30+ |
| CMake | 3.10 | 3.20+ |
| Node.js | 14.0 | 18+ |
| 磁盘空间 | 2GB | 10GB+ |
| 内存 | 4GB | 8GB+ |

### 1.2 完整安装流程

#### 1.2.1 Windows 安装

```powershell
# ========== 方法1: 使用 PowerShell (推荐) ==========

# 1. 打开 PowerShell (不是 CMD)
# 2. 克隆仓库
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# 3. 安装最新版本
.\emsdk install latest

# 4. 激活版本
.\emsdk activate latest

# 5. 设置环境变量（当前会话）
.\emsdk_env.ps1

# 6. 永久设置环境变量（可选，推荐）
# 将以下内容添加到系统环境变量 Path:
# %USERPROFILE%\emsdk
# %USERPROFILE%\emsdk\upstream\emscripten

# 7. 验证安装
emcc --version

# ========== 方法2: 使用 Windows Subsystem for Linux (WSL) ==========

# 在 WSL 终端中执行 Linux 安装命令
wsl
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

#### 1.2.2 macOS 安装

```bash
# ========== 使用 Homebrew ==========

# 1. 安装依赖
brew install git cmake python node

# 2. 克隆 emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# 3. 安装并激活
./emsdk install latest
./emsdk activate latest

# 4. 设置环境变量
source ./emsdk_env.sh

# 5. 添加到 shell 配置文件（永久生效）
echo 'source "$HOME/emsdk/emsdk_env.sh"' >> ~/.zshrc
# 或如果使用 bash:
# echo 'source "$HOME/emsdk/emsdk_env.sh"' >> ~/.bashrc

# 6. 验证
emcc --version

# ========== 使用 MacPorts ==========
sudo port install emscripten

# ========== 使用 Homebrew 直接安装（可能不是最新版） ==========
brew install emscripten
```

#### 1.2.3 Linux 安装

```bash
# ========== Ubuntu/Debian ==========

# 1. 安装依赖
sudo apt-get update
sudo apt-get install -y git cmake python3 python3-pip nodejs

# 2. 克隆 emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# 3. 安装
./emsdk install latest
./emsdk activate latest

# 4. 设置环境
source ./emsdk_env.sh

# 5. 永久设置
echo 'source "$HOME/emsdk/emsdk_env.sh"' >> ~/.bashrc

# ========== Fedora/RHEL/CentOS ==========
sudo dnf install git cmake python3 nodejs
# 其余步骤相同

# ========== Arch Linux ==========
yay -S emscripten
# 或
sudo pacman -S emscripten
```

#### 1.2.4 Docker 安装

```dockerfile
# Dockerfile
FROM emscripten/emsdk:latest

WORKDIR /src

# 复制项目文件
COPY . .

# 编译命令
RUN emcc main.c -o output.js

# 运行
CMD ["node", "output.js"]
```

```bash
# 使用 Docker 运行
docker run --rm -v $(pwd):/src emscripten/emsdk:latest \
    emcc /src/main.c -o /src/output.js

# 交互式容器
docker run -it --rm -v $(pwd):/src emscripten/emsdk:latest /bin/bash
```

### 1.3 多版本管理

```bash
# ========== 查看可用版本 ==========
./emsdk list

# ========== 安装特定版本 ==========
./emsdk install 3.1.45
./emsdk activate 3.1.45

# ========== 安装最新发布版 ==========
./emsdk install latest
./emsdk activate latest

# ========== 安装最新开发版 ==========
./emsdk install tot  # tip-of-tree
./emsdk activate tot

# ========== 安装 LTS 版本 ==========
./emsdk install 3.1.0
./emsdk activate 3.1.0 --permanent

# ========== 切换版本 ==========
./emsdk activate 3.1.45
source ./emsdk_env.sh

# ========== 卸载版本 ==========
./emsdk uninstall 3.1.45

# ========== 更新 emsdk 本身 ==========
git pull
./emsdk install latest
./emsdk activate latest
```

### 1.4 配置环境变量

```bash
# ========== emsdk_env.sh 设置的环境变量 ==========

echo $EMSDK                    # /path/to/emsdk
echo $EMSDK_NODE               # /path/to/node
echo $EMSDK_PYTHON             # /path/to/python
echo $EM_CONFIG                # /path/to/.emscripten
echo $EM_CACHE                 # /path/to/cache
echo $EMCC_WASM_BACKEND        # 1
echo $EMCC_SKIP_SANITY_CHECK   # (可选)

# ========== 自定义配置文件 ~/.emscripten ==========

# 创建配置文件
cat > ~/.emscripten << 'EOF'
import os

# LLVM 路径
LLVM_ROOT = os.path.expanduser('~/emsdk/upstream/bin')

# Node.js 路径
NODE_JS = os.path.expanduser('~/emsdk/node/bin/node')

# Python 路径
PYTHON = os.path.expanduser('~/emsdk/python/bin/python3')

# Java 路径（用于 Closure Compiler）
JAVA = 'java'

# 编译器优化级别
EMCC_DEBUG = 0

# 缓存目录
CACHE = os.path.expanduser('~/.emscripten_cache')

# 临时目录
TEMP_DIR = '/tmp'

# 启用 WASM 后端
WASM_BACKEND = 1
EOF

# ========== 使用 .emscripten 配置 ==========
export EM_CONFIG=~/.emscripten
```

### 1.5 验证安装和诊断

```bash
# ========== 基础验证 ==========

# 检查 emcc 版本
emcc --version
# 预期输出:
# emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 3.1.45
# clang version 17.0.0
# Target: wasm32-unknown-emscripten

# 检查环境
emcc --check

# 显示详细配置
emcc -v

# 显示帮助
emcc --help

# ========== 诊断工具 ==========

# 运行完整诊断
emcc --version
which emcc
em-config EMSDK_NODE
em-config LLVM_ROOT
em-config BINARYEN_ROOT

# 检查工具链
ls -la $EMSDK/upstream/bin/
emcc -print-search-dirs

# 测试编译
echo 'int main() { return 0; }' > test.c
emcc test.c -o test.js
node test.js
echo $?  # 应该输出 0

# ========== 清理和重置 ==========

# 清理缓存
emcc --clear-cache

# 清理所有临时文件
rm -rf ~/.emscripten_cache
rm -rf ~/.emscripten_ports

# 完全重置
./emsdk uninstall latest
./emsdk install latest
./emsdk activate latest
```

---

## 2. 编译选项详解

### 2.1 优化级别详解

```bash
# ========== 优化级别对比 ==========

# -O0: 无优化（调试模式）
# - 编译速度最快
# - 包含完整调试信息
# - 代码体积最大
# - 执行速度最慢
emcc main.c -O0 -g3 -o debug.js

# -O1: 基本优化
# - 移除死代码
# - 局部优化
# - 保留部分调试信息
emcc main.c -O1 -o basic_opt.js

# -O2: 更多优化（推荐用于开发）
# - 内联函数
# - 循环优化
# - 常量传播
# - 可能影响调试
emcc main.c -O2 -o dev_opt.js

# -O3: 激进优化（推荐用于生产）
# - 最大优化级别
# - 自动向量化
# - 函数克隆
# - 激进内联
emcc main.c -O3 -o production.js

# -Os: 优化体积
# - 专注于减少代码大小
# - 牺牲部分性能换取体积
emcc main.c -Os -o size_opt.js

# -Oz: 极致体积优化
# - 最小化代码体积
# - 可能显著影响性能
emcc main.c -Oz -o minimal.js

# ========== 链接时优化 (LTO) ==========

# LTO 模式
emcc main.c -flto -O3 -o lto_output.js

# 薄 LTO（编译更快，优化稍弱）
emcc main.c -flto=thin -O3 -o thin_lto.js

# ========== 编译对比表 ==========
```

| 优化级别 | 编译时间 | 执行速度 | 代码体积 | 调试友好 | 适用场景 |
|----------|----------|----------|----------|----------|----------|
| -O0 | ★★★★★ | ★☆☆☆☆ | ★★★★★ | ★★★★★ | 调试开发 |
| -O1 | ★★★★☆ | ★★★☆☆ | ★★★★☆ | ★★★★☆ | 快速迭代 |
| -O2 | ★★★☆☆ | ★★★★☆ | ★★★☆☆ | ★★★☆☆ | 开发测试 |
| -O3 | ★★☆☆☆ | ★★★★★ | ★★☆☆☆ | ★★☆☆☆ | 生产发布 |
| -Os | ★★★☆☆ | ★★★☆☆ | ★★★★☆ | ★★★☆☆ | 体积敏感 |
| -Oz | ★★☆☆☆ | ★★☆☆☆ | ★★★★★ | ★★☆☆☆ | 极端体积 |

### 2.2 内存模型选项

```bash
# ========== 内存配置 ==========

# 初始内存大小（64KB 的倍数）
-sINITIAL_MEMORY=16777216        # 16MB
-sINITIAL_MEMORY=33554432        # 32MB
-sINITIAL_MEMORY=67108864        # 64MB (默认)
-sINITIAL_MEMORY=134217728       # 128MB
-sINITIAL_MEMORY=268435456       # 256MB

# 简写形式
-sINITIAL_MEMORY=64MB
-sINITIAL_MEMORY=128MB

# 最大内存大小
-sMAXIMUM_MEMORY=536870912       # 512MB
-sMAXIMUM_MEMORY=1073741824      # 1GB
-sMAXIMUM_MEMORY=2147483648      # 2GB (WASM 最大)

# 简写形式
-sMAXIMUM_MEMORY=512MB
-sMAXIMUM_MEMORY=1GB
-sMAXIMUM_MEMORY=2GB

# 允许内存增长
-sALLOW_MEMORY_GROWTH=1
-sALLOW_MEMORY_GROWTH=0          # 固定内存

# 内存增长步长
-sMEMORY_GROWTH_STEP=16777216    # 16MB 每次增长

# 栈大小
-sSTACK_SIZE=5242880             # 5MB (默认)
-sSTACK_SIZE=8388608             # 8MB
-sSTACK_SIZE=16777216            # 16MB

# 简写形式
-sSTACK_SIZE=5MB
-sSTACK_SIZE=8MB

# 内存对齐
-sABORTING_MALLOC=1              # malloc 失败时终止
-sABORTING_MALLOC=0              # malloc 失败返回 NULL

# ========== 内存配置示例 ==========

# 高性能计算场景
emcc compute.c -O3 \
    -sINITIAL_MEMORY=256MB \
    -sMAXIMUM_MEMORY=2GB \
    -sALLOW_MEMORY_GROWTH=1 \
    -sSTACK_SIZE=16MB \
    -o compute.js

# 固定内存场景（性能更好）
emcc game.c -O3 \
    -sINITIAL_MEMORY=128MB \
    -sALLOW_MEMORY_GROWTH=0 \
    -sSTACK_SIZE=8MB \
    -o game.js

# 内存受限设备
emcc app.c -Os \
    -sINITIAL_MEMORY=16MB \
    -sMAXIMUM_MEMORY=64MB \
    -sALLOW_MEMORY_GROWTH=1 \
    -sSTACK_SIZE=2MB \
    -o app.js
```

### 2.3 导出配置选项

```bash
# ========== 导出函数 ==========

# 导出特定函数（C 函数名前加下划线）
-sEXPORTED_FUNCTIONS="['_main', '_add', '_multiply']"

# 导出所有函数（不推荐，体积大）
-sEXPORT_ALL=1
-sEXPORT_ALL=0

# 保留函数（防止被优化掉）
-sRETAIN_COMPILER_SETTINGS=1

# ========== 导出运行时方法 ==========

# 常用运行时方法
-sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'getValue', 'setValue', 'UTF8ToString', 'stringToUTF8', 'lengthBytesUTF8', 'addFunction', 'removeFunction']"

# 完整运行时方法列表
-sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'getValue', 'setValue', 'allocate', 'ALLOC_NORMAL', 'ALLOC_STACK', 'ALLOC_STATIC', 'UTF8ToString', 'stringToUTF8', 'lengthBytesUTF8', 'UTF16ToString', 'stringToUTF16', 'lengthBytesUTF16', 'UTF32ToString', 'stringToUTF32', 'lengthBytesUTF32', 'intArrayFromString', 'intArrayToString', 'writeStringToMemory', 'writeArrayToMemory', 'writeAsciiToMemory', 'addRunDependency', 'removeRunDependency', 'addFunction', 'removeFunction', 'dynCall', 'getMemory', 'Pointer_stringify', 'warnOnce']"

# ========== 模块化选项 ==========

# 生成模块化代码
-sMODULARIZE=1

# 指定模块名称
-sEXPORT_NAME="MyModule"

# ES6 模块
-sEXPORT_ES6=1

# UMD 模块
-sEXPORT_ES6=0

# 生成工厂函数（可多次实例化）
-sMODULARIZE=1 -sEXPORT_NAME="createModule"

# ========== 导出配置示例 ==========

# 数学库
emcc math.c -O3 \
    -sEXPORTED_FUNCTIONS="['_add', '_subtract', '_multiply', '_divide', '_sqrt']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
    -sMODULARIZE=1 \
    -sEXPORT_NAME="MathLib" \
    -o math.js

# 字符串处理库
emcc strings.c -O3 \
    -sEXPORTED_FUNCTIONS="['_reverse', '_to_upper', '_to_lower']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString', 'stringToUTF8', 'lengthBytesUTF8']" \
    -sMODULARIZE=1 \
    -sEXPORT_NAME="StringLib" \
    -o strings.js

# 图像处理库
emcc image.c -O3 \
    -sEXPORTED_FUNCTIONS="['_process_image', '_get_width', '_get_height']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'getValue', 'setValue']" \
    -sALLOW_MEMORY_GROWTH=1 \
    -sMODULARIZE=1 \
    -sEXPORT_NAME="ImageProcessor" \
    -o image.js
```

### 2.4 环境相关选项

```bash
# ========== 目标环境 ==========

# 仅浏览器
-sENVIRONMENT=web

# 仅 Node.js
-sENVIRONMENT=node

# 浏览器和 Node.js
-sENVIRONMENT=web,node

# Web Worker
-sENVIRONMENT=worker

# Shell 环境
-sENVIRONMENT=shell

# 所有环境
-sENVIRONMENT=web,node,worker,shell

# ========== WASM 相关 ==========

# 输出格式
-sWASM=1              # 输出 WASM（默认）
-sWASM=0              # 输出 asm.js（已废弃）
-sWASM=2              # 输出 WASM + 备用 asm.js

# WASM 异步编译
-sWASM_ASYNC_COMPILATION=1    # 异步编译（默认）
-sWASM_ASYNC_COMPILATION=0    # 同步编译

# WASM 大内存（>2GB）
-sMAXIMUM_MEMORY=4GB
-sALLOW_MEMORY_GROWTH=1

# ========== JavaScript 输出选项 ==========

# 单文件输出（嵌入 WASM）
-sSINGLE_FILE=1

# 最小化输出
-sMINIMAL_RUNTIME=1
-sMINIMAL_RUNTIME=2

# 使用 Closure Compiler 优化 JS
--closure 1
--closure 0

# 生成 Source Map
-g4
-sSOURCE_MAP_BASE='http://localhost:8000/'

# ========== 网络和异步 ==========

# 允许同步 XHR
-sALLOW_SYNC_XHR_IN_MAIN_THREAD=1

# 使用 Fetch API
-sFETCH=1

# 使用 WebSocket
-lwebsocket.js

# 使用 WebRTC
-lwebrtc.js

# ========== 多线程 ==========

# 启用 pthreads
-sUSE_PTHREADS=1
-sPTHREAD_POOL_SIZE=4
-sPTHREAD_POOL_SIZE_STRICT=1

# 启用 SharedArrayBuffer
-sSHARED_MEMORY=1

# 需要正确的 HTTP 头
# Cross-Origin-Opener-Policy: same-origin
# Cross-Origin-Embedder-Policy: require-corp

# ========== 环境配置示例 ==========

# 纯浏览器应用
emcc app.c -O3 \
    -sENVIRONMENT=web \
    -sWASM_ASYNC_COMPILATION=1 \
    -o app.js

# Node.js CLI 工具
emcc cli.c -O3 \
    -sENVIRONMENT=node \
    -sWASM_ASYNC_COMPILATION=0 \
    -o cli.js

# Web Worker
emcc worker.c -O3 \
    -sENVIRONMENT=worker \
    -sEXPORTED_FUNCTIONS="['_process']" \
    -o worker.js

# 多线程应用
emcc parallel.c -O3 \
    -sUSE_PTHREADS=1 \
    -sPTHREAD_POOL_SIZE=8 \
    -sINITIAL_MEMORY=256MB \
    -o parallel.js
```

### 2.5 调试和分析选项

```bash
# ========== 调试信息 ==========

# 无调试信息（发布）
# （默认，无 -g 标志）

# 基础调试信息
-g

# 详细调试信息
-g2

# DWARF 调试信息
-g3

# Source Map
-g4

# 保留函数名
-sDEMANGLE_SUPPORT=1

# 断言检查
-sASSERTIONS=1
-sASSERTIONS=2

# 安全检查
-sSAFE_HEAP=1
-sSAFE_HEAP_LOG=1

# 堆栈溢出检查
-sSTACK_OVERFLOW_CHECK=1
-sSTACK_OVERFLOW_CHECK=2

# ========== 分析工具 ==========

# 生成性能分析数据
--profiling
--profiling-funcs

# 生成代码大小分析
-sRELOCATABLE=1

# 生成内存分析器
-sMEMORYPROFILER=1

# 生成线程分析器（多线程时）
-sPTHREADS_PROFILING=1

# ========== 日志和诊断 ==========

# 详细输出
-v

# 显示编译阶段
-###

# 保存中间文件
-save-temps

# 显示内存使用
-sVERBOSE=1

# 运行时跟踪
-sRUNTIME_LOGGING=1

# ========== 调试配置示例 ==========

# 开发调试配置
emcc debug.c -O0 \
    -g3 \
    -sASSERTIONS=2 \
    -sSAFE_HEAP=1 \
    -sSTACK_OVERFLOW_CHECK=2 \
    -sDEMANGLE_SUPPORT=1 \
    -o debug.js

# 性能分析配置
emcc profile.c -O3 \
    -g2 \
    --profiling \
    -sDEMANGLE_SUPPORT=1 \
    -o profile.js

# Source Map 配置
emcc app.c -O2 \
    -g4 \
    -sSOURCE_MAP_BASE='http://localhost:8000/' \
    -o app.js
```

---

## 3. EMSCRIPTEN_KEEPALIVE 宏

### 3.1 宏的工作原理

```c
// emscripten/emscripten.h 中的定义
#ifndef EMSCRIPTEN_KEEPALIVE
#ifdef __EMSCRIPTEN__
#include "em_asm.h"
// 使用 __attribute__((used)) 防止被优化器移除
// 使用 __attribute__((visibility("default"))) 确保导出
#define EMSCRIPTEN_KEEPALIVE __attribute__((used)) __attribute__((visibility("default")))
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif
```

### 3.2 基本使用模式

```c
// ========== 基础用法 ==========

#include <emscripten/emscripten.h>

// 简单函数导出
EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {
    return a + b;
}

// 带参数验证的导出函数
EMSCRIPTEN_KEEPALIVE
int safe_divide(int a, int b) {
    if (b == 0) {
        return 0;  // 或设置错误码
    }
    return a / b;
}

// 浮点数运算
EMSCRIPTEN_KEEPALIVE
double calculate_distance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

// ========== 数据指针操作 ==========

// 处理数组
EMSCRIPTEN_KEEPALIVE
int sum_array(int* arr, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

// 修改数组（原地操作）
EMSCRIPTEN_KEEPALIVE
void scale_array(float* arr, int length, float scale) {
    for (int i = 0; i < length; i++) {
        arr[i] *= scale;
    }
}

// 处理二维数组（矩阵）
EMSCRIPTEN_KEEPALIVE
void matrix_multiply(
    const float* A,
    const float* B,
    float* C,
    int m, int n, int p
) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * p + j];
            }
            C[i * p + j] = sum;
        }
    }
}

// ========== 字符串处理 ==========

// 返回静态字符串
EMSCRIPTEN_KEEPALIVE
const char* get_version() {
    return "Library v1.0.0";
}

// 写入字符串到缓冲区
EMSCRIPTEN_KEEPALIVE
int format_message(char* buffer, int buffer_size, int code) {
    const char* msg;
    switch (code) {
        case 0: msg = "Success"; break;
        case 1: msg = "Invalid argument"; break;
        case 2: msg = "Out of memory"; break;
        default: msg = "Unknown error"; break;
    }
    int len = strlen(msg);
    if (len >= buffer_size) len = buffer_size - 1;
    memcpy(buffer, msg, len);
    buffer[len] = '\0';
    return len;
}

// 处理输入字符串
EMSCRIPTEN_KEEPALIVE
int count_vowels(const char* str) {
    int count = 0;
    while (*str) {
        char c = tolower(*str++);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            count++;
        }
    }
    return count;
}

// ========== 结构体操作 ==========

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    float r, g, b, a;
} Color;

// 向量运算
EMSCRIPTEN_KEEPALIVE
float vector_dot(const Vector3* a, const Vector3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

EMSCRIPTEN_KEEPALIVE
void vector_cross(const Vector3* a, const Vector3* b, Vector3* result) {
    result->x = a->y * b->z - a->z * b->y;
    result->y = a->z * b->x - a->x * b->z;
    result->z = a->x * b->y - a->y * b->x;
}

EMSCRIPTEN_KEEPALIVE
float vector_length(const Vector3* v) {
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

// 颜色混合
EMSCRIPTEN_KEEPALIVE
void color_lerp(const Color* a, const Color* b, float t, Color* result) {
    result->r = a->r + (b->r - a->r) * t;
    result->g = a->g + (b->g - a->g) * t;
    result->b = a->b + (b->b - a->b) * t;
    result->a = a->a + (b->a - a->a) * t;
}

// ========== 回调和状态 ==========

// 静态状态管理
static int counter = 0;

EMSCRIPTEN_KEEPALIVE
int increment_counter() {
    return ++counter;
}

EMSCRIPTEN_KEEPALIVE
int get_counter() {
    return counter;
}

EMSCRIPTEN_KEEPALIVE
void reset_counter() {
    counter = 0;
}

// 上下文句柄模式
typedef struct {
    int id;
    void* data;
    size_t size;
} Context;

static Context* contexts[100];
static int next_id = 1;

EMSCRIPTEN_KEEPALIVE
int context_create(size_t size) {
    Context* ctx = (Context*)malloc(sizeof(Context));
    if (!ctx) return 0;

    ctx->data = malloc(size);
    if (!ctx->data) {
        free(ctx);
        return 0;
    }

    ctx->id = next_id++;
    ctx->size = size;
    memset(ctx->data, 0, size);

    contexts[ctx->id] = ctx;
    return ctx->id;
}

EMSCRIPTEN_KEEPALIVE
void context_destroy(int handle) {
    if (handle <= 0 || handle >= 100) return;
    Context* ctx = contexts[handle];
    if (ctx) {
        free(ctx->data);
        free(ctx);
        contexts[handle] = NULL;
    }
}

EMSCRIPTEN_KEEPALIVE
void* context_get_data(int handle) {
    if (handle <= 0 || handle >= 100) return NULL;
    Context* ctx = contexts[handle];
    return ctx ? ctx->data : NULL;
}
```

### 3.3 编译和调用

```bash
# ========== 编译导出库 ==========

# 基础编译
emcc library.c -O3 \
    -sEXPORTED_FUNCTIONS="['_add', '_safe_divide', '_calculate_distance', '_sum_array', '_scale_array', '_get_version', '_count_vowels', '_vector_dot', '_vector_cross', '_increment_counter', '_context_create', '_context_destroy', '_context_get_data', '_malloc', '_free']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'getValue', 'setValue', 'UTF8ToString']" \
    -sMODULARIZE=1 \
    -sEXPORT_NAME="MyLibrary" \
    -o mylib.js

# 使用 Embind（更简单的接口）
emcc library.cpp -O3 \
    --bind \
    -o mylib.js
```

```javascript
// ========== JavaScript 调用示例 ==========

// 使用 cwrap（推荐）
const add = Module.cwrap('add', 'number', ['number', 'number']);
const divide = Module.cwrap('safe_divide', 'number', ['number', 'number']);
const sumArray = Module.cwrap('sum_array', 'number', ['number', 'number']);

console.log(add(5, 3));           // 8
console.log(divide(10, 2));       // 5
console.log(divide(10, 0));       // 0

// 数组操作
const arr = new Int32Array([1, 2, 3, 4, 5]);
const ptr = Module._malloc(arr.length * 4);
Module.HEAP32.set(arr, ptr / 4);
console.log(sumArray(ptr, arr.length));  // 15
Module._free(ptr);

// 字符串处理
const countVowels = Module.cwrap('count_vowels', 'number', ['number']);
const str = "Hello World";
const strPtr = allocate(intArrayFromString(str), ALLOC_NORMAL);
console.log(countVowels(strPtr));  // 3
Module._free(strPtr);

// 结构体操作（通过内存布局）
const vectorDot = Module.cwrap('vector_dot', 'number', ['number', 'number']);

// 分配两个 Vector3（每个 12 字节：3 x float）
const vecA = Module._malloc(12);
const vecB = Module._malloc(12);

// 设置 vecA = (1, 2, 3)
Module.HEAPF32[vecA / 4] = 1.0;
Module.HEAPF32[vecA / 4 + 1] = 2.0;
Module.HEAPF32[vecA / 4 + 2] = 3.0;

// 设置 vecB = (4, 5, 6)
Module.HEAPF32[vecB / 4] = 4.0;
Module.HEAPF32[vecB / 4 + 1] = 5.0;
Module.HEAPF32[vecB / 4 + 2] = 6.0;

console.log(vectorDot(vecA, vecB));  // 32.0 (1*4 + 2*5 + 3*6)

Module._free(vecA);
Module._free(vecB);

// 上下文句柄模式
const createContext = Module.cwrap('context_create', 'number', ['number']);
const destroyContext = Module.cwrap('context_destroy', null, ['number']);
const getContextData = Module.cwrap('context_get_data', 'number', ['number']);

const handle = createContext(1024);  // 创建 1KB 上下文
const dataPtr = getContextData(handle);
// 使用 dataPtr 读写数据...
destroyContext(handle);
```

---

## 4. JavaScript 胶水代码生成

### 4.1 胶水代码结构

```javascript
// ========== 胶水代码主要部分 ==========

// 1. 模块配置和初始化
var Module = {
    // 预运行回调
    preRun: [],
    // 后运行回调
    postRun: [],
    // 打印输出
    print: function(text) {
        console.log(text);
    },
    // 错误输出
    printErr: function(text) {
        console.error(text);
    },
    // 画布元素（用于图形）
    canvas: (function() {
        return document.getElementById('canvas');
    })()
};

// 2. 内存管理
var HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;

function updateGlobalBufferAndViews(buf) {
    buffer = buf;
    HEAP8 = new Int8Array(buf);
    HEAPU8 = new Uint8Array(buf);
    HEAP16 = new Int16Array(buf);
    HEAPU16 = new Uint16Array(buf);
    HEAP32 = new Int32Array(buf);
    HEAPU32 = new Uint32Array(buf);
    HEAPF32 = new Float32Array(buf);
    HEAPF64 = new Float64Array(buf);
}

// 3. 堆栈管理
var STACK_BASE, STACK_TOP, STACK_MAX;
var tempRet0 = 0;

function stackAlloc(size) {
    var ret = STACK_TOP;
    STACK_TOP = STACK_TOP + size + 15 & -16;
    return ret;
}

function stackSave() {
    return STACK_TOP;
}

function stackRestore(top) {
    STACK_TOP = top;
}

// 4. 运行时辅助函数
function getValue(ptr, type) {
    switch(type) {
        case 'i1': return HEAP8[ptr];
        case 'i8': return HEAP8[ptr];
        case 'i16': return HEAP16[ptr >> 1];
        case 'i32': return HEAP32[ptr >> 2];
        case 'float': return HEAPF32[ptr >> 2];
        case 'double': return HEAPF64[ptr >> 3];
        default: abort('invalid type for getValue: ' + type);
    }
}

function setValue(ptr, value, type) {
    switch(type) {
        case 'i1': HEAP8[ptr] = value; break;
        case 'i8': HEAP8[ptr] = value; break;
        case 'i16': HEAP16[ptr >> 1] = value; break;
        case 'i32': HEAP32[ptr >> 2] = value; break;
        case 'float': HEAPF32[ptr >> 2] = value; break;
        case 'double': HEAPF64[ptr >> 3] = value; break;
        default: abort('invalid type for setValue: ' + type);
    }
}

// 5. 字符串处理
function UTF8ToString(ptr, maxBytesToRead) {
    var u0, u1, u2, u3, u4, u5;
    var str = '';
    while (1) {
        u0 = HEAPU8[ptr++];
        if (!u0) return str;
        // UTF-8 解码逻辑...
    }
}

function stringToUTF8(str, outPtr, maxBytesToWrite) {
    // UTF-8 编码逻辑...
}

function lengthBytesUTF8(str) {
    var len = 0;
    for (var i = 0; i < str.length; ++i) {
        var u = str.charCodeAt(i);
        // 计算 UTF-8 字节长度...
    }
    return len;
}

// 6. ccall/cwrap 实现
function ccall(ident, returnType, argTypes, args, opts) {
    // 函数调用实现...
}

function cwrap(ident, returnType, argTypes, opts) {
    return function() {
        return ccall(ident, returnType, argTypes, arguments, opts);
    };
}

// 7. WASM 实例化
function instantiateAsync(binary, binaryFile, imports, callback) {
    if (!binary && typeof WebAssembly.instantiateStreaming === 'function') {
        // 流式编译
        return WebAssembly.instantiateStreaming(
            fetch(binaryFile, { credentials: 'same-origin' }),
            imports
        ).then(callback);
    }
    // 降级方案...
}

// 8. 导出的 WASM 函数
var _malloc, _free, _main;
// 在实例化后赋值:
// _malloc = wasmExports.malloc;
// _free = wasmExports.free;
// _main = wasmExports.main;
```

### 4.2 自定义胶水代码

```javascript
// ========== 自定义模块配置 ==========

// 创建自定义模块工厂
function createCustomModule(options = {}) {
    return {
        // 内存配置
        wasmMemory: new WebAssembly.Memory({
            initial: options.initialMemory || 256,
            maximum: options.maximumMemory || 512,
            shared: options.sharedMemory || false
        }),

        // 导入对象
        importObject: {
            env: {
                memory: this.wasmMemory,
                __memory_base: 1024,
                __table_base: 0,
                abort: () => { throw new Error('abort'); },
                // 自定义函数...
            },
            wasi_snapshot_preview1: {
                proc_exit: (code) => {
                    console.log('Exit:', code);
                }
            }
        },

        // 预初始化钩子
        preInit: [],

        // 初始化完成钩子
        onRuntimeInitialized: null,

        // 加载 WASM
        async load(wasmUrl) {
            const response = await fetch(wasmUrl);
            const { instance } = await WebAssembly.instantiateStreaming(
                response,
                this.importObject
            );

            // 绑定导出函数
            this.exports = instance.exports;
            this._malloc = instance.exports.malloc;
            this._free = instance.exports.free;

            // 运行初始化钩子
            if (this.onRuntimeInitialized) {
                this.onRuntimeInitialized();
            }

            return this;
        }
    };
}

// ========== 自定义运行时方法 ==========

// 扩展 Module 对象
Module.customAlloc = function(size) {
    const ptr = this._malloc(size);
    if (!ptr) throw new Error('Memory allocation failed');
    return ptr;
};

Module.customFree = function(ptr) {
    if (ptr) this._free(ptr);
};

Module.allocString = function(str) {
    const len = lengthBytesUTF8(str) + 1;
    const ptr = this.customAlloc(len);
    stringToUTF8(str, ptr, len);
    return ptr;
};

Module.allocArray = function(array, type) {
    const bytesPerElement = {
        'i8': 1, 'i16': 2, 'i32': 4,
        'f32': 4, 'f64': 8
    }[type] || 4;

    const ptr = this.customAlloc(array.length * bytesPerElement);

    switch(type) {
        case 'i8':
            this.HEAP8.set(new Int8Array(array), ptr);
            break;
        case 'i32':
            this.HEAP32.set(new Int32Array(array), ptr / 4);
            break;
        case 'f32':
            this.HEAPF32.set(new Float32Array(array), ptr / 4);
            break;
        case 'f64':
            this.HEAPF64.set(new Float64Array(array), ptr / 8);
            break;
    }

    return ptr;
};

Module.readArray = function(ptr, length, type) {
    switch(type) {
        case 'i32':
            return Array.from(this.HEAP32.subarray(ptr / 4, ptr / 4 + length));
        case 'f32':
            return Array.from(this.HEAPF32.subarray(ptr / 4, ptr / 4 + length));
        case 'f64':
            return Array.from(this.HEAPF64.subarray(ptr / 8, ptr / 8 + length));
        default:
            return [];
    }
};

// ========== 类型化数组视图 ==========

Module.createView = function(ptr, length, type) {
    switch(type) {
        case 'i8': return new Int8Array(this.buffer, ptr, length);
        case 'u8': return new Uint8Array(this.buffer, ptr, length);
        case 'i16': return new Int16Array(this.buffer, ptr, length);
        case 'i32': return new Int32Array(this.buffer, ptr, length);
        case 'f32': return new Float32Array(this.buffer, ptr, length);
        case 'f64': return new Float64Array(this.buffer, ptr, length);
        default: throw new Error('Unknown type: ' + type);
    }
};
```

### 4.3 模块化实例化

```javascript
// ========== ES6 模块使用 ==========

// 编译时添加: -sEXPORT_ES6=1 -sMODULARIZE=1

// main.mjs
import createModule from './mylib.js';

const module = await createModule({
    // 模块配置
    print: (text) => console.log('[WASM]', text),
    printErr: (text) => console.error('[WASM Error]', text)
});

// 使用导出的函数
const result = module._add(5, 3);
console.log(result);

// ========== UMD 模块使用 ==========

// 编译时不加 -sEXPORT_ES6

// AMD (RequireJS)
require(['./mylib.js'], function(createModule) {
    createModule().then(function(Module) {
        console.log(Module._add(1, 2));
    });
});

// CommonJS (Node.js)
const createModule = require('./mylib.js');
createModule().then(Module => {
    console.log(Module._add(1, 2));
});

// 浏览器全局变量
<script src="mylib.js"></script>
<script>
    createModule().then(Module => {
        console.log(Module._add(1, 2));
    });
</script>

// ========== 多次实例化 ==========

// 编译: -sMODULARIZE=1 -sEXPORT_NAME="createMyLib"

async function createMultipleInstances() {
    // 创建多个独立的实例
    const lib1 = await createMyLib({
        wasmMemory: new WebAssembly.Memory({ initial: 256 })
    });

    const lib2 = await createMyLib({
        wasmMemory: new WebAssembly.Memory({ initial: 128 })
    });

    // 各自独立运行
    console.log(lib1._add(1, 2));
    console.log(lib2._add(3, 4));

    return { lib1, lib2 };
}
```

---


## 5. 与 JavaScript 的交互

### 5.1 数据传递模式

```c
// ========== C 代码: 数据交互接口 ==========

#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <string.h>

// ===== 基本数值类型 =====

// 整数运算
EMSCRIPTEN_KEEPALIVE
int add_integers(int a, int b) {
    return a + b;
}

EMSCRIPTEN_KEEPALIVE
long long multiply_long(long long a, long long b) {
    return a * b;
}

// 浮点数运算
EMSCRIPTEN_KEEPALIVE
double calculate_circle_area(double radius) {
    return 3.14159265359 * radius * radius;
}

EMSCRIPTEN_KEEPALIVE
float vector_length(float x, float y, float z) {
    return sqrt(x*x + y*y + z*z);
}

// ===== 数组操作 =====

// 整数数组求和
EMSCRIPTEN_KEEPALIVE
int sum_int_array(const int* arr, int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

// 浮点数数组操作
EMSCRIPTEN_KEEPALIVE
double sum_double_array(const double* arr, int length) {
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
    return sum;
}

EMSCRIPTEN_KEEPALIVE
void scale_float_array(float* arr, int length, float scale) {
    for (int i = 0; i < length; i++) {
        arr[i] *= scale;
    }
}

// 查找最大值及其索引
EMSCRIPTEN_KEEPALIVE
void find_max_float(const float* arr, int length, float* max_value, int* max_index) {
    if (length <= 0) return;

    *max_value = arr[0];
    *max_index = 0;

    for (int i = 1; i < length; i++) {
        if (arr[i] > *max_value) {
            *max_value = arr[i];
            *max_index = i;
        }
    }
}

// ===== 字符串操作 =====

// 计算字符串长度（字节数）
EMSCRIPTEN_KEEPALIVE
int string_byte_length(const char* str) {
    return strlen(str);
}

// 计算字符串中的字符数（UTF-8）
EMSCRIPTEN_KEEPALIVE
int string_char_count(const char* str) {
    int count = 0;
    while (*str) {
        unsigned char c = *str;
        // 跳过 UTF-8 多字节序列的后续字节
        if ((c & 0xC0) != 0x80) {
            count++;
        }
        str++;
    }
    return count;
}

// 字符串复制
EMSCRIPTEN_KEEPALIVE
int copy_string(const char* source, char* dest, int max_length) {
    int len = strlen(source);
    if (len >= max_length) {
        len = max_length - 1;
    }
    memcpy(dest, source, len);
    dest[len] = '\0';
    return len;
}

// 字符串反转（原地）
EMSCRIPTEN_KEEPALIVE
void reverse_string(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

// 转换为大写
EMSCRIPTEN_KEEPALIVE
void to_uppercase(char* str) {
    while (*str) {
        if (*str >= 'a' && *str <= 'z') {
            *str = *str - 'a' + 'A';
        }
        str++;
    }
}

// ===== 结构体操作 =====

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    float m[16];
} Matrix4;

typedef struct {
    int id;
    float x, y;
    char name[32];
} Entity;

// 向量运算
EMSCRIPTEN_KEEPALIVE
float vector3_dot(const Vector3* a, const Vector3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

EMSCRIPTEN_KEEPALIVE
void vector3_cross(const Vector3* a, const Vector3* b, Vector3* result) {
    result->x = a->y * b->z - a->z * b->y;
    result->y = a->z * b->x - a->x * b->z;
    result->z = a->x * b->y - a->y * b->x;
}

EMSCRIPTEN_KEEPALIVE
void vector3_normalize(Vector3* v) {
    float len = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len > 0) {
        v->x /= len;
        v->y /= len;
        v->z /= len;
    }
}

// 矩阵乘法
EMSCRIPTEN_KEEPALIVE
void matrix4_multiply(const Matrix4* a, const Matrix4* b, Matrix4* result) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += a->m[i * 4 + k] * b->m[k * 4 + j];
            }
            result->m[i * 4 + j] = sum;
        }
    }
}

// 实体操作
EMSCRIPTEN_KEEPALIVE
void entity_update_position(Entity* entity, float dx, float dy) {
    entity->x += dx;
    entity->y += dy;
}

EMSCRIPTEN_KEEPALIVE
void entity_set_name(Entity* entity, const char* name) {
    strncpy(entity->name, name, 31);
    entity->name[31] = '\0';
}

// ===== 内存管理 =====

// 分配内存
EMSCRIPTEN_KEEPALIVE
void* allocate_memory(int size) {
    return malloc(size);
}

// 释放内存
EMSCRIPTEN_KEEPALIVE
void free_memory(void* ptr) {
    free(ptr);
}

// 重新分配内存
EMSCRIPTEN_KEEPALIVE
void* reallocate_memory(void* ptr, int new_size) {
    return realloc(ptr, new_size);
}

// 获取指针指向的值
EMSCRIPTEN_KEEPALIVE
int read_int(const int* ptr) {
    return *ptr;
}

EMSCRIPTEN_KEEPALIVE
double read_double(const double* ptr) {
    return *ptr;
}

EMSCRIPTEN_KEEPALIVE
void write_int(int* ptr, int value) {
    *ptr = value;
}

EMSCRIPTEN_KEEPALIVE
void write_double(double* ptr, double value) {
    *ptr = value;
}

// ===== 回调函数 =====

typedef void (*ProgressCallback)(int current, int total, const char* message);

static ProgressCallback g_progress_callback = NULL;

EMSCRIPTEN_KEEPALIVE
void set_progress_callback(ProgressCallback callback) {
    g_progress_callback = callback;
}

EMSCRIPTEN_KEEPALIVE
void perform_long_operation(int iterations) {
    for (int i = 0; i < iterations; i++) {
        // 模拟工作
        for (volatile int j = 0; j < 100000; j++);

        // 调用进度回调
        if (g_progress_callback) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Processing item %d", i);
            g_progress_callback(i, iterations, msg);
        }
    }
}
```

```javascript
// ========== JavaScript 代码: 数据交互实现 ==========

// ===== 辅助函数 =====

// 分配 UTF-8 字符串并返回指针
function stringToNewUTF8(str, module) {
    const size = module.lengthBytesUTF8(str) + 1;
    const ptr = module._malloc(size);
    module.stringToUTF8(str, ptr, size);
    return ptr;
}

// 释放字符串
function freeString(ptr, module) {
    module._free(ptr);
}

// 创建类型化数组视图
function createView(module, ptr, type, length) {
    const buffer = module.HEAPU8.buffer;
    switch(type) {
        case 'i8': return new Int8Array(buffer, ptr, length);
        case 'u8': return new Uint8Array(buffer, ptr, length);
        case 'i16': return new Int16Array(buffer, ptr, length);
        case 'i32': return new Int32Array(buffer, ptr, length);
        case 'f32': return new Float32Array(buffer, ptr, length);
        case 'f64': return new Float64Array(buffer, ptr, length);
        default: throw new Error(`Unknown type: ${type}`);
    }
}

// ===== 数值类型传递 =====

async function numberExamples(Module) {
    // 使用 cwrap
    const add = Module.cwrap('add_integers', 'number', ['number', 'number']);
    const area = Module.cwrap('calculate_circle_area', 'number', ['number']);
    const vecLen = Module.cwrap('vector_length', 'number', ['number', 'number', 'number']);

    // 整数运算
    console.log('5 + 3 =', add(5, 3));  // 8

    // 浮点数运算
    console.log('Circle area (r=5):', area(5.0));  // ~78.54

    // 向量长度
    console.log('Vector length:', vecLen(3, 4, 0));  // 5.0

    // 使用 ccall
    const result = Module.ccall(
        'multiply_long',
        'number',
        ['number', 'number'],
        [123456789, 987654321]
    );
    console.log('Long multiplication:', result);
}

// ===== 数组操作 =====

async function arrayExamples(Module) {
    // 使用 cwrap
    const sumIntArray = Module.cwrap('sum_int_array', 'number', ['number', 'number']);
    const sumDoubleArray = Module.cwrap('sum_double_array', 'number', ['number', 'number']);
    const scaleArray = Module.cwrap('scale_float_array', null, ['number', 'number', 'number']);

    // 整数数组求和
    const intArr = new Int32Array([1, 2, 3, 4, 5]);
    const intPtr = Module._malloc(intArr.length * 4);
    Module.HEAP32.set(intArr, intPtr / 4);
    const intSum = sumIntArray(intPtr, intArr.length);
    console.log('Int sum:', intSum);  // 15
    Module._free(intPtr);

    // 浮点数数组求和
    const doubleArr = new Float64Array([1.5, 2.5, 3.5, 4.5]);
    const doublePtr = Module._malloc(doubleArr.length * 8);
    Module.HEAPF64.set(doubleArr, doublePtr / 8);
    const doubleSum = sumDoubleArray(doublePtr, doubleArr.length);
    console.log('Double sum:', doubleSum);  // 12.0
    Module._free(doublePtr);

    // 原地修改数组
    const floatArr = new Float32Array([1, 2, 3, 4, 5]);
    const floatPtr = Module._malloc(floatArr.length * 4);
    Module.HEAPF32.set(floatArr, floatPtr / 4);
    scaleArray(floatPtr, floatArr.length, 2.0);

    // 读取修改后的值
    const modified = createView(Module, floatPtr, 'f32', floatArr.length);
    console.log('Scaled array:', Array.from(modified));  // [2, 4, 6, 8, 10]
    Module._free(floatPtr);

    // 查找最大值
    const findMax = Module.cwrap('find_max_float', null, ['number', 'number', 'number', 'number']);
    const data = new Float32Array([3.5, 1.2, 8.7, 2.3, 5.1]);
    const dataPtr = Module._malloc(data.length * 4);
    const maxValPtr = Module._malloc(4);
    const maxIdxPtr = Module._malloc(4);

    Module.HEAPF32.set(data, dataPtr / 4);
    findMax(dataPtr, data.length, maxValPtr, maxIdxPtr);

    const maxValue = Module.HEAPF32[maxValPtr / 4];
    const maxIndex = Module.HEAP32[maxIdxPtr / 4];

    console.log('Max value:', maxValue, 'at index:', maxIndex);  // 8.7 at 2

    Module._free(dataPtr);
    Module._free(maxValPtr);
    Module._free(maxIdxPtr);
}

// ===== 字符串操作 =====

async function stringExamples(Module) {
    const byteLen = Module.cwrap('string_byte_length', 'number', ['number']);
    const charCount = Module.cwrap('string_char_count', 'number', ['number']);
    const copyString = Module.cwrap('copy_string', 'number', ['number', 'number', 'number']);
    const reverseString = Module.cwrap('reverse_string', null, ['number']);
    const toUpper = Module.cwrap('to_uppercase', null, ['number']);

    // UTF-8 字符串
    const str = "Hello 世界 🌍";
    const strPtr = stringToNewUTF8(str, Module);

    // 获取字节长度
    const bytes = byteLen(strPtr);
    console.log('Byte length:', bytes);

    // 获取字符数
    const chars = charCount(strPtr);
    console.log('Character count:', chars);

    // 复制字符串
    const destPtr = Module._malloc(256);
    const copiedLen = copyString(strPtr, destPtr, 256);
    const copied = Module.UTF8ToString(destPtr);
    console.log('Copied:', copied);

    // 反转字符串
    const mutableStr = stringToNewUTF8("Hello", Module);
    reverseString(mutableStr);
    console.log('Reversed:', Module.UTF8ToString(mutableStr));
    Module._free(mutableStr);

    // 转换为大写
    const upperPtr = stringToNewUTF8("hello world", Module);
    toUpper(upperPtr);
    console.log('Uppercase:', Module.UTF8ToString(upperPtr));
    Module._free(upperPtr);

    Module._free(strPtr);
    Module._free(destPtr);
}

// ===== 结构体操作 =====

async function structExamples(Module) {
    // Vector3 操作（12 字节 = 3 x float）
    const vectorDot = Module.cwrap('vector3_dot', 'number', ['number', 'number']);
    const vectorCross = Module.cwrap('vector3_cross', null, ['number', 'number', 'number']);
    const vectorNormalize = Module.cwrap('vector3_normalize', null, ['number']);

    // 分配两个 Vector3
    const vecA = Module._malloc(12);
    const vecB = Module._malloc(12);
    const vecResult = Module._malloc(12);

    // 设置 vecA = (1, 2, 3)
    Module.HEAPF32[vecA / 4] = 1.0;
    Module.HEAPF32[vecA / 4 + 1] = 2.0;
    Module.HEAPF32[vecA / 4 + 2] = 3.0;

    // 设置 vecB = (4, 5, 6)
    Module.HEAPF32[vecB / 4] = 4.0;
    Module.HEAPF32[vecB / 4 + 1] = 5.0;
    Module.HEAPF32[vecB / 4 + 2] = 6.0;

    // 点积
    const dot = vectorDot(vecA, vecB);
    console.log('Dot product:', dot);  // 32.0

    // 叉积
    vectorCross(vecA, vecB, vecResult);
    console.log('Cross product:',
        Module.HEAPF32[vecResult / 4],
        Module.HEAPF32[vecResult / 4 + 1],
        Module.HEAPF32[vecResult / 4 + 2]
    );  // (-3, 6, -3)

    // 归一化
    vectorNormalize(vecA);
    console.log('Normalized:',
        Module.HEAPF32[vecA / 4],
        Module.HEAPF32[vecA / 4 + 1],
        Module.HEAPF32[vecA / 4 + 2]
    );

    Module._free(vecA);
    Module._free(vecB);
    Module._free(vecResult);

    // Matrix4 操作（64 字节 = 16 x float）
    const matrixMultiply = Module.cwrap('matrix4_multiply', null, ['number', 'number', 'number']);

    const matA = Module._malloc(64);
    const matB = Module._malloc(64);
    const matResult = Module._malloc(64);

    // 初始化为单位矩阵
    for (let i = 0; i < 16; i++) {
        Module.HEAPF32[matA / 4 + i] = (i % 5 === 0) ? 1.0 : 0.0;
        Module.HEAPF32[matB / 4 + i] = (i % 5 === 0) ? 1.0 : 0.0;
    }

    matrixMultiply(matA, matB, matResult);

    // 读取结果
    const result = createView(Module, matResult, 'f32', 16);
    console.log('Matrix result:', Array.from(result));

    Module._free(matA);
    Module._free(matB);
    Module._free(matResult);
}

// ===== 回调函数 =====

async function callbackExamples(Module) {
    const setCallback = Module.cwrap('set_progress_callback', null, ['number']);
    const performOperation = Module.cwrap('perform_long_operation', null, ['number']);

    // 创建回调函数
    const callbackPtr = Module.addFunction((current, total, msgPtr) => {
        const message = Module.UTF8ToString(msgPtr);
        const progress = (current / total * 100).toFixed(1);
        console.log(`Progress: ${progress}% - ${message}`);
    }, 'viii');  // void func(int, int, int)

    // 设置回调
    setCallback(callbackPtr);

    // 执行操作
    performOperation(5);

    // 清理回调
    Module.removeFunction(callbackPtr);
}

// ===== 主函数 =====

async function main() {
    // 加载模块
    const Module = await createModule();

    console.log('=== Number Examples ===');
    await numberExamples(Module);

    console.log('\n=== Array Examples ===');
    await arrayExamples(Module);

    console.log('\n=== String Examples ===');
    await stringExamples(Module);

    console.log('\n=== Struct Examples ===');
    await structExamples(Module);

    console.log('\n=== Callback Examples ===');
    await callbackExamples(Module);
}

main().catch(console.error);
```

### 5.2 性能优化技巧

```javascript
// ========== 批量数据处理优化 ==========

class WasmArrayBuffer {
    constructor(module, type, length) {
        this.module = module;
        this.type = type;
        this.length = length;
        this.elementSize = this.getElementSize(type);
        this.byteLength = length * this.elementSize;
        this.ptr = module._malloc(this.byteLength);
        this.view = this.createView();
    }

    getElementSize(type) {
        const sizes = { 'i8': 1, 'u8': 1, 'i16': 2, 'i32': 4, 'f32': 4, 'f64': 8 };
        return sizes[type] || 4;
    }

    createView() {
        const buffer = this.module.HEAPU8.buffer;
        switch(this.type) {
            case 'i8': return new Int8Array(buffer, this.ptr, this.length);
            case 'u8': return new Uint8Array(buffer, this.ptr, this.length);
            case 'i16': return new Int16Array(buffer, this.ptr, this.length);
            case 'i32': return new Int32Array(buffer, this.ptr, this.length);
            case 'f32': return new Float32Array(buffer, this.ptr, this.length);
            case 'f64': return new Float64Array(buffer, this.ptr, this.length);
        }
    }

    set(data) {
        this.view.set(data);
    }

    get() {
        return this.view.slice();
    }

    free() {
        if (this.ptr) {
            this.module._free(this.ptr);
            this.ptr = null;
        }
    }
}

// 使用示例
async function optimizedArrayProcessing(Module) {
    const buffer = new WasmArrayBuffer(Module, 'f32', 1000000);

    // 填充数据
    const data = new Float32Array(1000000);
    for (let i = 0; i < data.length; i++) {
        data[i] = Math.random();
    }
    buffer.set(data);

    // 调用 WASM 处理
    const processArray = Module.cwrap('process_large_array', null, ['number', 'number']);
    processArray(buffer.ptr, buffer.length);

    // 获取结果
    const result = buffer.get();

    buffer.free();
    return result;
}

// ========== 对象池模式 ==========

class WasmObjectPool {
    constructor(module, objectSize, poolSize) {
        this.module = module;
        this.objectSize = objectSize;
        this.poolSize = poolSize;
        this.available = [];
        this.inUse = new Set();

        // 预分配内存池
        this.poolPtr = module._malloc(objectSize * poolSize);

        // 初始化可用列表
        for (let i = 0; i < poolSize; i++) {
            this.available.push(this.poolPtr + i * objectSize);
        }
    }

    acquire() {
        if (this.available.length === 0) {
            throw new Error('Pool exhausted');
        }
        const ptr = this.available.pop();
        this.inUse.add(ptr);
        return ptr;
    }

    release(ptr) {
        if (this.inUse.has(ptr)) {
            this.inUse.delete(ptr);
            this.available.push(ptr);
        }
    }

    destroy() {
        this.module._free(this.poolPtr);
    }
}

// ========== 批量字符串处理 ==========

function batchStringProcessing(Module, strings) {
    // 计算总内存需求
    let totalSize = 0;
    const stringInfo = strings.map(str => {
        const len = Module.lengthBytesUTF8(str) + 1;
        totalSize += len;
        return { str, len };
    });

    // 分配连续内存
    const buffer = Module._malloc(totalSize);

    // 写入所有字符串
    let offset = 0;
    const pointers = stringInfo.map(({ str, len }) => {
        const ptr = buffer + offset;
        Module.stringToUTF8(str, ptr, len);
        offset += len;
        return ptr;
    });

    // 处理...

    // 清理
    Module._free(buffer);

    return pointers;
}
```

---

## 6. 文件系统模拟

### 6.1 Emscripten 文件系统概述

```
┌─────────────────────────────────────────────────────────────┐
│                 Emscripten 文件系统架构                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  MEMFS (Memory File System)                            │  │
│  │  ├─ 数据存储在内存中                                    │  │
│  │  ├─ 页面刷新后数据丢失                                  │  │
│  │  └─ 适用于临时文件                                      │  │
│  └───────────────────────────────────────────────────────┘  │
│                           ↓                                 │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  IDBFS (IndexedDB File System)                         │  │
│  │  ├─ 数据持久化到 IndexedDB                             │  │
│  │  ├─ 页面刷新后数据保留                                  │  │
│  │  └─ 需要手动同步                                        │  │
│  └───────────────────────────────────────────────────────┘  │
│                           ↓                                 │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  NODEFS (Node.js File System)                          │  │
│  │  ├─ 访问本地文件系统                                    │  │
│  │  └─ 仅在 Node.js 环境可用                               │  │
│  └───────────────────────────────────────────────────────┘  │
│                           ↓                                 │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  WORKERFS (Worker File System)                         │  │
│  │  ├─ 在 Worker 中读取文件                               │  │
│  │  └─ 用于大文件处理                                      │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 文件系统操作

```c
// ========== C 文件操作 ==========

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>

// 写入文本文件
EMSCRIPTEN_KEEPALIVE
int write_text_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        return -1;
    }

    size_t written = fwrite(content, 1, strlen(content), file);
    fclose(file);

    return (written == strlen(content)) ? 0 : -1;
}

// 读取文本文件
EMSCRIPTEN_KEEPALIVE
char* read_text_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file for reading: %s\n", filename);
        return NULL;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 分配缓冲区
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    // 读取内容
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';
    fclose(file);

    return buffer;
}

// 写入二进制文件
EMSCRIPTEN_KEEPALIVE
int write_binary_file(const char* filename, const void* data, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;

    size_t written = fwrite(data, 1, size, file);
    fclose(file);

    return (written == size) ? 0 : -1;
}

// 读取二进制文件
EMSCRIPTEN_KEEPALIVE
void* read_binary_file(const char* filename, size_t* out_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void* buffer = malloc(size);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    *out_size = fread(buffer, 1, size, file);
    fclose(file);

    return buffer;
}

// 追加内容到文件
EMSCRIPTEN_KEEPALIVE
int append_to_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "a");
    if (!file) return -1;

    fprintf(file, "%s", content);
    fclose(file);
    return 0;
}

// 检查文件是否存在
EMSCRIPTEN_KEEPALIVE
int file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// 获取文件大小
EMSCRIPTEN_KEEPALIVE
long get_file_size(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

// 删除文件
EMSCRIPTEN_KEEPALIVE
int delete_file(const char* filename) {
    return remove(filename);
}

// 创建目录
EMSCRIPTEN_KEEPALIVE
int create_directory(const char* path) {
    #ifdef _WIN32
        return _mkdir(path);
    #else
        return mkdir(path, 0755);
    #endif
}

// 列出目录内容
EMSCRIPTEN_KEEPALIVE
void list_directory(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", path);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

// CSV 文件处理示例
EMSCRIPTEN_KEEPALIVE
int process_csv_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;

    char line[1024];
    int row = 0;

    while (fgets(line, sizeof(line), file)) {
        // 解析 CSV 行
        char* token = strtok(line, ",");
        int col = 0;

        while (token) {
            printf("Row %d, Col %d: %s\n", row, col, token);
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    fclose(file);
    return row;
}
```

### 6.3 预加载和嵌入文件

```bash
# ========== 预加载文件 ==========

# 预加载单个文件
emcc main.c -o app.js --preload-file data.txt

# 预加载整个目录
emcc main.c -o app.js --preload-file assets/

# 预加载并重命名
emcc main.c -o app.js --preload-file original.txt@renamed.txt

# 预加载到虚拟路径
emcc main.c -o app.js --preload-file local/data@/virtual/data

# 多个预加载
emcc main.c -o app.js \
    --preload-file config.json \
    --preload-file textures/ \
    --preload-file sounds/

# ========== 嵌入文件 ==========

# 嵌入单个文件（包含在 JS 中）
emcc main.c -o app.js --embed-file data.txt

# 嵌入目录
emcc main.c -o app.js --embed-file assets/

# ========== 从文件系统加载 (Node.js) ==========

# 使用 NODEFS
emcc main.c -o app.js -lnodefs.js \
    -sENVIRONMENT=node

# ========== 完整示例 ==========

# 游戏资源打包
emcc game.c -O3 -o game.js \
    --preload-file assets/images@/images \
    --preload-file assets/audio@/audio \
    --preload-file assets/fonts@/fonts \
    --preload-file config/game.cfg@/config.cfg \
    -sINITIAL_MEMORY=128MB
```

```javascript
// ========== JavaScript 文件系统操作 ==========

// 等待文件系统初始化
Module.onRuntimeInitialized = function() {
    // 访问预加载的文件
    const data = Module.FS.readFile('/data.txt', { encoding: 'utf8' });
    console.log(data);
};

// IDBFS 同步
Module.preRun = [function() {
    // 挂载 IDBFS
    FS.mkdir('/persist');
    FS.mount(IDBFS, {}, '/persist');

    // 从 IndexedDB 同步到内存
    FS.syncfs(true, function(err) {
        if (err) console.error('Sync error:', err);
    });
}];

// 保存到持久存储
function saveToPersistent(data, filename) {
    // 写入文件
    FS.writeFile('/persist/' + filename, data);

    // 同步到 IndexedDB
    FS.syncfs(false, function(err) {
        if (err) {
            console.error('Save failed:', err);
        } else {
            console.log('Saved successfully');
        }
    });
}

// 文件系统 API 封装
class WasmFileSystem {
    constructor(module) {
        this.FS = module.FS;
    }

    // 写入文本文件
    writeText(path, content) {
        const data = new TextEncoder().encode(content);
        this.FS.writeFile(path, data);
    }

    // 读取文本文件
    readText(path) {
        const data = this.FS.readFile(path);
        return new TextDecoder().decode(data);
    }

    // 写入二进制文件
    writeBinary(path, buffer) {
        this.FS.writeFile(path, new Uint8Array(buffer));
    }

    // 读取二进制文件
    readBinary(path) {
        return this.FS.readFile(path).buffer;
    }

    // 检查文件是否存在
    exists(path) {
        try {
            this.FS.stat(path);
            return true;
        } catch (e) {
            return false;
        }
    }

    // 创建目录
    mkdir(path) {
        this.FS.mkdir(path);
    }

    // 删除文件或目录
    remove(path) {
        this.FS.unlink(path);
    }

    // 列出目录
    readdir(path) {
        return this.FS.readdir(path);
    }

    // 获取文件信息
    stat(path) {
        return this.FS.stat(path);
    }
}
```

---

## 7. 多线程支持

### 7.1 Pthreads 配置

```bash
# ========== 启用多线程编译 ==========

# 基础多线程编译
emcc main.c -o app.js \
    -sUSE_PTHREADS=1 \
    -sPTHREAD_POOL_SIZE=4

# 完整多线程配置
emcc main.c -O3 -o app.js \
    -sUSE_PTHREADS=1 \
    -sPTHREAD_POOL_SIZE=8 \
    -sPTHREAD_POOL_SIZE_STRICT=2 \
    -sINITIAL_MEMORY=256MB \
    -sMAXIMUM_MEMORY=2GB \
    -sALLOW_MEMORY_GROWTH=1 \
    -sSHARED_MEMORY=1

# 编译 Worker 文件
emcc worker.c -O3 -o worker.js \
    -sUSE_PTHREADS=1 \
    -sPTHREAD_POOL_SIZE=4 \
    -sEXPORTED_FUNCTIONS="['_worker_main']" \
    -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"

# ========== 必需的 HTTP 响应头 ==========

# 对于多线程应用，服务器必须设置:
Cross-Origin-Embedder-Policy: require-corp
Cross-Origin-Opener-Policy: same-origin
```

### 7.2 多线程 C 代码

```c
// ========== 多线程计算示例 ==========

#include <pthread.h>
#include <emscripten/emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_THREADS 4

// 线程参数结构
typedef struct {
    int thread_id;
    int start;
    int end;
    float* input;
    float* output;
} ThreadData;

// 线程函数：并行数组处理
void* process_chunk(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    for (int i = data->start; i < data->end; i++) {
        // 计算密集型操作
        data->output[i] = sqrt(data->input[i] * data->input[i] +
                               data->input[i] * 0.5f);
    }

    return NULL;
}

// 并行处理数组
EMSCRIPTEN_KEEPALIVE
void parallel_process(float* input, float* output, int size) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int chunk_size = size / NUM_THREADS;

    // 创建线程
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == NUM_THREADS - 1) ? size : (i + 1) * chunk_size;
        thread_data[i].input = input;
        thread_data[i].output = output;

        pthread_create(&threads[i], NULL, process_chunk, &thread_data[i]);
    }

    // 等待所有线程完成
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

// ========== 线程池任务处理 ==========

#define TASK_QUEUE_SIZE 100

typedef struct {
    void (*function)(void*);
    void* argument;
} Task;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Task tasks[TASK_QUEUE_SIZE];
    int head;
    int tail;
    int count;
    int shutdown;
} TaskQueue;

static TaskQueue queue;
static pthread_t* workers = NULL;
static int num_workers = 0;

// Worker 线程函数
void* worker_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&queue.mutex);

        while (queue.count == 0 && !queue.shutdown) {
            pthread_cond_wait(&queue.cond, &queue.mutex);
        }

        if (queue.shutdown) {
            pthread_mutex_unlock(&queue.mutex);
            break;
        }

        Task task = queue.tasks[queue.head];
        queue.head = (queue.head + 1) % TASK_QUEUE_SIZE;
        queue.count--;

        pthread_mutex_unlock(&queue.mutex);

        // 执行任务
        task.function(task.argument);
    }

    return NULL;
}

// 初始化线程池
EMSCRIPTEN_KEEPALIVE
int thread_pool_init(int num_threads) {
    pthread_mutex_init(&queue.mutex, NULL);
    pthread_cond_init(&queue.cond, NULL);
    queue.head = 0;
    queue.tail = 0;
    queue.count = 0;
    queue.shutdown = 0;

    workers = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    num_workers = num_threads;

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&workers[i], NULL, worker_thread, NULL);
    }

    return 0;
}

// 提交任务
EMSCRIPTEN_KEEPALIVE
int thread_pool_submit(void (*function)(void*), void* argument) {
    pthread_mutex_lock(&queue.mutex);

    if (queue.count >= TASK_QUEUE_SIZE) {
        pthread_mutex_unlock(&queue.mutex);
        return -1;  // 队列满
    }

    queue.tasks[queue.tail].function = function;
    queue.tasks[queue.tail].argument = argument;
    queue.tail = (queue.tail + 1) % TASK_QUEUE_SIZE;
    queue.count++;

    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.mutex);

    return 0;
}

// 关闭线程池
EMSCRIPTEN_KEEPALIVE
void thread_pool_shutdown() {
    pthread_mutex_lock(&queue.mutex);
    queue.shutdown = 1;
    pthread_cond_broadcast(&queue.cond);
    pthread_mutex_unlock(&queue.mutex);

    for (int i = 0; i < num_workers; i++) {
        pthread_join(workers[i], NULL);
    }

    free(workers);
    workers = NULL;
    num_workers = 0;

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.cond);
}

// ========== 同步原语示例 ==========

// 互斥锁示例
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
static int shared_counter = 0;

EMSCRIPTEN_KEEPALIVE
void increment_counter() {
    pthread_mutex_lock(&counter_mutex);
    shared_counter++;
    pthread_mutex_unlock(&counter_mutex);
}

EMSCRIPTEN_KEEPALIVE
int get_counter() {
    int value;
    pthread_mutex_lock(&counter_mutex);
    value = shared_counter;
    pthread_mutex_unlock(&counter_mutex);
    return value;
}

// 条件变量示例
static pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
static int condition_met = 0;

EMSCRIPTEN_KEEPALIVE
void wait_for_condition() {
    pthread_mutex_lock(&cond_mutex);
    while (!condition_met) {
        pthread_cond_wait(&cond_var, &cond_mutex);
    }
    pthread_mutex_unlock(&cond_mutex);
}

EMSCRIPTEN_KEEPALIVE
void signal_condition() {
    pthread_mutex_lock(&cond_mutex);
    condition_met = 1;
    pthread_cond_broadcast(&cond_var);
    pthread_mutex_unlock(&cond_mutex);
}
```

### 7.3 JavaScript 多线程控制

```javascript
// ========== 主线程代码 ==========

// 配置 SharedArrayBuffer
if (typeof SharedArrayBuffer === 'undefined') {
    console.error('SharedArrayBuffer not supported. Ensure correct COOP/COEP headers.');
}

// 加载多线程模块
const Module = {
    // 预分配线程池
    pthreadPoolSize: 4,

    // 打印输出
    print: (text) => console.log('[WASM]', text),
    printErr: (text) => console.error('[WASM Error]', text),

    // 初始化完成回调
    onRuntimeInitialized: function() {
        console.log('Thread pool initialized');
        runParallelComputation();
    }
};

// 并行计算示例
async function runParallelComputation() {
    const size = 1000000;

    // 创建共享内存数组
    const input = new Float32Array(new SharedArrayBuffer(size * 4));
    const output = new Float32Array(new SharedArrayBuffer(size * 4));

    // 填充输入数据
    for (let i = 0; i < size; i++) {
        input[i] = Math.random() * 100;
    }

    // 获取 WASM 内存指针
    const inputPtr = Module._malloc(size * 4);
    const outputPtr = Module._malloc(size * 4);

    // 复制数据到 WASM 内存
    Module.HEAPF32.set(input, inputPtr / 4);

    // 调用并行处理
    const startTime = performance.now();
    Module._parallel_process(inputPtr, outputPtr, size);
    const endTime = performance.now();

    console.log(`Parallel processing took ${(endTime - startTime).toFixed(2)} ms`);

    // 读取结果
    const result = Module.HEAPF32.slice(outputPtr / 4, outputPtr / 4 + size);

    // 清理
    Module._free(inputPtr);
    Module._free(outputPtr);

    return result;
}

// ========== 线程池使用示例 ==========

class WasmThreadPool {
    constructor(module, poolSize = 4) {
        this.module = module;
        this.poolSize = poolSize;
        this.initialized = false;
    }

    async init() {
        this.module._thread_pool_init(this.poolSize);
        this.initialized = true;
    }

    // 提交任务
    submit(taskFunction, argument) {
        if (!this.initialized) {
            throw new Error('Thread pool not initialized');
        }

        return this.module._thread_pool_submit(taskFunction, argument);
    }

    shutdown() {
        if (this.initialized) {
            this.module._thread_pool_shutdown();
            this.initialized = false;
        }
    }
}

// ========== Web Worker 中的 WASM ==========

// worker.js
self.importScripts('wasm-thread.js');

let Module;

self.onmessage = async function(e) {
    const { type, data } = e.data;

    switch (type) {
        case 'init':
            Module = await createModule({
                pthreadPoolSize: data.threads || 4
            });
            self.postMessage({ type: 'initialized' });
            break;

        case 'compute':
            const startTime = performance.now();
            const result = Module._process_data(data.inputPtr, data.size);
            const endTime = performance.now();

            self.postMessage({
                type: 'result',
                result: result,
                duration: endTime - startTime
            });
            break;
    }
};

// 主线程
const worker = new Worker('worker.js');

worker.postMessage({
    type: 'init',
    data: { threads: 4 }
});
```

---

## 8. 完整示例项目

### 8.1 图像处理应用

```c
// image_processor.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <math.h>

// 完整图像处理库（见前文）
// ... (已在前面的章节中定义)

// 批量处理接口
EMSCRIPTEN_KEEPALIVE
void batch_process_images(
    unsigned char** images,
    int* widths,
    int* heights,
    int count,
    int operation
) {
    for (int i = 0; i < count; i++) {
        Image* img = create_image_from_data(images[i], widths[i], heights[i], 4);

        switch (operation) {
            case 0: grayscale(img); break;
            case 1: blur(img); break;
            case 2: invert(img); break;
            case 3: edge_detect(img); break;
        }

        destroy_image(img);
    }
}
```

```javascript
// image-app.js
class ImageProcessorApp {
    constructor() {
        this.module = null;
        this.canvas = document.getElementById('canvas');
        this.ctx = this.canvas.getContext('2d');
    }

    async init() {
        this.module = await createImageProcessor({
            initialMemory: 128 * 1024 * 1024
        });
    }

    async loadImage(file) {
        return new Promise((resolve) => {
            const img = new Image();
            img.onload = () => resolve(img);
            img.src = URL.createObjectURL(file);
        });
    }

    imageToBuffer(img) {
        this.canvas.width = img.width;
        this.canvas.height = img.height;
        this.ctx.drawImage(img, 0, 0);
        return this.ctx.getImageData(0, 0, img.width, img.height);
    }

    applyFilter(imageData, filterType) {
        const size = imageData.width * imageData.height * 4;
        const ptr = this.module._malloc(size);

        // 复制数据到 WASM
        this.module.HEAPU8.set(imageData.data, ptr);

        // 应用滤镜
        const applyFilter = this.module.cwrap('apply_filter', null, [
            'number', 'number', 'number', 'number'
        ]);

        applyFilter(ptr, imageData.width, imageData.height, filterType);

        // 读取结果
        const result = new Uint8ClampedArray(
            this.module.HEAPU8.slice(ptr, ptr + size)
        );

        this.module._free(ptr);

        return new ImageData(result, imageData.width, imageData.height);
    }

    displayResult(imageData) {
        this.canvas.width = imageData.width;
        this.canvas.height = imageData.height;
        this.ctx.putImageData(imageData, 0, 0);
    }
}

// 使用
const app = new ImageProcessorApp();
await app.init();

const img = await app.loadImage(fileInput.files[0]);
const imageData = app.imageToBuffer(img);
const processed = app.applyFilter(imageData, 0);  // grayscale
app.displayResult(processed);
```

### 8.2 音频处理应用

```c
// audio_processor.c
#include <emscripten/emscripten.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define MAX_CHANNELS 2

// 音频缓冲区
typedef struct {
    float* data;
    int length;
    int channels;
} AudioBuffer;

// 生成正弦波
EMSCRIPTEN_KEEPALIVE
void generate_sine_wave(float* buffer, int length, float frequency, float amplitude) {
    for (int i = 0; i < length; i++) {
        float t = (float)i / SAMPLE_RATE;
        buffer[i] = amplitude * sinf(2.0f * 3.14159265f * frequency * t);
    }
}

// 应用增益
EMSCRIPTEN_KEEPALIVE
void apply_gain(float* buffer, int length, float gain) {
    for (int i = 0; i < length; i++) {
        buffer[i] *= gain;
        // 限幅
        if (buffer[i] > 1.0f) buffer[i] = 1.0f;
        if (buffer[i] < -1.0f) buffer[i] = -1.0f;
    }
}

// 简单混响效果
EMSCRIPTEN_KEEPALIVE
void apply_reverb(float* input, float* output, int length, float mix, float decay) {
    static float delay_buffer[44100] = {0};  // 1秒延迟
    static int delay_pos = 0;

    for (int i = 0; i < length; i++) {
        float delayed = delay_buffer[delay_pos];
        delay_buffer[delay_pos] = input[i] + delayed * decay;
        output[i] = input[i] * (1.0f - mix) + delayed * mix;
        delay_pos = (delay_pos + 1) % 44100;
    }
}

// FFT 辅助函数（简化版）
EMSCRIPTEN_KEEPALIVE
void compute_spectrum(float* input, float* output, int length) {
    // 简化的频谱计算
    // 实际应用中应使用完整的 FFT 实现
    for (int i = 0; i < length / 2; i++) {
        float sum = 0;
        for (int j = i * 2; j < (i + 1) * 2 && j < length; j++) {
            sum += fabsf(input[j]);
        }
        output[i] = sum / 2.0f;
    }
}
```

### 8.3 游戏物理引擎

```c
// physics_engine.c
#include <emscripten/emscripten.h>
#include <math.h>

#define MAX_BODIES 1000
#define GRAVITY 9.81f
#define TIME_STEP 0.016f

typedef struct {
    float x, y;        // 位置
    float vx, vy;      // 速度
    float ax, ay;      // 加速度
    float mass;        // 质量
    float radius;      // 半径（用于碰撞）
    int active;        // 是否激活
} RigidBody;

static RigidBody bodies[MAX_BODIES];
static int num_bodies = 0;

// 创建物体
EMSCRIPTEN_KEEPALIVE
int create_body(float x, float y, float mass, float radius) {
    if (num_bodies >= MAX_BODIES) return -1;

    int id = num_bodies++;
    bodies[id].x = x;
    bodies[id].y = y;
    bodies[id].vx = 0;
    bodies[id].vy = 0;
    bodies[id].ax = 0;
    bodies[id].ay = 0;
    bodies[id].mass = mass;
    bodies[id].radius = radius;
    bodies[id].active = 1;

    return id;
}

// 施加力
EMSCRIPTEN_KEEPALIVE
void apply_force(int body_id, float fx, float fy) {
    if (body_id < 0 || body_id >= num_bodies) return;
    if (!bodies[body_id].active) return;

    bodies[body_id].ax += fx / bodies[body_id].mass;
    bodies[body_id].ay += fy / bodies[body_id].mass;
}

// 更新物理
EMSCRIPTEN_KEEPALIVE
void step_physics() {
    // 应用重力
    for (int i = 0; i < num_bodies; i++) {
        if (!bodies[i].active) continue;
        bodies[i].ay += GRAVITY;
    }

    // 更新速度和位置
    for (int i = 0; i < num_bodies; i++) {
        if (!bodies[i].active) continue;

        bodies[i].vx += bodies[i].ax * TIME_STEP;
        bodies[i].vy += bodies[i].ay * TIME_STEP;

        bodies[i].x += bodies[i].vx * TIME_STEP;
        bodies[i].y += bodies[i].vy * TIME_STEP;

        // 阻尼
        bodies[i].vx *= 0.99f;
        bodies[i].vy *= 0.99f;

        // 重置加速度
        bodies[i].ax = 0;
        bodies[i].ay = 0;
    }

    // 简单碰撞检测
    for (int i = 0; i < num_bodies; i++) {
        if (!bodies[i].active) continue;

        for (int j = i + 1; j < num_bodies; j++) {
            if (!bodies[j].active) continue;

            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
            float min_dist = bodies[i].radius + bodies[j].radius;

            if (dist < min_dist) {
                // 简单弹性碰撞响应
                float nx = dx / dist;
                float ny = dy / dist;

                float dvx = bodies[j].vx - bodies[i].vx;
                float dvy = bodies[j].vy - bodies[i].vy;

                float vel_along_normal = dvx * nx + dvy * ny;

                if (vel_along_normal > 0) continue;

                float e = 0.8f;  // 弹性系数
                float j_val = -(1 + e) * vel_along_normal;
                j_val /= (1 / bodies[i].mass + 1 / bodies[j].mass);

                float impulse_x = j_val * nx;
                float impulse_y = j_val * ny;

                bodies[i].vx -= impulse_x / bodies[i].mass;
                bodies[i].vy -= impulse_y / bodies[i].mass;
                bodies[j].vx += impulse_x / bodies[j].mass;
                bodies[j].vy += impulse_y / bodies[j].mass;
            }
        }
    }
}

// 获取物体位置
EMSCRIPTEN_KEEPALIVE
void get_body_position(int body_id, float* x, float* y) {
    if (body_id < 0 || body_id >= num_bodies) return;
    *x = bodies[body_id].x;
    *y = bodies[body_id].y;
}

// 设置物体位置
EMSCRIPTEN_KEEPALIVE
void set_body_position(int body_id, float x, float y) {
    if (body_id < 0 || body_id >= num_bodies) return;
    bodies[body_id].x = x;
    bodies[body_id].y = y;
}
```

---

## 9. 高级调试技巧

### 9.1 调试配置

```bash
# ========== 调试构建设置 ==========

# 开发调试版本
emcc app.c -O0 -g3 \
    -sASSERTIONS=2 \
    -sSAFE_HEAP=1 \
    -sSTACK_OVERFLOW_CHECK=2 \
    -sDEMANGLE_SUPPORT=1 \
    -sERROR_ON_UNDEFINED_SYMBOLS=1 \
    -sWARN_ON_UNDEFINED_SYMBOLS=1 \
    -o app_debug.js

# 内存检查版本
emcc app.c -O0 -g3 \
    -sSAFE_HEAP=1 \
    -sSAFE_HEAP_LOG=1 \
    -sASSERTIONS=2 \
    -o app_memcheck.js

# 地址清理器 (ASan)
emcc app.c -O1 -g3 \
    -fsanitize=address \
    -sALLOW_MEMORY_GROWTH=1 \
    -o app_asan.js

# 线程清理器 (TSan) - 仅多线程
ecc app.c -O1 -g3 \
    -fsanitize=thread \
    -sUSE_PTHREADS=1 \
    -o app_tsan.js
```

### 9.2 浏览器调试

```javascript
// ========== 浏览器控制台调试 ==========

// 1. 检查模块加载状态
Module.onRuntimeInitialized = function() {
    console.log('Module initialized');
    console.log('Exports:', Object.keys(Module.asm || Module));
    console.log('Memory:', Module.buffer);
    console.log('HEAP8 length:', Module.HEAP8.length);
};

// 2. 内存检查
function inspectMemory(ptr, size) {
    const bytes = new Uint8Array(Module.buffer, ptr, size);
    console.log('Memory at', ptr, ':', Array.from(bytes));
}

// 3. 堆栈跟踪
function logStackTrace() {
    console.trace();
    if (Module.stackSave) {
        console.log('Stack top:', Module.stackSave());
    }
}

// 4. 检查函数导出
function checkExports() {
    const exports = Module.asm || Module;
    for (let name in exports) {
        if (typeof exports[name] === 'function') {
            console.log('Export:', name);
        }
    }
}

// 5. 性能分析
function profileFunction(fn, ...args) {
    const start = performance.now();
    const result = fn(...args);
    const end = performance.now();
    console.log(`Execution time: ${(end - start).toFixed(2)} ms`);
    return result;
}

// ========== Chrome DevTools 调试 ==========

// 1. Source Map 配置（编译时添加 -g4）
// 在 Sources 面板中可以看到原始 C 代码

// 2. Memory 面板检查
// - 使用 Chrome DevTools 的 Memory 面板
// - 拍摄堆快照
// - 分析内存使用

// 3. Performance 面板
// - 记录性能分析
// - 查看 WASM 函数调用
```

### 9.3 日志和追踪

```c
// debug_utils.c
#include <emscripten/emscripten.h>
#include <stdio.h>
#include <stdarg.h>

// 调试日志级别
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3
} LogLevel;

static LogLevel current_log_level = LOG_DEBUG;

EMSCRIPTEN_KEEPALIVE
void set_log_level(LogLevel level) {
    current_log_level = level;
}

void log_message(LogLevel level, const char* file, int line, const char* fmt, ...) {
    if (level < current_log_level) return;

    const char* level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};

    fprintf(stderr, "[%s] %s:%d: ", level_str[level], file, line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}

#define LOG_DEBUG(...) log_message(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_message(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_message(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

// 断言宏
#define WASM_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            LOG_ERROR("Assertion failed: %s\n  Condition: %s\n  File: %s:%d", \
                      message, #condition, __FILE__, __LINE__); \
            emscripten_debugger(); \
        } \
    } while(0)

// 内存调试
EMSCRIPTEN_KEEPALIVE
void print_memory_stats() {
    LOG_INFO("Memory stats:");
    LOG_INFO("  HEAP8 length: %zu", (size_t)EM_ASM_INT({ return Module.HEAP8.length; }));
    LOG_INFO("  Total memory: %zu MB",
             (size_t)EM_ASM_INT({ return Module.buffer.byteLength; }) / (1024 * 1024));
}

// 性能计时器
typedef struct {
    double start_time;
    const char* name;
} Timer;

EMSCRIPTEN_KEEPALIVE
Timer* timer_start(const char* name) {
    Timer* timer = (Timer*)malloc(sizeof(Timer));
    timer->name = name;
    timer->start_time = emscripten_get_now();
    return timer;
}

EMSCRIPTEN_KEEPALIVE
void timer_end(Timer* timer) {
    double elapsed = emscripten_get_now() - timer->start_time;
    LOG_INFO("Timer [%s]: %.3f ms", timer->name, elapsed);
    free(timer);
}
```

---

## 10. 性能优化最佳实践

### 10.1 编译优化总结

| 优化技术 | 编译标志 | 效果 | 适用场景 |
|----------|----------|------|----------|
| 基础优化 | `-O3` | 显著提升 | 生产环境 |
| 体积优化 | `-Oz` | 减少 30-50% | 网络传输 |
| 链接时优化 | `-flto` | 额外 5-15% | 大型项目 |
| JS 优化 | `--closure 1` | 减少 JS 大小 | 发布版本 |
| 无文件系统 | `-sFILESYSTEM=0` | 减少 100KB+ | 纯计算库 |
| 单文件输出 | `-sSINGLE_FILE=1` | 简化部署 | 嵌入式应用 |

### 10.2 运行时优化

```javascript
// ========== 减少 JS/WASM 边界穿越 ==========

// 差的实践：频繁调用
for (let i = 0; i < 1000; i++) {
    Module._process_single(data[i]);  // 1000 次穿越
}

// 好的实践：批量处理
Module._process_batch(dataPtr, data.length);  // 1 次穿越

// ========== 内存预分配 ==========

// 创建对象池
class MemoryPool {
    constructor(module, blockSize, poolSize) {
        this.module = module;
        this.blockSize = blockSize;
        this.pool = [];

        // 预分配
        const totalSize = blockSize * poolSize;
        this.basePtr = module._malloc(totalSize);

        for (let i = 0; i < poolSize; i++) {
            this.pool.push(this.basePtr + i * blockSize);
        }
    }

    acquire() {
        return this.pool.pop() || this.module._malloc(this.blockSize);
    }

    release(ptr) {
        this.pool.push(ptr);
    }
}

// ========== TypedArray 直接访问 ==========

// 直接访问 WASM 内存
const view = new Float32Array(Module.buffer, ptr, length);
// 修改 view 直接修改 WASM 内存

// ========== 避免不必要的复制 ==========

// 使用 transferables 传递大数组
worker.postMessage({
    data: largeArray.buffer
}, [largeArray.buffer]);
```

### 10.3 性能对比基准

| 操作 | JavaScript | WebAssembly | 加速比 |
|------|------------|-------------|--------|
| 矩阵乘法 (1024x1024) | 2500 ms | 45 ms | 55x |
| 斐波那契 (n=45) | 6500 ms | 2800 ms | 2.3x |
| 图像模糊 (4K) | 120 ms | 8 ms | 15x |
| MD5 哈希 (100MB) | 850 ms | 120 ms | 7x |
| JSON 解析 | 45 ms | 85 ms | 0.5x |

### 10.4 优化检查清单

```markdown
## 编译前检查
- [ ] 使用 -O3 或 -Oz 优化级别
- [ ] 启用 LTO (-flto)
- [ ] 移除未使用的导出
- [ ] 禁用不需要的功能（文件系统、异常等）

## 运行时检查
- [ ] 预分配足够的内存
- [ ] 使用批量处理减少边界穿越
- [ ] 复用分配的内存
- [ ] 使用 TypedArray 视图直接访问内存

## 调试检查
- [ ] 只在调试版本启用断言
- [ ] 发布版本移除所有调试代码
- [ ] 使用性能分析工具识别瓶颈

## 部署检查
- [ ] 启用 gzip/brotli 压缩
- [ ] 使用 CDN 分发
- [ ] 配置正确的缓存头
- [ ] 提供降级方案（WASM 不支持时）
```

---

*文档版本: 1.0*
*最后更新: 2024年*
*作者: C_Lang 项目团队*


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
