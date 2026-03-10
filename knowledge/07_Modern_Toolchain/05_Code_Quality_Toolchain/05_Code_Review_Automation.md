# 代码审查自动化

> **层级定位**: 07 Modern Toolchain / 05 Code Quality
> **难度级别**: L3 进阶
> **预估学习时间**: 2-3小时

---

## 📋 审查流程

### GitHub Flow

```
创建分支 → 提交代码 → 创建PR → 自动检查 → 人工审查 → 合并
```

### 自动化检查清单

- [ ] CI构建通过
- [ ] 测试通过
- [ ] 静态分析无错误
- [ ] 代码格式化检查
- [ ] 覆盖率不下降
- [ ] 安全扫描通过

---

## 🚀 GitHub Actions集成

```yaml
name: PR Checks

on:
  pull_request:
    branches: [main]

jobs:
  check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Build
        run: cmake -B build && cmake --build build

      - name: Test
        run: ctest --test-dir build

      - name: Format Check
        run: |
          find . -name "*.c" | xargs clang-format -i
          git diff --exit-code

      - name: Static Analysis
        run: cppcheck --error-exitcode=1 src/
```

---

## 📋 审查清单模板

### C代码审查清单

**功能性**:

- [ ] 功能实现正确
- [ ] 边界条件处理
- [ ] 错误处理完善

**安全性**:

- [ ] 无缓冲区溢出风险
- [ ] 输入验证
- [ ] 资源正确释放

**代码质量**:

- [ ] 命名清晰
- [ ] 函数长度适中
- [ ] 注释充分

---

**← [返回工具链主页](../README.md)**
