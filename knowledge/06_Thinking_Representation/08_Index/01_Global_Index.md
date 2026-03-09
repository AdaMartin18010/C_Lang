# C语言知识库全局索引

> **层级定位**: 06 Thinking Representation / 08 Index
> **用途**: 快速查找所有知识点

---

## 按主题索引

### 核心知识 (01_Core_Knowledge_System)

| 主题 | 文件 | 难度 | 关键概念 |
|:-----|:-----|:----:|:---------|
| 基础语法 | 01_Basic_Layer/01_Hello_World.md | L1 | printf, main |
| 数据类型 | 02_Core_Layer/01_Data_Types.md | L2 | 整型, 浮点, 类型转换 |
| 指针 | 02_Core_Layer/04_Pointers.md | L3 | 地址, 解引用, 指针运算 |
| 内存管理 | 02_Core_Layer/06_Memory_Management.md | L3 | malloc, free, 堆栈 |
| 结构体 | 03_Construction_Layer/01_Structures_Unions.md | L3 | struct, union, 对齐 |
| 标准IO | 04_Standard_Library/01_File_Operations.md | L3 | fopen, fread, fwrite |
| 并发 | 04_Standard_Library/10_Threads_C11.md | L4 | thrd_t, mtx_t |
| 调试 | 05_Engineering/06_Debugging_Techniques.md | L3 | gdb, valgrind |

### 系统技术领域 (03_System_Technology_Domains)

| 主题 | 文件 | 难度 | 关键概念 |
|:-----|:-----|:----:|:---------|
| 字节码VM | 01_Virtual_Machine_Interpreter/01_Bytecode_VM.md | L4 | 指令集, 栈机 |
| 安全启动 | 06_Security_Boot/01_ARM_Trusted_Firmware.md | L5 | TrustZone |
| Raft共识 | 08_Distributed_Consensus/01_Raft_Core.md | L4 | 领导者选举 |
| 无锁队列 | 09_Performance_Logging/01_Lockless_Ring_Buffer.md | L4 | SPSC, MPMC |
| Rust互操作 | 10_Rust_Interop/01_C_ABI_Basics.md | L4 | FFI |
| B+树 | 11_In_Memory_Database/01_B_Tree_Index.md | L4 | 节点分裂 |
| RDMA | 12_RDMA_Networking/01_Verbs_API.md | L5 | InfiniBand |

### 工业场景 (04_Industrial_Scenarios)

| 主题 | 文件 | 难度 | 关键概念 |
|:-----|:-----|:----:|:---------|
| DPDK网络 | 03_High_Frequency_Trading/01_DPDK_Network_Stack.md | L5 | 零拷贝 |
| 5G SIMD | 04_5G_Baseband/01_SIMD_Vectorization.md | L5 | NEON |
| ECS架构 | 05_Game_Engine/01_ECS_Architecture.md | L5 | SoA |
| 量子模拟 | 06_Quantum_Computing/01_Quantum_Simulation_C.md | L5 | 态矢量 |
| 航天加固 | 07_Space_Computing/01_Radiation_Hardening.md | L5 | TMR |

### 深层结构 (05_Deep_Structure_MetaPhysics)

| 主题 | 文件 | 难度 | 关键概念 |
|:-----|:-----|:----:|:---------|
| 操作语义 | 01_Formal_Semantics/01_Operational_Semantics.md | L6 | 大步/小步语义 |
| 类型代数 | 02_Algebraic_Topology/01_Type_Algebra.md | L6 | 积类型, 和类型 |
| 形式验证 | 03_Verification_Energy/01_Coq_Verification.md | L6 | Hoare逻辑 |
| JIT编译 | 04_Self_Modifying_Code/01_JIT_Basics.md | L6 | 代码生成 |

---

## 学习路径推荐

```
入门：01_Basic_Layer → 02_Core_Layer → 03_Construction_Layer
进阶：02_Formal_Semantics → 03_System_Technology_Domains
专家：04_Industrial_Scenarios → 05_Deep_Structure_MetaPhysics
```

---

> **更新记录**
>
> - 2025-03-09: 创建全局索引
