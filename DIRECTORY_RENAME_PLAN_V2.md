# C_Lang 目录结构重命名方案 V2

> **状态**: 方案设计阶段（未执行）
> **日期**: 2026-05-13
> **原则**: 保持现有编号体系，消除跨模块重复，统一命名规范

---

## 一、问题诊断

### 1.1 跨模块一级编号重复（设计意图，非错误）

每个一级模块（01_Core, 02_Formal, 03_System...）内部独立编号，这是**有意为之**的设计。例如：

- `01_Core_Knowledge_System/01_Basic_Layer`
- `03_System_Technology_Domains/01_System_Programming`

这两个"01"分属不同模块，互不冲突。维持现状。

### 1.2 真正需要处理的问题

| 问题类型 | 具体案例 | 优先级 |
|:---------|:---------|:------:|
| 空壳目录 | `03_Computer_Vision`（3文件） | 低 |
| 跨模块主题重复 | `14_Concurrency_Parallelism` 在 01_Core 和 03_System 中并存 | 中 |
| Lean4子项目膨胀 | `10_Lean4_Compilation`（52文件，1.5MB） | 高 |
| archive/断链 | 大量归档文件链接失效 | 低 |

### 1.3 编号跳跃

`01_Core_Knowledge_System` 子目录编号：01,02,03,04,05,06,07,08,09,11,13,14,14,15,16,17,18,20,21,22,25,26,28

缺少：10,12,19,23,24,27,29,30,31,32,33

**处理建议**：编号跳跃不影响功能，无需填充。新主题应使用连续编号或语义化命名。

---

## 二、具体处理方案

### 2.1 Lean4子项目外部化（S3）

**现状**: `02_Formal_Semantics_and_Physics/10_Lean4_Compilation/` 含52个文件（1.5MB），是独立生态。

**方案**:

- 将Lean4内容迁移到 `knowledge/30_Lean4_Compilation/`（新一级模块）或独立为外部子模块
- 保留 `02_Formal_Semantics_and_Physics/` 下与C直接相关的编译映射内容（如 `06_C_Assembly_Mapping/`）
- 在原地创建 README 说明迁移

**不执行原因**: 需大规模修改交叉引用，建议放在Phase 1最后执行。

### 2.2 目录重复处理（S4）

| 目录A | 目录B | 建议 |
|:------|:------|:-----|
| `01_Core/05_Engineering_Layer` | `01_Core/05_Engineering` | 合并到 `05_Engineering_Layer` |
| `01_Core/14_Advanced_Memory` | `01_Core/14_Concurrency_Parallelism` | 重命名其中一个为 `15_Concurrency_Parallelism` |

**不执行原因**: 需修改所有交叉引用，建议与S1一并执行。

### 2.3 空壳目录处理（S2）

| 目录 | 文件数 | 内容评估 | 建议 |
|:-----|:------:|:---------|:-----|
| `03_System_Technology_Domains/03_Computer_Vision` | 3 | V4L2采集、边缘检测 | 保留，内容有实质 |
| `03_System_Technology_Domains/03_Embedded_Systems` | 1 | 实时系统 | 合并到 `19_Embedded_Systems_C` |
| `04_Industrial_Scenarios/13_Linux_Kernel` | 4 | 页表、缓存一致性、设备驱动 | 保留，内容有实质 |
| `03_System_Technology_Domains/17_Distributed_Systems` | 1 | 共识算法 | 合并到 `03_System_Technology_Domains/28_Distributed_Systems` |
| `03_System_Technology_Domains/34_Systems_Performance` | 3 | Linux调优、BPF工具、无锁编程 | 保留，内容有实质 |

---

## 三、重命名执行策略（建议暂不执行）

鉴于：

1. 项目已有1,664个Markdown文件，交叉引用复杂
2. 现有GitHub Actions CI已运行良好
3. 断链主要集中在archive/目录（旧归档内容）

**建议**:

- **短期**: 维持现有编号体系，仅处理空壳合并（S2）
- **中期**: 完成Phase 2-4内容补充后再考虑全局重命名
- **长期**: 采用语义化URL（如 `core/pointer-depth` 替代 `01_Core/02_Core_Layer/01_Pointer_Depth`）

---

## 四、Phase 1 执行优先级调整

| 任务ID | 任务 | 原优先级 | 调整后 | 原因 |
|:-------|:-----|:--------:|:------:|:-----|
| S2 | 清理幽灵目录 | P0 | P1 | 多数目录有实质内容 |
| S3 | Lean4外部化 | P0 | P2 | 影响大，内容实质存在 |
| S1 | 目录编号去重 | P0 | P2 | 跨模块重复是设计意图 |
| S4 | 合并内部重复 | P0 | P1 | 仅2处真正重复 |
| S5 | CI增强 | P0 | P0 | 已完成（C23已添加） |
| S6 | 测试脚本 | P0 | P0 | 已完成 |
| S7 | 链接验证 | P1 | P0 | 基线已生成，需修复根目录断链 |
| S8 | 模板去重 | P1 | P1 | 低影响，可延后 |
