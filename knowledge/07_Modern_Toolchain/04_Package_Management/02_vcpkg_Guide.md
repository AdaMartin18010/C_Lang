# vcpkg包管理指南

> **层级定位**: 07 Modern Toolchain / 04 Package Management
> **难度级别**: L3 进阶
> **预估学习时间**: 3-4小时

---

## 📋 vcpkg简介

微软开源的C/C++包管理器：

- 跨平台支持
- 与Visual Studio深度集成
- 源码编译安装
- 版本锁定支持

---

## 🚀 快速开始

### 安装

```bash
# 克隆仓库
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # Linux/macOS
./bootstrap-vcpkg.bat # Windows
```

### 基础用法

```bash
# 安装包
./vcpkg install zlib
./vcpkg install curl[ssl]

# 列出已安装
./vcpkg list

# 更新
./vcpkg update
./vcpkg upgrade
```

### 与CMake集成

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 清单模式 (vcpkg.json)

```json
{
  "name": "myproject",
  "version": "1.0.0",
  "dependencies": [
    "zlib",
    {
      "name": "curl",
      "features": ["ssl"]
    }
  ]
}
```

---

**← [返回工具链主页](../README.md)**
