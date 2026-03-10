# xrepo包管理指南

> **层级定位**: 07 Modern Toolchain / 04 Package Management
> **难度级别**: L2 应用
> **预估学习时间**: 2-3小时

---

## 📋 xrepo简介

xrepo是Xmake的包管理器：

- 跨平台
- 多仓库支持
- 自动构建依赖
- 分布式仓库

---

## 🚀 快速开始

### 安装

随Xmake一起安装:

```bash
curl -fsSL https://xmake.io/shget.text | bash
```

### 基础命令

```bash
# 搜索包
xrepo search zlib

# 安装包
xrepo install zlib

# 安装特定版本
xrepo install zlib 1.2.13

# 从特定仓库安装
xrepo install -f vcpkg::zlib
```

### 在Xmake中使用

```lua
add_requires("zlib 1.2.13", {optional = true})
add_requires("libcurl", {system = false})

target("myapp")
    add_packages("zlib", "libcurl")
```

---

**← [返回工具链主页](../README.md)**
