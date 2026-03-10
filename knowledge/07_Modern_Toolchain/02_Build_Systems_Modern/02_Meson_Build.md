# Meson构建系统指南

> **层级定位**: 07 Modern Toolchain / 02 Build Systems
> **难度级别**: L3 进阶
> **预估学习时间**: 4-6小时

---

## 📋 Meson简介

Meson是一个现代化的构建系统，设计目标：

- **速度快**:  Ninja后端，并行构建
- **易用**:  简洁的Python式语法
- **现代**:  原生支持交叉编译、依赖管理

### Meson vs CMake

| 特性 | Meson | CMake |
|:-----|:------|:------|
| 语法 | 简洁直观 | 复杂难懂 |
| 速度 | 快 | 中等 |
| 依赖管理 | 内置 | 需配合 |
| 生态 | 增长中 | 成熟 |
| IDE支持 | 有限 | 广泛 |

---

## 🚀 快速开始

### 安装

```bash
# Python pip
pip install meson ninja

# macOS
brew install meson ninja

# Linux
sudo apt-get install meson ninja-build
```

### 最小项目

**meson.build**:

```meson
project('myapp', 'c',
    version: '1.0.0',
    default_options: ['warning_level=3', 'c_std=c11'])

executable('myapp', 'main.c')
```

**构建**:

```bash
meson setup builddir
meson compile -C builddir
meson test -C builddir
```

---

## 📚 核心概念

### 项目声明

```meson
project('myproject', 'c',
    version: '1.0.0',
    license: 'MIT',
    default_options: [
        'warning_level=3',
        'c_std=c11',
        'buildtype=debugoptimized',
    ],
)
```

### 目标定义

```meson
# 可执行文件
executable('myapp',
    sources: ['main.c', 'utils.c'],
    include_directories: include_directories('include'),
    dependencies: [math_dep, thread_dep],
    install: true,
)

# 静态库
static_library('mylib',
    sources: ['lib.c'],
    include_directories: inc,
)

# 共享库
shared_library('mylib_shared',
    sources: ['lib.c'],
    version: '1.0.0',
    soversion: '1',
)
```

### 依赖管理

```meson
# 查找依赖
math_dep = meson.get_compiler('c').find_library('m', required: false)
thread_dep = dependency('threads')

# pkg-config
curl_dep = dependency('libcurl', version: '>=7.0')

# 可选依赖
zlib_dep = dependency('zlib', required: false)
if zlib_dep.found()
    add_project_arguments('-DHAVE_ZLIB', language: 'c')
endif
```

### 交叉编译

**cross_file.txt**:

```ini
[binaries]
c = 'arm-linux-gnueabihf-gcc'
cpp = 'arm-linux-gnueabihf-g++'
ar = 'arm-linux-gnueabihf-ar'
strip = 'arm-linux-gnueabihf-strip'

[host_machine]
system = 'linux'
cpu_family = 'arm'
cpu = 'armv7'
endian = 'little'
```

**使用**:

```bash
meson setup builddir --cross-file cross_file.txt
```

---

## 🔧 高级用法

### 测试

```meson
test_exe = executable('test_math', 'test_math.c',
    link_with: mylib)

test('math_test', test_exe)

# 带参数
test('cli_test', myapp, args: ['--test'])
```

### 自定义选项

```meson
# meson_options.txt
option('enable_feature_x', type: 'boolean', value: false,
       description: 'Enable experimental feature X')
```

```meson
# meson.build
if get_option('enable_feature_x')
    add_project_arguments('-DFEATURE_X', language: 'c')
endif
```

---

## ✅ 检查清单

- [ ] meson.build在项目根目录
- [ ] 定义了project()
- [ ] 源文件列表正确
- [ ] 依赖项已声明
- [ ] 添加了测试
- [ ] 安装规则正确

---

**← [返回工具链主页](../README.md)**
