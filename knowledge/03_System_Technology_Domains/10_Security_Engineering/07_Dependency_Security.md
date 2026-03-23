# 依赖安全与供应链安全

> **层级定位**: 03 System Technology Domains / 10 Security Engineering
> **难度级别**: L4
> **预估学习时间**: 6-8 小时

---

## 目录

- [依赖安全与供应链安全](#依赖安全与供应链安全)
  - [目录](#目录)
  - [1. 软件供应链安全概述](#1-软件供应链安全概述)
    - [1.1 供应链攻击面](#11-供应链攻击面)
    - [1.2 C/C++生态风险](#12-cc生态风险)
  - [2. 依赖管理最佳实践](#2-依赖管理最佳实践)
    - [2.1 依赖清单](#21-依赖清单)
    - [2.2 依赖固定](#22-依赖固定)
    - [2.3 漏洞扫描](#23-漏洞扫描)
  - [3. 安全编译选项](#3-安全编译选项)
    - [3.1 编译器安全标志](#31-编译器安全标志)
    - [3.2 链接器安全选项](#32-链接器安全选项)
  - [4. 安全审计清单](#4-安全审计清单)
    - [4.1 代码审计检查项](#41-代码审计检查项)
    - [4.2 自动化安全测试](#42-自动化安全测试)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 软件供应链安全概述

### 1.1 供应链攻击面

```
软件供应链攻击向量：
┌─────────────────────────────────────────────────────┐
│  源代码 → 依赖库 → 构建系统 → 分发渠道 → 运行时   │
│     │        │         │         │         │       │
│     ▼        ▼         ▼         ▼         ▼       │
│  恶意提交  漏洞库   被篡改构建  劫持下载  运行时注入 │
│  后门代码  过时组件  编译器后门  中间人    动态加载  │
└─────────────────────────────────────────────────────┘

著名攻击案例：
- SolarWinds (2020): 构建系统被植入后门
- Codecov (2021): Bash Uploader脚本被篡改
- Log4j (2021): 广泛使用组件存在严重漏洞
- XZ Utils (2024): 后门试图植入SSH
```

### 1.2 C/C++生态风险

```
C/C++特殊风险：
- 大量使用系统级库
- 依赖常以源码形式集成
- 手动内存管理引入漏洞
- 编译时宏可能隐藏恶意代码
- 复杂的构建脚本难以审计
```

---

## 2. 依赖管理最佳实践

### 2.1 依赖清单

```
# 生成完整依赖树
# Conan
conan graph info . --format=html > deps.html

# CMake with Conan
conan install . --output-folder=build

# 检查依赖许可证
conan info . --only=license
```

### 2.2 依赖固定

```
conanfile.txt
[requires]
# 固定确切版本
zlib/1.2.13@#revision_hash
openssl/3.0.8@#revision_hash

[lock]
# 使用锁文件确保可复现
conan.lock
```

### 2.3 漏洞扫描

```bash
# OWASP Dependency Check
./dependency-check.sh --project myproject --scan . --format JSON

# Snyk
snyk test --file=conanfile.py

# GitHub Dependabot
# .github/dependabot.yml
version: 2
updates:
  - package-ecosystem: "pip"
    directory: "/"
    schedule:
      interval: "weekly"
```

---

## 3. 安全编译选项

### 3.1 编译器安全标志

```cmake
# CMakeLists.txt 安全配置

# 栈保护
target_compile_options(mytarget PRIVATE
    -fstack-protector-strong    # 强栈保护
    -fstack-clash-protection    # 栈冲突保护
    -Wstack-protector           # 警告未保护函数
)

# 位置无关执行
target_compile_options(mytarget PRIVATE
    -fPIE                       # 位置无关代码
    -fPIC                       # 位置无关可执行
)
target_link_options(mytarget PRIVATE
    -pie
    -Wl,-z,relro,-z,now         # 全RELRO
)

# 控制流保护
target_compile_options(mytarget PRIVATE
    -fcf-protection=full        # 控制流保护(CET)
    -mcet                       # 启用CET
)

# 边界检查 (C23/_FORTIFY_SOURCE)
target_compile_definitions(mytarget PRIVATE
    _FORTIFY_SOURCE=3           # 强化检查
)
```

### 3.2 链接器安全选项

```cmake
# 符号和段保护
target_link_options(mytarget PRIVATE
    -Wl,-z,relro               # 重定位只读（部分RELRO）
    -Wl,-z,now                 # 立即绑定（完整RELRO）
    -Wl,-z,noexecstack         # 栈不可执行
    -Wl,-z,separate-code       # 代码段分离
    -Wl,--as-needed            # 仅链接需要的库
    -Wl,--no-copy-dt-needed-entries  # 不复制DT_NEEDED
)
```

---

## 4. 安全审计清单

### 4.1 代码审计检查项

```
□ 输入验证
  - 所有外部输入是否验证长度？
  - 字符串操作是否使用安全函数？
  - 整数运算是否检查溢出？

□ 内存安全
  - 是否使用智能指针/RAII？
  - 缓冲区操作是否检查边界？
  - 是否使用-after-free？

□ 并发安全
  - 共享数据是否同步访问？
  - 是否存在竞态条件？
  - 死锁风险？

□ 加密使用
  - 是否使用批准的算法？
  - 随机数生成器是否安全？
  - 密钥管理是否安全？

□ 依赖检查
  - 所有依赖是否有已知漏洞？
  - 依赖许可证是否兼容？
  - 依赖版本是否固定？
```

### 4.2 自动化安全测试

```yaml
# .github/workflows/security.yml
name: Security Scan

on: [push, pull_request]

jobs:
  security:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    # 静态分析
    - name: Run CodeQL
      uses: github/codeql-action/init@v2
      with:
        languages: cpp
    - uses: github/codeql-action/analyze@v2

    # 依赖扫描
    - name: Run Trivy
      uses: aquasecurity/trivy-action@master
      with:
        scan-type: 'fs'
        scan-ref: '.'
        format: 'sarif'
        output: 'trivy-results.sarif'

    # 模糊测试
    - name: Build with ASan
      run: |
        cmake -B build -DSANITIZE_ADDRESS=ON
        cmake --build build

    - name: Run Fuzz Tests
      run: |
        ./build/fuzz_target -max_total_time=300 corpus/
```

---

## 关联导航

### 前置知识

- [安全编码实践](./03_Secure_Coding_Practices.md)
- [Conan包管理](../../../01_Core_Knowledge_System/05_Engineering_Layer/05_Conan_Package_Management.md)

### 后续延伸

- [静态分析工具](../../../07_Modern_Toolchain/03_Static_Analysis_Tools.md)
- [模糊测试实践](./09_Fuzzing_Techniques.md)

### 参考

- OWASP Software Component Verification Standard
- SLSA (Supply Chain Levels for Software Artifacts)
- NIST SP 800-204
