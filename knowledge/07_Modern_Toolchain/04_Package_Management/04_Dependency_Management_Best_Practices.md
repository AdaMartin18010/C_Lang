# C语言依赖管理最佳实践

> **层级定位**: 07 Modern Toolchain / 04 Package Management
> **难度级别**: L3 进阶
> **预估学习时间**: 2-3小时

---

## 📋 依赖管理策略对比

| 策略 | 优点 | 缺点 | 适用场景 |
|:-----|:-----|:-----|:---------|
| **系统包管理器** | 简单、与系统集成 | 版本受限、跨平台难 | 小型项目 |
| **Git子模块** | 版本控制、明确 | 更新麻烦、构建复杂 | 固定依赖 |
| **源码复制(vendoring)** | 完全控制 | 代码膨胀、更新困难 | 关键依赖 |
| **包管理器** | 自动化、版本管理 | 学习成本 | 推荐方案 |

---

## 🎯 推荐方案

### 现代C项目推荐

```text
首选: 包管理器 (Conan/vcpkg/xrepo)
补充: Git子模块 (少量、关键依赖)
避免: 源码复制 (维护困难)
```

### 版本锁定策略

**Conan**: `conan.lock`
**vcpkg**: `vcpkg-configuration.json`
**xrepo**: 自动锁定

### 安全更新流程

1. 监控CVE安全公告
2. 测试环境先更新
3. 运行完整测试套件
4. 代码审查后合并

---

## 📦 依赖版本管理策略

### 语义化版本控制

```text
版本格式: MAJOR.MINOR.PATCH

MAJOR: 不兼容的API更改
MINOR: 向后兼容的功能添加
PATCH: 向后兼容的问题修复
```

**版本约束示例**:

```ini
# Conan
[requires]
zlib/1.2.13          # 精确版本
openssl/[>=1.1 <2.0] # 版本范围
```

```json
// vcpkg
{
  "dependencies": [
    {
      "name": "zlib",
      "version>=": "1.2.13"
    }
  ]
}
```

### 版本锁定文件

**Conan lockfile**:

```bash
# 生成锁定文件
conan install . --lockfile-out=conan.lock

# 使用锁定文件重建
conan install . --lockfile=conan.lock
```

**vcpkg manifest**:

```json
{
  "name": "my-project",
  "version": "1.0.0",
  "dependencies": ["zlib", "openssl"],
  "builtin-baseline": "a1b2c3d4"
}
```

---

## 🔒 依赖安全最佳实践

### CVE漏洞监控

**工具推荐**:

```bash
# OWASP Dependency-Check
dependency-check.sh --project myproject --scan .

# Snyk CLI
snyk test

# GitHub Dependabot (自动PR)
# 在项目设置中启用
```

### 供应链安全

1. **验证依赖来源**
   - 使用官方仓库
   - 验证包签名
   - 检查维护者信誉

2. **最小化依赖**
   - 只引入必要的依赖
   - 定期审查无用依赖
   - 优先选择单头文件库

3. **私有依赖管理**

   ```bash
   # Conan私服
   conan remote add private http://artifactory.company.com

   # vcpkg注册表
   vcpkg x-add-registry https://github.com/company/registry
   ```

---

## 🔄 CI/CD集成

### 自动化依赖更新

**Dependabot配置** (`.github/dependabot.yml`):

```yaml
version: 2
updates:
  - package-ecosystem: "github-actions"
    directory: "/"
    schedule:
      interval: "weekly"

  - package-ecosystem: "pip"
    directory: "/scripts"
    schedule:
      interval: "daily"
```

### 依赖缓存策略

**GitHub Actions缓存**:

```yaml
- name: Cache Conan packages
  uses: actions/cache@v4
  with:
    path: ~/.conan/data
    key: ${{ runner.os }}-conan-${{ hashFiles('**/conanfile.txt') }}
```

---

## 📝 依赖管理清单

### 项目初始化

- [ ] 选择包管理器 (Conan/vcpkg/xrepo)
- [ ] 创建清单文件 (conanfile.txt/vcpkg.json)
- [ ] 配置版本约束
- [ ] 生成锁定文件
- [ ] 文档化依赖安装步骤

### 日常维护

- [ ] 每月检查依赖更新
- [ ] 审查CVE公告
- [ ] 更新锁定文件
- [ ] 测试依赖升级
- [ ] 清理无用依赖

### 安全审计

- [ ] 运行漏洞扫描
- [ ] 验证依赖签名
- [ ] 审查新增依赖
- [ ] 更新安全补丁

---

## 🎓 决策指南

```text
项目类型判断:
├─ 小型项目 (< 5个依赖)
│  └─ 考虑: Git子模块或vcpkg baseline
├─ 中型项目 (5-20个依赖)
│  └─ 推荐: Conan + lockfile
├─ 大型项目 (20+ 依赖)
│  └─ 推荐: Conan私服 + 自动化更新
└─ 企业项目
   └─ 推荐: Artifactory/Nexus + 安全扫描
```

---

**← [返回工具链主页](../README.md)**
