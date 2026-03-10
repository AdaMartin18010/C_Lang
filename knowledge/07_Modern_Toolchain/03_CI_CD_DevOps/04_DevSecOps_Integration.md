# DevSecOps集成指南

> **层级定位**: 07 Modern Toolchain / 03 CI/CD DevOps
> **难度级别**: L4 专家
> **预估学习时间**: 4-6小时

---

## 📋 DevSecOps概述

将安全实践融入CI/CD流程：

- **SAST**: 静态应用安全测试
- **DAST**: 动态应用安全测试
- **SCA**: 软件成分分析
- **Secret Detection**: 密钥泄露检测
- **Container Security**: 容器安全扫描

---

## 🚀 安全扫描集成

### SAST工具

#### CodeQL (GitHub)

```yaml
# .github/workflows/codeql.yml
name: CodeQL

on: [push, pull_request]

jobs:
  analyze:
    runs-on: ubuntu-latest
    permissions:
      actions: read
      contents: read
      security-events: write
    steps:
    - uses: actions/checkout@v4
    - uses: github/codeql-action/init@v3
      with:
        languages: cpp
    - uses: github/codeql-action/analyze@v3
```

#### SonarQube

```yaml
# .github/workflows/sonar.yml
- name: SonarQube Scan
  uses: SonarSource/sonarqube-scan-action@master
  env:
    SONAR_TOKEN: ${{ secrets.SONAR_TOKEN }}
```

### 依赖漏洞扫描

#### Snyk

```yaml
- name: Snyk Security Scan
  uses: snyk/actions/setup@master
- run: snyk test --file=CMakeLists.txt
  env:
    SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}
- run: snyk monitor
  env:
    SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}
```

### 密钥检测

#### GitLeaks

```yaml
- name: GitLeaks
  uses: gitleaks/gitleaks-action@v2
  env:
    GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

#### TruffleHog

```yaml
- name: TruffleHog
  uses: trufflesecurity/trufflehog@main
  with:
    path: ./
    base: main
    head: HEAD
```

---

## 🔒 安全实践

### 安全编码检查

```bash
# 编译器安全选项
add_compile_options(
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -Wformat
    -Wformat-security
    -fPIE
)

# 链接器安全选项
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -Wl,-z,relro,-z,now")
```

### 模糊测试集成

```bash
# libFuzzer示例
clang -fsanitize=fuzzer,address fuzz_target.c -o fuzz_target
./fuzz_target -max_total_time=300
```

---

## 📊 安全指标

| 指标 | 目标 | 工具 |
|:-----|:----:|:-----|
| 高危漏洞 | 0 | Snyk, CodeQL |
| 密钥泄露 | 0 | GitLeaks |
| 代码覆盖率 | >80% | gcov |
| 安全测试通过率 | 100% | 自定义测试 |

---

## 🛡️ 完整工作流示例

```yaml
name: Security Pipeline

on: [push, pull_request]

jobs:
  sast:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: CodeQL
        uses: github/codeql-action/analyze@v3
      - name: Cppcheck
        run: cppcheck --enable=all src/

  sca:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Snyk
        uses: snyk/actions/node@master

  secrets:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: GitLeaks
        uses: gitleaks/gitleaks-action@v2
```

---

## ✅ 安全检查清单

- [ ] SAST集成
- [ ] 依赖漏洞扫描
- [ ] 密钥泄露检测
- [ ] 容器安全扫描
- [ ] SBOM生成
- [ ] 模糊测试
- [ ] 安全编码规范

---

**← [返回工具链主页](../README.md)**
