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
    steps:
    - uses: actions/checkout@v4
    - uses: github/codeql-action/init@v2
      with:
        languages: cpp
    - uses: github/codeql-action/analyze@v2
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
```

### 密钥检测

#### GitLeaks

```yaml
- name: GitLeaks
  uses: zricethezav/gitleaks-action@master
```

---

## ✅ 安全检查清单

- [ ] SAST集成
- [ ] 依赖漏洞扫描
- [ ] 密钥泄露检测
- [ ] 容器安全扫描
- [ ] SBOM生成

---

**← [返回工具链主页](../README.md)**
