# 目录结构与格式修复计划

> **日期**: 2026-05-10
> **状态**: 部分完成，持续推进中

---

## 已完成的格式修复

### 批量修复（脚本自动化）

| 修复类型 | 数量 | 说明 |
|----------|------|------|
| 删除末尾模板填充 | ~80个文件 | 删除"深入理解/核心原理/实践应用/最佳实践"占位符 |
| multi_h1降级 | ~60个文件 | 将多余H1降级为H2 |

### 手动修复

| 文件 | 修复内容 |
|------|----------|
| `knowledge/README.md` | 删除末尾多余fence |
| `knowledge/11_Machine_Learning_C/03_Embedded_AI_Deployment.md` | 添加缺失的代码块闭合、删除末尾模板 |
| `knowledge/05_C_Language_Standards/C23_C2Y_ROADMAP.md` | 修正LLVM 22→LLVM 20 |

---

## 剩余的目录结构问题

### P0: 编号系统混乱

**问题**: `03_System_Technology_Domains` 子目录编号全部重复（多个子目录使用相同前缀编号）。

**影响**: 导航索引失效，文件归属不清。

**建议修复方案**:

```
当前（混乱）:
03_System_Technology_Domains/
├── 03_Edge_Detection.md
├── 03_Hardware_Acceleration.md
├── 03_JIT_Regex.md
├── ...

建议（统一）:
03_System_Technology_Domains/
├── 01_Computer_Vision/
│   ├── 01_Edge_Detection.md
│   └── 02_Hardware_Acceleration.md
├── 02_Regex_Engines/
│   └── 01_JIT_Regex.md
├── ...
```

### P1: 顶级目录编号重复

**问题**: 04~20编号在knowledge/根目录下有重复（如04_Industrial_Scenarios和04_Standard_Library_Frameworks）。

**影响**: 知识图谱索引失效。

### P2: 缺失README

**问题**: ~203个子目录缺失README.md。

**影响**: 目录导航困难。

### P3: 文件命名风格混杂

**问题**: snake_case、kebab-case、中文标题、英文标题混用。

**影响**: 一致性差，链接易错。

---

## 可持续推进策略

### 短期（1-2周）

- [ ] 完成unclosed_fence修复（剩余3-5个文件）
- [ ] 统一C23/C2y相关文件的命名风格
- [ ] 为所有新增文档创建交叉链接

### 中期（1个月）

- [ ] 修复03_System_Technology_Domains编号混乱
- [ ] 为关键目录补写README
- [ ] 建立文件命名规范文档

### 长期（持续）

- [ ] 开发自动化格式检查脚本（集成到CI）
- [ ] 建立外部事件驱动更新机制
- [ ] 每季度进行一次全面格式审计

---

## 命名规范建议（草案）

### 文件命名

```
首选: 02_C23_Core_Features.md        # snake_case + 编号
次选: 02-c23-core-features.md        # kebab-case + 编号
避免: 02 C23 核心特性.md             # 空格 + 中文
避免: 02C23CoreFeatures.md           # 无分隔符
```

### 目录命名

```
首选: 07_Modern_C/                    # 编号 + snake_case
次选: 07-modern-c/                    # 编号 + kebab-case
```

### 文档头部元数据

```markdown
> **最后更新**: YYYY-MM-DD
> **文档版本**: X.Y.Z
> **维护状态**: 活跃更新 / 稳定 / 归档
```
