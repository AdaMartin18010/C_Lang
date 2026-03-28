# C_Lang 项目结构

> **归档后版本**: 2026-03-29
> **核心主题**: 从图灵机到C语言的完整知识链条

---

## 根目录结构

```
C_Lang/
├── README.md                          # 项目主介绍
├── NAVIGATION.md                      # 知识库导航指南
├── CONTRIBUTING.md                    # 贡献指南
├── LICENSE                            # 许可证
├── .gitignore                         # Git忽略配置
├── AGENTS.md                          # 代理配置（待创建）
│
├── CONFIRMED_ACTION_PLAN.md           # 确认执行计划
├── PERSONAL_KNOWLEDGE_CHAIN_PLAN.md   # 个人学习计划
│
├── archive/                           # 📦 归档文件
├── examples/                          # 💻 代码示例
├── knowledge/                         # 📚 核心知识库
├── scripts/                           # 🔧 脚本工具
├── extracted_code/                    # 📂 提取的代码
└── web/                               # 🌐 Web相关
```

---

## 核心知识库 (knowledge/)

### 新增知识链条 (21-24)

```
knowledge/
├── 21_Computation_Theory/             # 🧮 计算理论基础
│   ├── README.md
│   ├── 01_Turing_Machines.md          # 图灵机
│   ├── 02_Lambda_Calculus.md          # λ演算
│   ├── 03_Computational_Complexity.md # 复杂性理论
│   └── 04_Theory_to_Physical_Bridge.md # 理论到物理桥梁 ⭐
│
├── 22_Assembler_Implementation/       # 🔧 汇编器实现
│   ├── README.md
│   ├── 01_Hack_Assembler.md           # Hack汇编器
│   ├── 02_RISCV_Assembler.md          # RISC-V汇编器
│   ├── 03_C_to_Assembly_Data.md       # C数据表示
│   └── 04_C_to_Assembly_Control.md    # C控制流
│
├── 23_VM_Implementation/              # 🖥️ 虚拟机实现
│   ├── README.md
│   ├── 01_Stack_Based_VM.md           # 栈式VM
│   └── 02_C_Subset_Interpreter.md     # C子集解释器
│
└── 24_Complete_Chain/                 # 🔗 完整链条整合
    ├── README.md
    ├── 01_From_Turing_to_C.md         # 完整回顾
    ├── 02_Layer_Bridges.md            # 层间桥梁
    └── 03_Complete_Glossary.md        # 术语表
```

### 原有知识模块 (01-20)

```
knowledge/
├── 00_INDEX.md                        # 📖 全局索引
├── 00_VERSION_TRACKING/               # 📅 版本追踪
├── 01_Core_Knowledge_System/          # 📘 C语言核心
├── 02_Formal_Semantics_and_Physics/   # 🔬 形式语义
├── 03_System_Technology_Domains/      # ⚙️ 系统技术
├── 04_Industrial_Scenarios/           # 🏭 工业场景
├── 05_Deep_Structure_MetaPhysics/     # 🔮 深层结构
├── 06_Thinking_Representation/        # 🧠 思维表达
├── 07_Modern_Toolchain/               # 🛠️ 现代工具链
├── 08_Zig_C_Connection/               # 🔗 Zig-C连接
├── 09_Data_Structures_Algorithms/     # 📊 数据结构
└── 10_WebAssembly_C/                  # 🌐 WebAssembly
```

---

## 代码示例 (examples/)

```
examples/
├── turing_machine/
│   └── tm_simulator.py                # 图灵机模拟器
├── lambda/
│   └── lambda_eval.py                 # λ演算解释器
├── assembler/
│   ├── hack_assembler.c               # Hack汇编器
│   ├── riscv_assembler.c              # RISC-V汇编器
│   ├── Makefile
│   ├── README.md
│   └── tests/                         # 测试用例
├── vm/
│   └── stack_vm.c                     # 栈式虚拟机
└── interpreter/
    └── c4.c                           # C子集解释器
```

---

## 归档文件 (archive/)

```
archive/
├── README.md                          # 归档说明
│
├── reports/                           # 📊 项目报告
│   ├── ALIGNMENT_ROADMAP_2026.md
│   ├── COMPLETION_REPORT_*.md
│   ├── PROGRESS_REPORT_*.md
│   ├── AUDIT_REPORT*.md
│   └── ... (其他报告)
│
├── roadmaps/                          # 🗺️ 路线图
│   ├── CRITICAL_EVALUATION_AND_ROADMAP_2026.md
│   ├── PROJECT_CRITIQUE_AND_ROADMAP.md
│   └── ...
│
├── tracking/                          # 📈 进度追踪
│   ├── CHANGELOG.md
│   ├── COMPUTATION_CHAIN_PROGRESS.md
│   ├── PROGRESS_TRACKER.md
│   └── ... (日志和追踪文件)
│
└── enhancement/                       # ✨ 内容增强
    ├── CONTENT_ENHANCEMENT_*.md
    └── ...
```

---

## 快速导航

### 按主题访问

| 主题 | 入口 |
|:-----|:-----|
| **计算理论** | `knowledge/21_Computation_Theory/README.md` |
| **汇编实现** | `knowledge/22_Assembler_Implementation/README.md` |
| **虚拟机** | `knowledge/23_VM_Implementation/README.md` |
| **完整链条** | `knowledge/24_Complete_Chain/README.md` |
| **C语言核心** | `knowledge/01_Core_Knowledge_System/README.md` |
| **形式语义** | `knowledge/02_Formal_Semantics_and_Physics/README.md` |
| **物理实现** | `knowledge/02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/README.md` |

### 按代码访问

| 代码 | 路径 |
|:-----|:-----|
| 图灵机模拟器 | `examples/turing_machine/tm_simulator.py` |
| λ演算解释器 | `examples/lambda/lambda_eval.py` |
| Hack汇编器 | `examples/assembler/hack_assembler.c` |
| 栈式VM | `examples/vm/stack_vm.c` |
| C解释器 | `examples/interpreter/c4.c` |

---

## 统计

| 类别 | 数量 |
|:-----|:-----|
| 核心文档 | 17个 (21-24模块) |
| 总字数 | 350,000+ 字 |
| 代码文件 | 6个 |
| 代码行数 | 2,500+ 行 |
| 知识链接 | 150+ 个 |
| 归档文件 | 100+ 个 |

---

## 使用建议

1. **新手入门**: 从 `knowledge/21_Computation_Theory/01_Turing_Machines.md` 开始
2. **实践验证**: 运行 `examples/turing_machine/tm_simulator.py`
3. **快速查找**: 使用 `NAVIGATION.md` 导航
4. **深度阅读**: 按 21→22→23→24 的顺序阅读

---

*结构整理: 2026-03-29*
*状态: 归档完成，结构清晰*
