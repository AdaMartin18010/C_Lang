# xrepo包管理指南

> **层级定位**: 07 Modern Toolchain / 04 Package Management
> **难度级别**: L2 应用
> **预估学习时间**: 2-3小时

---

## 📋 xrepo简介

xrepo是Xmake的包管理器：

- 跨平台（Windows/Linux/macOS/Android/iOS等）
- 多仓库支持（官方仓库、vcpkg、Conan、apt等）
- 自动构建依赖（自动处理编译、链接）
- 分布式仓库（支持私有仓库、自建仓库）

---

## 🚀 快速开始

### 安装

随Xmake一起安装:

```bash
# Linux/macOS
curl -fsSL https://xmake.io/shget.text | bash

# Windows PowerShell
Invoke-Expression (Invoke-Webrequest 'https://xmake.io/psget.text' -UseBasicParsing).Content
```

### 基础命令

```bash
# 搜索包
xrepo search zlib

# 安装包
xrepo install zlib

# 安装特定版本
xrepo install zlib 1.2.13

# 安装特定配置
xrepo install zlib --configs="shared=true,debug=true"

# 从特定仓库安装
xrepo install -f vcpkg::zlib
xrepo install -f conan::zlib/1.2.13

# 查看已安装包
xrepo list

# 卸载包
xrepo remove zlib

# 清理缓存
xrepo clean
```

### 在Xmake中使用

```lua
-- 基本依赖声明
add_requires("zlib 1.2.13")

-- 可选依赖
add_requires("zlib 1.2.13", {optional = true})

-- 强制从源码构建
add_requires("libcurl", {system = false})

-- 指定配置选项
add_requires("zlib", {configs = {shared = true, debug = false}})

-- 多版本/多配置
add_requires("zlib >=1.2.0", "libpng", "libtiff")

target("myapp")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("zlib", "libcurl", "libpng", "libtiff")
```

---

## ⚖️ xrepo与vcpkg/Conan对比

| 特性 | xrepo | vcpkg | Conan |
|------|-------|-------|-------|
| **构建系统** | Xmake | CMake | 多构建系统 |
| **包定义语言** | Lua | CMake/JSON | Python |
| **跨平台** | ✅ 全平台 | ✅ 全平台 | ✅ 全平台 |
| **二进制缓存** | ✅ 支持 | ✅ 支持 | ✅ 支持 |
| **私有仓库** | ✅ 简单 | ⚠️ 较复杂 | ✅ 支持 |
| **版本锁定** | ✅ lockfile | ✅ 版本文件 | ✅ lockfile |
| **集成难度** | 低（同生态） | 中 | 中 |
| **生态规模** | 中等（快速增长） | 大（微软支持） | 大（社区活跃） |

### 选择建议

| 场景 | 推荐工具 |
|------|----------|
| 使用Xmake构建 | **xrepo**（原生支持） |
| CMake项目 | vcpkg/Conan |
| 微软生态 | vcpkg |
| 复杂版本管理 | Conan |
| 多构建系统兼容 | Conan |
| 快速上手 | xrepo |

---

## 🔧 高级用法

### 交叉编译配置

```lua
-- xmake.lua 中配置交叉编译
set_plat("cross")
set_arch("armv7-a")

-- 指定交叉编译工具链
set_toolchains("arm-linux-gnueabihf")

-- 或使用xrepo命令行
-- xrepo install -p cross -a arm64 zlib
```

创建交叉编译工具链配置 `toolchains/arm.lua`：

```lua
toolchain("arm-linux-gnueabihf")
    set_kind("cross")
    set_sdkdir("/opt/arm-linux-gnueabihf")
    set_toolset("cc", "arm-linux-gnueabihf-gcc")
    set_toolset("cxx", "arm-linux-gnueabihf-g++")
    set_toolset("ld", "arm-linux-gnueabihf-g++")
    set_toolset("ar", "arm-linux-gnueabihf-ar")
```

### 私有仓库配置

#### 1. 创建本地私有仓库

```bash
# 初始化私有仓库
mkdir -p my-xrepo/packages

# 创建包定义
# my-xrepo/packages/m/mylib/xmake.lua
```

包定义示例 `my-xrepo/packages/m/mylib/xmake.lua`：

```lua
package("mylib")
    set_homepage("https://github.com/user/mylib")
    set_description("My private library")
    set_license("MIT")

    add_urls("https://github.com/user/mylib/archive/$(version).tar.gz")
    add_versions("1.0.0", "sha256_hash_here")

    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("mylib_init", {includes = "mylib.h"}))
    end)
```

#### 2. 添加私有仓库

```bash
# 添加本地仓库
xrepo add-repo myrepo ~/my-xrepo

# 添加Git仓库
xrepo add-repo myrepo https://github.com/user/xmake-repo.git

# 在xmake.lua中使用
add_repositories("myrepo https://github.com/user/xmake-repo.git")
add_requires("mylib 1.0.0")
```

#### 3. 团队共享仓库

```lua
-- xmake.lua
add_repositories("company-repo git@gitlab.company.com:xmake-repo.git")
add_requires("internal-lib >=1.2.0")
```

---

## 🐛 常见问题排查

### 1. 包下载失败

```bash
# 问题：连接超时或下载失败

# 解决方案1：使用镜像源
xrepo install -vD zlib 2>&1 | tee install.log

# 解决方案2：手动下载并指定本地路径
xrepo install --linkdir=/path/to/local/zlib --includedir=/path/to/local/zlib/include zlib

# 解决方案3：设置代理
export HTTP_PROXY=http://proxy.company.com:8080
export HTTPS_PROXY=http://proxy.company.com:8080
```

### 2. 编译失败

```bash
# 问题：缺少系统依赖
# 解决方案：安装系统依赖后重试

# Ubuntu/Debian
sudo apt-get install build-essential libssl-dev

# macOS
xcode-select --install
brew install autoconf automake libtool

# Windows
# 确保已安装 Visual Studio 或 MinGW
```

### 3. 版本冲突

```lua
-- 问题：不同包依赖同一库的不同版本
-- 解决方案：使用版本覆盖或统一版本

add_requireconfs("zlib", {override = true, version = "1.2.13"})
add_requires("package-a", "package-b")  -- 两者都使用zlib 1.2.13
```

### 4. 缓存问题

```bash
# 清理所有缓存
xrepo clean

# 强制重新安装
xrepo install -f zlib

# 清理构建缓存
xmake clean -a
```

### 5. 调试安装过程

```bash
# 详细输出
xrepo install -v zlib

# 调试模式
xrepo install -vD zlib

# 查看包配置
xrepo info zlib
```

---

## 🔄 CI/CD集成

### GitHub Actions

```yaml
# .github/workflows/build.yml
name: Build

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    runs-on: ${{ matrix.os }}

    steps:
    - uses: actions/checkout@v4

    - name: Setup Xmake
      uses: xmake-io/github-action-setup-xmake@v1
      with:
        xmake-version: latest

    - name: Cache Xrepo Packages
      uses: actions/cache@v3
      with:
        path: |
          ~/.xmake/packages
          ~/AppData/Local/.xmake/packages
        key: ${{ runner.os }}-xrepo-${{ hashFiles('**/xmake.lua') }}
        restore-keys: |
          ${{ runner.os }}-xrepo-

    - name: Configure
      run: xmake config -vD

    - name: Build
      run: xmake build -vD

    - name: Test
      run: xmake test -vD
```

### GitLab CI

```yaml
# .gitlab-ci.yml
variables:
  XMAKE_PKG_CACHEDIR: "$CI_PROJECT_DIR/.xmake-cache"

cache:
  key: ${CI_JOB_NAME}
  paths:
    - .xmake-cache/

build:
  image: ubuntu:22.04
  before_script:
    - apt-get update && apt-get install -y build-essential curl git
    - curl -fsSL https://xmake.io/shget.text | bash
    - export PATH="$HOME/.local/bin:$PATH"
  script:
    - xmake config -vD
    - xmake build -vD
    - xmake test -vD
```

### Jenkins

```groovy
// Jenkinsfile
pipeline {
    agent any

    environment {
        XMAKE_PKG_CACHEDIR = "${WORKSPACE}/.xmake-cache"
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Install Xmake') {
            steps {
                sh 'curl -fsSL https://xmake.io/shget.text | bash'
            }
        }

        stage('Install Dependencies') {
            steps {
                sh '~/.local/bin/xrepo install -y zlib libcurl'
            }
        }

        stage('Build') {
            steps {
                sh '~/.local/bin/xmake build -vD'
            }
        }
    }

    post {
        always {
            // 清理或保存缓存
            archiveArtifacts artifacts: '.xmake-cache/**/*', allowEmptyArchive: true
        }
    }
}
```

---

## ✅ 最佳实践

### 1. 版本锁定

```bash
# 生成lockfile确保构建可复现
xmake require --export

# 在CI中使用lockfile
xmake require --import
```

### 2. 分层依赖管理

```lua
-- xmake.lua
-- 核心依赖（必须）
add_requires("zlib", "libpng")

-- 可选依赖（功能开关）
option("enable_ssl")
    set_default(false)
    set_description("Enable SSL support")

if has_config("enable_ssl") then
    add_requires("openssl", {optional = true})
end

-- 平台特定依赖
if is_plat("windows") then
    add_requires("pthreads4w")
end
```

### 3. 配置管理

```lua
-- 使用模式管理不同配置
add_rules("mode.debug", "mode.release")

-- 调试模式配置
if is_mode("debug") then
    add_requires("spdlog", {configs = {debug = true}})
else
    add_requires("spdlog")
end
```

### 4. 包缓存策略

```lua
-- 使用全局缓存目录
set_xmakever("2.8.0")

-- 大型项目建议配置
set_policy("package.install_only", true)  -- 只安装不自动构建
```

### 5. 依赖可视化

```bash
# 查看依赖树
xmake require --info

# 导出依赖图
xmake require --graph > deps.dot
dot -Tpng deps.dot -o deps.png
```

### 6. 安全实践

```lua
-- 验证包校验和
add_versions("1.2.13", "b3a24de97a8fdbc835b9833169501030b8977031bcb54b3b3ac13740f846ab30")

-- 限制网络访问
set_policy("package.fetch_only", true)
```

### 7. 团队协作规范

```lua
-- 统一仓库配置
add_repositories("company-repo git@gitlab.company.com:xmake-repo.git master")

-- 使用配置文件管理不同环境
includes("options.lua")

-- 文档化依赖用途
-- zlib: 压缩支持
-- openssl: HTTPS/加密支持
add_requires("zlib", "openssl")
```

---

## 📚 参考资源

- [xrepo官方文档](https://xrepo.xmake.io/)
- [Xmake包仓库](https://github.com/xmake-io/xmake-repo)
- [自定义包教程](https://xmake.io/#/zh-cn/package/remote_package)
- [社区论坛](https://github.com/xmake-io/xmake/discussions)

---

**← [返回工具链主页](../README.md)**
