# 内存保护单元 (Memory Protection Unit, MPU)

---

## 🔗 知识关联网络

### 1. 全局导航

| 层级 | 文档 | 作用 |
|:-----|:-----|:-----|
| 总索引 | [../../../00_GLOBAL_INDEX.md](../../../00_GLOBAL_INDEX.md) | 完整知识图谱入口 |
| 本模块 | [../../../readme.md](../../../readme.md) | 模块总览与导航 |
| 学习路径 | [../../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 推荐学习路线 |

### 2. 前置知识依赖

| 文档 | 关系 | 掌握要求 |
|:-----|:-----|:---------|
| [../../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md](../../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 语言基础 | 必须掌握 |
| [../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心机制 | 必须掌握 |
| [../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 内存基础 | 必须掌握 |

### 3. 同层横向关联

| 文档 | 关系 | 互补内容 |
|:-----|:-----|:---------|
| [../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 技术扩展 | 并发编程技术 |
| [../../../02_Formal_Semantics_and_Physics/readme.md](../../../02_Formal_Semantics_and_Physics/readme.md) | 理论支撑 | 形式化方法 |
| [../../../04_Industrial_Scenarios/readme.md](../../../04_Industrial_Scenarios/readme.md) | 实践应用 | 工业实践案例 |

### 4. 深层理论关联

| 文档 | 关系 | 理论深度 |
|:-----|:-----|:---------|
| [../../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义基础 | 操作语义、类型理论 |
| [../../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 概念映射 | 知识关联网络 |

### 5. 后续进阶延伸

| 文档 | 关系 | 进阶方向 |
|:-----|:-----|:---------|
| [../../../03_System_Technology_Domains/readme.md](../../../03_System_Technology_Domains/readme.md) | 系统技术 | 系统级开发 |
| [../../../01_Core_Knowledge_System/09_Safety_Standards/readme.md](../../../01_Core_Knowledge_System/09_Safety_Standards/readme.md) | 安全标准 | 安全编码规范 |
| [../../../07_Modern_Toolchain/readme.md](../../../07_Modern_Toolchain/readme.md) | 工具链 | 现代开发工具 |

### 6. 阶段学习定位

```
当前位置: 根据文档主题确定学习阶段
├─ 入门阶段: 基础语法、数据类型
├─ 基础阶段: 控制流程、函数
├─ 进阶阶段: 指针、内存管理 ⬅️ 可能在此
├─ 高级阶段: 并发、系统编程
└─ 专家阶段: 形式验证、编译器
```

### 7. 局部索引

本文件所属模块的详细内容：

- 参见本模块 [readme.md](../../../readme.md)
- 相关子目录文档


## 1. 概述

内存保护单元(MPU)是一种轻量级的内存保护机制，主要用于嵌入式系统和实时操作系统(RTOS)。与完整的内存管理单元(MMU)不同，MPU不提供虚拟地址到物理地址的转换，而是专注于内存访问权限的保护。

### 1.1 MPU vs MMU

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                         MPU vs MMU 对比                                                  │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                              MPU (内存保护单元)                                  │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   功能:                                                                           │  │
│   │   • 内存访问权限控制 (读/写/执行)                                                 │  │
│   │   • 内存区域属性设置 (缓存/非缓存)                                                │  │
│   │   • 无地址转换 - 使用物理地址                                                     │  │
│   │                                                                                 │  │
│   │   地址处理流程:                                                                   │  │
│   │   ┌──────────────┐         ┌──────────────┐         ┌──────────────┐           │  │
│   │   │  物理地址    │ ──────> │     MPU      │ ──────> │  物理内存    │           │  │
│   │   │  (输入)     │         │  (权限检查)   │         │  (访问)     │           │  │
│   │   └──────────────┘         └──────────────┘         └──────────────┘           │  │
│   │          │                        │                                               │  │
│   │          │              ┌─────────┴─────────┐                                     │  │
│   │          │              ▼                   ▼                                     │  │
│   │          │        [允许]                 [拒绝]                                    │  │
│   │          │           │                     │                                      │  │
│   │          │           ▼                     ▼                                      │  │
│   │          │      继续访问内存          触发异常(Fault)                              │  │
│   │                                                                                 │  │
│   │   特点:                                                                          │  │
│   │   • 确定性 - 无TLB未命中，访问时间固定                                           │  │
│   │   • 简单 - 硬件复杂度低，面积小，功耗低                                          │  │
│   │   • 实时友好 - 适合硬实时系统                                                    │  │
│   │   • 内存有限 - 通常支持8-16个保护区域                                            │  │
│   │                                                                                 │  │
│   │   应用: 嵌入式系统、实时操作系统、安全关键系统                                    │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                              MMU (内存管理单元)                                  │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   功能:                                                                           │  │
│   │   • 虚拟地址到物理地址转换                                                        │  │
│   │   • 内存访问权限控制                                                              │  │
│   │   • 内存属性设置                                                                  │  │
│   │   • 按需分页、交换、内存映射文件                                                  │  │
│   │                                                                                 │  │
│   │   地址处理流程:                                                                   │  │
│   │   ┌──────────────┐         ┌──────────────┐         ┌──────────────┐           │  │
│   │   │  虚拟地址    │ ──────> │     MMU      │ ──────> │  物理内存    │           │  │
│   │   │  (输入)     │         │ (TLB+页表)   │         │  (访问)     │           │  │
│   │   └──────────────┘         └──────────────┘         └──────────────┘           │  │
│   │          │                   │    │                                               │  │
│   │          │         ┌─────────┘    └─────────┐                                     │  │
│   │          │         ▼                       ▼                                     │  │
│   │          │   [TLB命中]                [TLB未命中]                                  │  │
│   │          │         │                       │                                      │  │
│   │          │         ▼                       ▼                                      │  │
│   │          │    快速完成              页表遍历(慢)                                   │  │
│   │                                                                                 │  │
│   │   特点:                                                                          │  │
│   │   • 灵活性 - 支持大虚拟地址空间，进程隔离                                        │  │
│   │   • 复杂性 - 需要TLB、页表、复杂硬件                                             │  │
│   │   • 非确定性 - TLB未命中导致访问时间变化                                         │  │
│   │   • 内存开销 - 页表占用内存                                                      │  │
│   │                                                                                 │  │
│   │   应用: 通用操作系统、服务器、桌面系统、高端移动设备                              │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   关键差异总结                                                                   │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   特性           │    MPU         │    MMU                                       │  │
│   │   ────────────────────────────────────────────────────────                      │  │
│   │   地址转换       │    无           │    有 (VA -> PA)                           │  │
│   │   虚拟内存       │    不支持       │    支持                                    │  │
│   │   内存区域数     │    8-16个       │    无限制 (由内存限制)                     │  │
│   │   确定性延迟     │    是           │    否 (TLB未命中)                          │  │
│   │   硬件复杂度     │    低           │    高                                      │  │
│   │   功耗           │    低           │    较高                                    │  │
│   │   页表内存开销   │    无           │    有                                      │  │
│   │   适用场景       │    嵌入式/实时  │    通用计算                                │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 MPU在系统中的位置

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                    MPU在处理器系统中的位置                                               │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                                                                                 │  │
│   │   ┌─────────────────────────────────────────────────────────────────────────┐  │  │
│   │   │                      Processor Core                                      │  │  │
│   │   │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────────┐  │  │  │
│   │   │  │ Fetch   │  │ Decode  │  │ Execute │  │ Memory  │  │ Writeback   │  │  │  │
│   │   │  │ Stage   │  │ Stage   │  │ Stage   │  │ Stage   │  │ Stage       │  │  │  │
│   │   │  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘  └─────────────┘  │  │  │
│   │   │       │            │            │            │                         │  │  │
│   │   │       └────────────┴────────────┴────────────┘                         │  │  │
│   │   │                          │                                               │  │  │
│   │   │                          ▼                                               │  │  │
│   │   │                ┌─────────────────┐                                       │  │  │
│   │   │                │  Physical Addr  │                                       │  │  │
│   │   │                │   (物理地址)     │                                       │  │  │
│   │   │                └────────┬────────┘                                       │  │  │
│   │   └─────────────────────────┼────────────────────────────────────────────────┘  │  │
│   │                             │                                                   │  │
│   │                             ▼                                                   │  │
│   │   ┌─────────────────────────────────────────────────────────────────────────┐  │  │
│   │   │                              MPU                                        │  │  │
│   │   │  ┌─────────────────────────────────────────────────────────────────┐   │  │  │
│   │   │  │                    Region Lookup Logic                          │   │  │  │
│   │   │  │                                                                  │   │  │  │
│   │   │  │   Physical Address                                               │   │  │  │
│   │   │  │        │                                                         │   │  │  │
│   │   │  │        ▼                                                         │   │  │  │
│   │   │  │   ┌──────────────────────────────────────────────────────────┐  │   │  │  │
│   │   │  │   │  Region 0  │  Region 1  │  ...  │  Region N-1           │  │   │  │  │
│   │   │  │   │ Base/Limit │ Base/Limit │       │ Base/Limit            │  │   │  │  │
│   │   │  │   │   Attr     │   Attr     │       │   Attr                │  │   │  │  │
│   │   │  │   └─────┬──────┴─────┬──────┴───────┴───────┬─────────────────┘  │   │  │  │
│   │   │  │         │            │                      │                    │   │  │  │
│   │   │  │         └────────────┴──────────────────────┘                    │   │  │  │
│   │   │  │                          │                                       │   │  │  │
│   │   │  │                          ▼                                       │   │  │  │
│   │   │  │   ┌──────────────────────────────────────────────────────────┐  │   │  │  │
│   │   │  │   │              Address Comparator Array                     │  │   │  │  │
│   │   │  │   │  (并行比较地址与所有区域的Base/Limit)                     │  │   │  │  │
│   │   │  │   └──────────────────────────┬───────────────────────────────┘  │   │  │  │
│   │   │  │                              │                                   │   │  │  │
│   │   │  │                              ▼                                   │   │  │  │
│   │   │  │   ┌──────────────────────────────────────────────────────────┐  │   │  │  │
│   │   │  │   │                 Access Control Check                      │  │   │  │  │
│   │   │  │   │  (检查特权级、读写执行权限、缓存属性)                     │  │   │  │  │
│   │   │  │   └──────────────────────────┬───────────────────────────────┘  │   │  │  │
│   │   │  │                              │                                   │   │  │  │
│   │   │  │                  ┌───────────┴───────────┐                       │   │  │  │
│   │   │  │                  ▼                       ▼                       │   │  │  │
│   │   │  │            [Allow]                  [Deny]                       │   │  │  │
│   │   │  │               │                       │                          │   │  │  │
│   │   │  │               ▼                       ▼                          │   │  │  │
│   │   │  │         Continue Access       Generate Fault                     │   │  │  │
│   │   │  │                                                                  │   │  │  │
│   │   │  └──────────────────────────────────────────────────────────────────┘   │  │  │
│   │   │                                                                         │  │  │
│   │   │  配置寄存器:                                                              │  │  │
│   │   │  • MPU_TYPE: 区域数量                                                     │  │  │
│   │   │  • MPU_CTRL: MPU使能、默认内存映射                                        │  │  │
│   │   │  • RBAR (Region Base Address): 区域基址和编号                             │  │  │
│   │   │  • RASR (Region Attribute and Size Register): 属性和大小                  │  │  │
│   │   │                                                                         │  │  │
│   │   └─────────────────────────────────────────────────────────────────────────┘  │  │
│   │                             │                                                   │  │
│   │                             ▼                                                   │  │
│   │   ┌─────────────────────────────────────────────────────────────────────────┐  │  │
│   │   │                        Memory System                                    │  │  │
│   │   │  ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐         │  │  │
│   │   │  │   SRAM   │    │  Flash   │    │  Periph  │    │ External │         │  │  │
│   │   │  │          │    │          │    │          │    │  Memory  │         │  │  │
│   │   │  └──────────┘    └──────────┘    └──────────┘    └──────────┘         │  │  │
│   │   └─────────────────────────────────────────────────────────────────────────┘  │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

## 2. 区域属性配置

### 2.1 MPU区域属性详解

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                    MPU区域属性配置                                                       │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   ARMv7-M MPU (Cortex-M3/M4/M7) 区域属性寄存器 (RASR)                           │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   ┌────────┬────────┬────────┬────────┬────────┬────────┬────────┬────────┐  │  │
│   │   │ 31:29  │ 28     │ 27:24  │   23   │ 22:19  │  18    │ 17:16  │ 15:8   │  │  │
│   │   │  Res   │  XN    │  AP    │   Res  │  TEX   │  S     │  C     │  B     │  │  │
│   │   ├────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┤  │  │
│   │   │   3b   │  1b    │  4b    │   1b   │  4b    │  1b    │  1b    │  8b    │  │  │
│   │   └────────┴────────┴────────┴────────┴────────┴────────┴────────┴────────┘  │  │
│   │                                                                                 │  │
│   │   字段说明:                                                                      │  │
│   │                                                                                 │  │
│   │   XN (Execute Never): 执行禁止位                                                │  │
│   │   • 0 = 可执行                                                                  │  │
│   │   • 1 = 不可执行 (数据区推荐)                                                   │  │
│   │                                                                                 │  │
│   │   AP (Access Permission): 访问权限                                              │  │
│   │   ┌───────────┬──────────────┬──────────────┐                                  │  │
│   │   │   AP[2:0] │  特权模式    │   用户模式   │                                  │  │
│   │   ├───────────┼──────────────┼──────────────┤                                  │  │
│   │   │    000    │  无访问      │   无访问     │                                  │  │
│   │   │    001    │  读写        │   无访问     │                                  │  │
│   │   │    010    │  读写        │   只读       │                                  │  │
│   │   │    011    │  读写        │   读写       │                                  │  │
│   │   │    101    │  只读        │   无访问     │                                  │  │
│   │   │    110    │  只读        │   只读       │                                  │  │
│   │   └───────────┴──────────────┴──────────────┘                                  │  │
│   │                                                                                 │  │
│   │   TEX, C, B (Type Extension, Cacheable, Bufferable): 内存类型                  │  │
│   │   ┌───────────┬───────┬───────┬────────────────────────────────┐               │  │
│   │   │   TEX     │   C   │   B   │   内存类型                     │               │  │
│   │   ├───────────┼───────┼───────┼────────────────────────────────┤               │  │
│   │   │   000     │   0   │   0   │   Strongly-ordered             │               │  │
│   │   │   000     │   0   │   1   │   Shareable Device             │               │  │
│   │   │   000     │   1   │   0   │   Outer/Inner Write-Through    │               │  │
│   │   │   000     │   1   │   1   │   Outer/Inner Write-Back       │               │  │
│   │   │   001     │   0   │   0   │   Outer/Inner Non-cacheable    │               │  │
│   │   │   010     │   0   │   0   │   Non-shareable Device         │               │  │
│   │   │   011     │   1   │   1   │   Write-Back (Write-Allocate)  │               │  │
│   │   │   ...     │  ...  │  ...  │   ...                          │               │  │
│   │   └───────────┴───────┴───────┴────────────────────────────────┘               │  │
│   │                                                                                 │  │
│   │   S (Shareable): 共享属性                                                       │  │
│   │   • 0 = 非共享                                                                  │  │
│   │   • 1 = 共享 (多核系统)                                                         │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   RISC-V PMP (Physical Memory Protection) 配置                                  │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   PMPcfg CSR (8个，每个配置4个PMP条目):                                         │  │
│   │   ┌────────┬────────┬────────┬────────┬────────┬────────┬────────┬────────┐  │  │
│   │   │  7:6   │   5    │   4    │   3    │   2    │   1    │   0    │        │  │  │
│   │   │  Res   │   L    │  Res   │  X     │   W    │   R    │  A[1:0]│        │  │  │
│   │   ├────────┼────────┼────────┼────────┼────────┼────────┼────────┼────────┤  │  │
│   │   │   2b   │   1b   │   1b   │   1b   │   1b   │   1b   │   2b   │        │  │  │
│   │   └────────┴────────┴────────┴────────┴────────┴────────┴────────┴────────┘  │  │
│   │                                                                                 │  │
│   │   字段说明:                                                                      │  │
│   │                                                                                 │  │
│   │   R (Read), W (Write), X (Execute): 权限位                                      │  │
│   │   • 0 = 禁止该访问                                                              │  │
│   │   • 1 = 允许该访问                                                              │  │
│   │                                                                                 │  │
│   │   A (Address Matching): 地址匹配模式                                            │  │
│   │   • 00 = OFF (条目禁用)                                                         │  │
│   │   • 01 = TOR (Top of Range) - 以该地址为上界，以下一区域为下界                 │  │
│   │   • 10 = NA4 (Naturally aligned 4-byte region)                                  │  │
│   │   • 11 = NAPOT (Naturally aligned power-of-2 region)                            │  │
│   │                                                                                 │  │
│   │   L (Lock): 锁定位                                                              │  │
│   │   • 0 = 可在S/M模式下修改                                                       │  │
│   │   • 1 = 锁定，只能在M模式下修改，复位前不能解锁                                 │  │
│   │                                                                                 │  │
│   │   PMPaddr CSR: 区域地址寄存器                                                   │  │
│   │   • 包含区域地址的高34位 (低2位隐含为0)                                         │  │
│   │   • 具体含义取决于A字段的配置                                                   │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 MPU区域配置示例

```c
/*
 * ARM Cortex-M MPU 配置示例
 */

#include <stdint.h>

// MPU寄存器定义 (Cortex-M3/M4)
#define MPU_BASE        0xE000ED90

#define MPU_TYPER       (*(volatile uint32_t *)(MPU_BASE + 0x00))
#define MPU_CTRL        (*(volatile uint32_t *)(MPU_BASE + 0x04))
#define MPU_RNR         (*(volatile uint32_t *)(MPU_BASE + 0x08))
#define MPU_RBAR        (*(volatile uint32_t *)(MPU_BASE + 0x0C))
#define MPU_RASR        (*(volatile uint32_t *)(MPU_BASE + 0x10))

// MPU_CTRL位
#define MPU_CTRL_ENABLE     (1 << 0)
#define MPU_CTRL_HFNMIENA   (1 << 1)
#define MPU_CTRL_PRIVDEFENA (1 << 2)

// RASR位域
#define RASR_XN             (1 << 28)
#define RASR_AP_SHIFT       24
#define RASR_TEX_SHIFT      19
#define RASR_S              (1 << 18)
#define RASR_C              (1 << 17)
#define RASR_B              (1 << 16)
#define RASR_SRD_SHIFT      8
#define RASR_SIZE_SHIFT     1
#define RASR_ENABLE         (1 << 0)

// 访问权限
#define AP_NO_ACCESS        0x0
#define AP_PRIV_RW          0x1
#define AP_PRIV_RW_USER_RO  0x2
#define AP_PRIV_RW_USER_RW  0x3
#define AP_PRIV_RO          0x5
#define AP_PRIV_RO_USER_RO  0x6

/*
 * MPU区域大小编码
 * 实际大小 = 2^(SIZE+1) bytes
 */
typedef enum {
    MPU_SIZE_32B    = 0x04,   // 2^5 = 32B
    MPU_SIZE_64B    = 0x05,   // 2^6 = 64B
    MPU_SIZE_128B   = 0x06,   // 2^7 = 128B
    MPU_SIZE_256B   = 0x07,   // 2^8 = 256B
    MPU_SIZE_512B   = 0x08,   // 2^9 = 512B
    MPU_SIZE_1KB    = 0x09,   // 2^10 = 1KB
    MPU_SIZE_2KB    = 0x0A,   // 2^11 = 2KB
    MPU_SIZE_4KB    = 0x0B,   // 2^12 = 4KB
    MPU_SIZE_8KB    = 0x0C,   // 2^13 = 8KB
    MPU_SIZE_16KB   = 0x0D,   // 2^14 = 16KB
    MPU_SIZE_32KB   = 0x0E,   // 2^15 = 32KB
    MPU_SIZE_64KB   = 0x0F,   // 2^16 = 64KB
    MPU_SIZE_128KB  = 0x10,   // 2^17 = 128KB
    MPU_SIZE_256KB  = 0x11,   // 2^18 = 256KB
    MPU_SIZE_512KB  = 0x12,   // 2^19 = 512KB
    MPU_SIZE_1MB    = 0x13,   // 2^20 = 1MB
    MPU_SIZE_2MB    = 0x14,   // 2^21 = 2MB
    MPU_SIZE_4MB    = 0x15,   // 2^22 = 4MB
    MPU_SIZE_8MB    = 0x16,   // 2^23 = 8MB
    MPU_SIZE_16MB   = 0x17,   // 2^24 = 16MB
    MPU_SIZE_32MB   = 0x18,   // 2^25 = 32MB
    MPU_SIZE_64MB   = 0x19,   // 2^26 = 64MB
    MPU_SIZE_128MB  = 0x1A,   // 2^27 = 128MB
    MPU_SIZE_256MB  = 0x1B,   // 2^28 = 256MB
    MPU_SIZE_512MB  = 0x1C,   // 2^29 = 512MB
    MPU_SIZE_1GB    = 0x1D,   // 2^30 = 1GB
    MPU_SIZE_2GB    = 0x1E,   // 2^31 = 2GB
    MPU_SIZE_4GB    = 0x1F,   // 2^32 = 4GB
} mpu_region_size_t;

/*
 * 配置MPU区域
 */
void mpu_configure_region(uint32_t region_num,
                          uint32_t base_addr,
                          mpu_region_size_t size,
                          uint32_t attributes) {
    // 选择区域
    MPU_RNR = region_num;

    // 设置基址 (必须对齐到区域大小)
    MPU_RBAR = base_addr & 0xFFFFFFE0;

    // 设置属性
    MPU_RASR = attributes | (size << RASR_SIZE_SHIFT) | RASR_ENABLE;
}

/*
 * 典型的嵌入式系统MPU配置
 */
void mpu_setup_typical(void) {
    /*
     * 配置区域:
     *
     * Region 0: 代码段 (Flash)
     * - 基址: 0x08000000
     * - 大小: 512KB
     * - 属性: 只读执行，可缓存
     */
    mpu_configure_region(0,
                         0x08000000,
                         MPU_SIZE_512KB,
                         (AP_PRIV_RO_USER_RO << RASR_AP_SHIFT) |  // 只读
                         RASR_C | RASR_B);                         // 可缓存

    /*
     * Region 1: 数据段 (SRAM)
     * - 基址: 0x20000000
     * - 大小: 128KB
     * - 属性: 读写，可缓存
     */
    mpu_configure_region(1,
                         0x20000000,
                         MPU_SIZE_128KB,
                         (AP_PRIV_RW_USER_RW << RASR_AP_SHIFT) |  // 读写
                         RASR_XN |                                  // 不可执行
                         RASR_C | RASR_B);                          // 可缓存

    /*
     * Region 2: 栈区 (SRAM的一部分)
     * - 基址: 0x20020000
     * - 大小: 32KB
     * - 属性: 读写，可缓存
     */
    mpu_configure_region(2,
                         0x20020000,
                         MPU_SIZE_32KB,
                         (AP_PRIV_RW << RASR_AP_SHIFT) |  // 特权读写，用户无访问
                         RASR_XN |                          // 不可执行
                         RASR_C | RASR_B);

    /*
     * Region 3: 外设区域
     * - 基址: 0x40000000
     * - 大小: 512MB (通常根据具体外设调整)
     * - 属性: 读写，不可缓存，设备内存
     */
    mpu_configure_region(3,
                         0x40000000,
                         MPU_SIZE_512MB,
                         (AP_PRIV_RW << RASR_AP_SHIFT) |  // 特权读写
                         RASR_XN |                          // 不可执行
                         (2 << RASR_TEX_SHIFT));            // 设备内存

    /*
     * Region 4: 系统控制块 (SCB, NVIC等)
     * - 基址: 0xE0000000
     * - 大小: 1MB
     * - 属性: 读写，强有序 (设备内存)
     */
    mpu_configure_region(4,
                         0xE0000000,
                         MPU_SIZE_1MB,
                         (AP_PRIV_RW << RASR_AP_SHIFT) |
                         RASR_XN |
                         (0 << RASR_TEX_SHIFT));  // 强有序

    /*
     * 启用MPU
     * - 使能MPU
     * - 启用默认内存映射 (未配置区域按默认属性访问)
     * - 允许在HardFault/NMI中使用MPU
     */
    MPU_CTRL = MPU_CTRL_ENABLE | MPU_CTRL_PRIVDEFENA | MPU_CTRL_HFNMIENA;

    /*
     * 屏障指令确保MPU配置生效
     */
    __DSB();  // 数据同步屏障
    __ISB();  // 指令同步屏障
}

/*
 * 保护堆栈溢出检测
 * 在栈底设置一个 guard page
 */
void mpu_setup_stack_guard(void *stack_bottom) {
    /*
     * Region 7: 栈保护页 (guard page)
     * - 大小: 32字节 (最小)
     * - 属性: 无访问权限
     *
     * 当栈溢出时访问此区域会触发MemManage Fault
     */
    mpu_configure_region(7,
                         (uint32_t)stack_bottom - 32,
                         MPU_SIZE_32B,
                         (AP_NO_ACCESS << RASR_AP_SHIFT) |
                         RASR_XN);
}
```

## 3. 嵌入式系统应用

### 3.1 FreeRTOS MPU集成

```c
/*
 * FreeRTOS与MPU集成示例
 * 实现任务级别的内存隔离
 */

#include "FreeRTOS.h"
#include "task.h"
#include "mpu_wrappers.h"

/*
 * 受MPU保护的任务创建
 */
void create_protected_task(void) {
    /*
     * 定义任务的内存区域
     */
    static const xMemoryRegion xRegions[] = {
        // 区域1: 任务的栈
        {
            .pvBaseAddress = (void *)0x20010000,
            .ulLengthInBytes = 8192,
            .ulParameters = (portMPU_REGION_READ_WRITE |
                           portMPU_REGION_CACHEABLE_BUFFERABLE)
        },
        // 区域2: 任务私有的数据区
        {
            .pvBaseAddress = (void *)0x20012000,
            .ulLengthInBytes = 4096,
            .ulParameters = (portMPU_REGION_READ_WRITE |
                           portMPU_REGION_CACHEABLE_BUFFERABLE)
        },
        // 区域3: 与其他任务共享的数据区 (只读)
        {
            .pvBaseAddress = (void *)0x20014000,
            .ulLengthInBytes = 4096,
            .ulParameters = (portMPU_REGION_READ_ONLY |
                           portMPU_REGION_CACHEABLE_BUFFERABLE)
        }
    };

    TaskParameters_t xTaskParameters = {
        .pvTaskCode = protected_task_function,
        .pcName = "ProtectedTask",
        .usStackDepth = 256,  // 以字为单位
        .pvParameters = NULL,
        .uxPriority = 1,
        .puxStackBuffer = (StackType_t *)0x20010000,  // 预分配栈
        .xRegions = xRegions,
        .mpuParameters = {
            .privileges = tskMPU_READ_WRITE  // 任务运行在特权模式
        }
    };

    TaskHandle_t xHandle;
    xTaskCreateRestricted(&xTaskParameters, &xHandle);
}

/*
 * 受保护的任务函数
 * 只能访问其被授权的区域
 */
void protected_task_function(void *pvParameters) {
    /*
     * 可以访问:
     * - 自己的栈
     * - 自己的数据区
     * - 共享数据区 (只读)
     *
     * 不能访问:
     * - 其他任务的栈或数据
     * - 系统关键区域
     */

    volatile uint32_t *my_data = (uint32_t *)0x20012000;
    volatile const uint32_t *shared_data = (uint32_t *)0x20014000;

    for (;;) {
        // 允许: 访问自己的数据
        *my_data = 0x12345678;

        // 允许: 读取共享数据
        uint32_t value = *shared_data;

        // 会触发MemManage Fault: 写入共享数据区 (只读)
        // *shared_data = 0xDEADBEEF;  // 禁止!

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*
 * MemManage Fault处理程序
 */
void MemManage_Handler(void) {
    /*
     * 读取MPU错误状态寄存器
     */
    uint32_t mmfsr = *(volatile uint32_t *)0xE000ED28;
    uint32_t mmar = *(volatile uint32_t *)0xE000ED34;

    if (mmfsr & 0x01) {
        // IACCVIOL: 指令访问违规
        printf("Instruction access violation at 0x%08X\n", mmar);
    }
    if (mmfsr & 0x02) {
        // DACCVIOL: 数据访问违规
        printf("Data access violation at 0x%08X\n", mmar);
    }
    if (mmfsr & 0x08) {
        // MUNSTKERR: 异常退出时堆栈错误
        printf("Unstacking error\n");
    }
    if (mmfsr & 0x10) {
        // MSTKERR: 异常进入时堆栈错误
        printf("Stacking error\n");
    }

    /*
     * 可以选择:
     * 1. 终止违规任务 (如果支持)
     * 2. 重启系统
     * 3. 记录错误并继续
     */

    // 清除错误标志
    *(volatile uint32_t *)0xE000ED28 = mmfsr;

    // 挂起或删除违规任务
    // vTaskDelete(xTaskGetCurrentTaskHandle());
}
```

### 3.2 安全关键系统应用

```c
/*
 * 安全关键系统中的MPU应用
 * 符合IEC 61508 / ISO 26262 要求
 */

/*
 * 安全分区架构
 */

// 定义安全分区
typedef enum {
    PARTITION_ASIL_D,   // 最高安全等级 (制动、转向)
    PARTITION_ASIL_B,   // 中等安全等级 (仪表)
    PARTITION_QM,       // 质量管理 (信息娱乐)
    PARTITION_NUM
} safety_partition_t;

/*
 * 每个分区的MPU配置
 */
typedef struct {
    uint32_t code_base;
    uint32_t code_size;
    uint32_t data_base;
    uint32_t data_size;
    uint32_t stack_base;
    uint32_t stack_size;
    uint32_t permissions;
} partition_mpu_config_t;

static const partition_mpu_config_t partition_config[PARTITION_NUM] = {
    [PARTITION_ASIL_D] = {
        .code_base = 0x08000000,
        .code_size = 256 * 1024,   // 256KB
        .data_base = 0x20000000,
        .data_size = 128 * 1024,   // 128KB
        .stack_base = 0x20020000,
        .stack_size = 32 * 1024,   // 32KB
        .permissions = MPU_PRIV_RO_USER_NO   // 严格隔离
    },
    [PARTITION_ASIL_B] = {
        .code_base = 0x08040000,
        .code_size = 128 * 1024,
        .data_base = 0x20040000,
        .data_size = 64 * 1024,
        .stack_base = 0x20050000,
        .stack_size = 16 * 1024,
        .permissions = MPU_PRIV_RW_USER_RO
    },
    [PARTITION_QM] = {
        .code_base = 0x08060000,
        .code_size = 512 * 1024,
        .data_base = 0x20060000,
        .data_size = 256 * 1024,
        .stack_base = 0x20090000,
        .stack_size = 64 * 1024,
        .permissions = MPU_PRIV_RW_USER_RW
    }
};

/*
 * 上下文切换时更新MPU
 * 不同安全等级的任务使用不同的MPU配置
 */
void mpu_switch_partition(safety_partition_t new_partition) {
    const partition_mpu_config_t *cfg = &partition_config[new_partition];

    // 禁用MPU进行重新配置
    MPU->CTRL = 0;

    // 配置代码区域
    MPU->RNR = 0;
    MPU->RBAR = cfg->code_base | (1 << 4);  // 区域有效
    MPU->RASR = ((31 - __CLZ(cfg->code_size)) << 1) |  // 大小
                (AP_RO << 24) |                          // 只读
                (1 << 28);                                // XN=0 (可执行)

    // 配置数据区域
    MPU->RNR = 1;
    MPU->RBAR = cfg->data_base | (1 << 4);
    MPU->RASR = ((31 - __CLZ(cfg->data_size)) << 1) |
                (cfg->permissions << 24) |
                (1 << 28);  // XN=1 (不可执行)

    // 配置栈区域
    MPU->RNR = 2;
    MPU->RBAR = cfg->stack_base | (1 << 4);
    MPU->RASR = ((31 - __CLZ(cfg->stack_size)) << 1) |
                (AP_RW << 24) |
                (1 << 28);

    // 启用MPU
    MPU->CTRL = MPU_CTRL_ENABLE | MPU_CTRL_PRIVDEFENA;

    __DSB();
    __ISB();
}

/*
 * 安全监控: 检测未授权访问尝试
 */
void safety_monitor_hook(void) {
    uint32_t cfsr = SCB->CFSR;

    if (cfsr & SCB_CFSR_MMARVALID_Msk) {
        uint32_t fault_addr = SCB->MMFAR;
        uint32_t task_id = get_current_task_id();

        // 记录安全事件
        log_safety_event(SEV_MEM_FAULT, task_id, fault_addr);

        // 根据安全策略处理
        switch (get_current_partition()) {
            case PARTITION_ASIL_D:
                // 最高安全分区出错，进入安全状态
                enter_safe_state();
                break;
            case PARTITION_ASIL_B:
                // 尝试恢复或重启分区
                restart_partition(PARTITION_ASIL_B);
                break;
            case PARTITION_QM:
                // 非安全分区出错，仅记录
                log_event("QM partition fault");
                break;
        }
    }
}
```

## 4. ARM Cortex-M MPU

### 4.1 Cortex-M MPU架构

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                    ARM Cortex-M MPU 架构                                                 │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                                                                                 │  │
│   │   Cortex-M3/M4: 支持8个区域                                                        │  │
│   │   Cortex-M7: 支持16个区域                                                        │  │
│   │   Cortex-M23/M33: 支持8个区域 (支持TrustZone)                                   │  │
│   │                                                                                 │  │
│   │   架构图:                                                                         │  │
│   │                                                                                 │  │
│   │   ┌─────────────────────────────────────────────────────────────────────────┐  │  │
│   │   │                      Cortex-M Processor                                 │  │  │
│   │   │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐                   │  │  │
│   │   │  │  NVIC   │  │  Core   │  │  Debug  │  │  MPU    │                   │  │  │
│   │   │  │         │  │         │  │         │  │         │                   │  │  │
│   │   │  └─────────┘  └────┬────┘  └─────────┘  └────┬────┘                   │  │  │
│   │   │                    │                         │                         │  │  │
│   │   │                    │                         │                         │  │  │
│   │   │                    └───────────┬─────────────┘                         │  │  │
│   │   │                                │                                       │  │  │
│   │   │                                ▼                                       │  │  │
│   │   │   ┌─────────────────────────────────────────────────────────────────┐ │  │  │
│   │   │   │                         MPU                                    │ │  │  │
│   │   │   │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────────────┐   │ │  │  │
│   │   │   │  │ Region  │  │ Region  │  │ Region  │  │  Access Control │   │ │  │  │
│   │   │   │  │  0      │  │  1      │  │  2..7   │  │     Logic       │   │ │  │  │
│   │   │   │  │ Config  │  │ Config  │  │ Config  │  │                 │   │ │  │  │
│   │   │   │  │         │  │         │  │         │  │  - Privilege    │   │ │  │  │
│   │   │   │  │ RBAR    │  │ RBAR    │  │ RBAR    │  │  - Size/Align   │   │ │  │  │
│   │   │   │  │ RASR    │  │ RASR    │  │ RASR    │  │  - Attributes   │   │ │  │  │
│   │   │   │  └────┬────┘  └────┬────┘  └────┬────┘  └─────────────────┘   │ │  │  │
│   │   │   │       └─────────────┴─────────────┘                            │ │  │  │
│   │   │   │                     │                                          │ │  │  │
│   │   │   │                     ▼                                          │ │  │  │
│   │   │   │   ┌─────────────────────────────────────────────────────────┐ │ │  │  │
│   │   │   │   │                 Address Decoder                         │ │ │  │  │
│   │   │   │   │  (确定访问落在哪个区域)                                  │ │ │  │  │
│   │   │   │   └─────────────────────────────────────────────────────────┘ │ │  │  │
│   │   │   │                                                                 │ │  │  │
│   │   │   └─────────────────────────────────────────────────────────────────┘ │  │  │
│   │   │                                                                         │  │  │
│   │   │   寄存器:                                                                 │  │  │
│   │   │   • MPU_TYPER: 区域数量和特性                                           │  │  │
│   │   │   • MPU_CTRL: MPU使能控制                                               │  │  │
│   │   │   • MPU_RNR:  区域号选择                                                │  │  │
│   │   │   • MPU_RBAR: 区域基址寄存器                                            │  │  │
│   │   │   • MPU_RASR: 区域属性和大小寄存器                                      │  │  │
│   │   │                                                                         │  │  │
│   │   └─────────────────────────────────────────────────────────────────────────┘  │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   MPU_TYPER 寄存器                                                              │  │
│   │   ────────────────                                                              │  │
│   │                                                                                 │  │
│   │   ┌────────────┬────────────┬───────────────────────────────────────────────┐  │  │
│   │   │   31:16    │    15:8    │                    7:0                        │  │  │
│   │   │  IREGION   │  DREGION   │                 Reserved                      │  │  │
│   │   ├────────────┼────────────┼───────────────────────────────────────────────┤  │  │
│   │   │  指令区域数 │  数据区域数 │                                               │  │  │
│   │   │  (通常为0) │ (Cortex-M3/4: 0x08, Cortex-M7: 0x10)                        │  │  │
│   │   └────────────┴────────────┴───────────────────────────────────────────────┘  │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   MPU_CTRL 寄存器                                                               │  │
│   │   ────────────────                                                              │  │
│   │                                                                                 │  │
│   │   ┌────────────┬────────────┬────────────┬──────────────────────────────────┐  │  │
│   │   │   31:3     │     2      │     1      │         0                        │  │  │
│   │   │  Reserved  │PRIVDEFENA  │ HFNMIENA   │       ENABLE                     │  │  │
│   │   ├────────────┼────────────┼────────────┼──────────────────────────────────┤  │  │
│   │   │            │  特权默认  │ HardFault  │  MPU使能                         │  │  │
│   │   │            │  内存映射  │ NMI使能    │  0=禁用, 1=使能                  │  │  │
│   │   │            │  0=禁用    │ 0=禁用     │                                  │  │  │
│   │   │            │  1=启用    │ 1=启用     │                                  │  │  │
│   │   └────────────┴────────────┴────────────┴──────────────────────────────────┘  │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 Cortex-M MPU编程模型

```c
/*
 * ARM Cortex-M MPU 完整编程接口
 */

#ifndef MPU_H
#define MPU_H

#include <stdint.h>

// MPU寄存器结构体
typedef struct {
    volatile uint32_t TYPE;     // 0xE000ED90
    volatile uint32_t CTRL;     // 0xE000ED94
    volatile uint32_t RNR;      // 0xE000ED98
    volatile uint32_t RBAR;     // 0xE000ED9C
    volatile uint32_t RASR;     // 0xE000EDA0
} MPU_Type;

#define MPU_BASE        0xE000ED90
#define MPU             ((MPU_Type *)MPU_BASE)

// RASR寄存器位定义
#define RASR_XN_Pos             28
#define RASR_AP_Pos             24
#define RASR_TEX_Pos            19
#define RASR_S_Pos              18
#define RASR_C_Pos              17
#define RASR_B_Pos              16
#define RASR_SRD_Pos            8
#define RASR_SIZE_Pos           1
#define RASR_ENABLE_Pos         0

// 访问权限定义
#define RASR_AP_NO_ACCESS       (0x0 << RASR_AP_Pos)
#define RASR_AP_PRIV_RW         (0x1 << RASR_AP_Pos)
#define RASR_AP_PRIV_RW_USER_RO (0x2 << RASR_AP_Pos)
#define RASR_AP_PRIV_RW_USER_RW (0x3 << RASR_AP_Pos)
#define RASR_AP_PRIV_RO         (0x5 << RASR_AP_Pos)
#define RASR_AP_PRIV_RO_USER_RO (0x6 << RASR_AP_Pos)
#define RASR_AP_PRIV_RO_USER_RW (0x7 << RASR_AP_Pos)

// 内存类型定义
#define RASR_STRONGLY_ORDERED   ((0x0 << RASR_TEX_Pos) | (0 << RASR_C_Pos) | (0 << RASR_B_Pos))
#define RASR_SHAREABLE_DEVICE   ((0x0 << RASR_TEX_Pos) | (0 << RASR_C_Pos) | (1 << RASR_B_Pos))
#define RASR_WRITE_THROUGH      ((0x0 << RASR_TEX_Pos) | (1 << RASR_C_Pos) | (0 << RASR_B_Pos))
#define RASR_WRITE_BACK         ((0x0 << RASR_TEX_Pos) | (1 << RASR_C_Pos) | (1 << RASR_B_Pos))
#define RASR_NON_CACHEABLE      ((0x1 << RASR_TEX_Pos) | (0 << RASR_C_Pos) | (0 << RASR_B_Pos))
#define RASR_WRITE_BACK_ALLOC   ((0x1 << RASR_TEX_Pos) | (1 << RASR_C_Pos) | (1 << RASR_B_Pos))

/*
 * 初始化MPU
 */
void mpu_init(void) {
    // 禁用MPU
    MPU->CTRL = 0;

    // 检查MPU是否可用
    uint32_t type = MPU->TYPE;
    uint32_t dregion = (type >> 8) & 0xFF;

    if (dregion == 0) {
        // MPU不可用
        return;
    }

    // 禁用所有区域
    for (uint32_t i = 0; i < dregion; i++) {
        MPU->RNR = i;
        MPU->RASR = 0;
    }
}

/*
 * 启用MPU
 */
void mpu_enable(uint32_t privdefena, uint32_t hfnmiena) {
    uint32_t ctrl = MPU_CTRL_ENABLE_Msk;

    if (privdefena) {
        ctrl |= MPU_CTRL_PRIVDEFENA_Msk;
    }

    if (hfnmiena) {
        ctrl |= MPU_CTRL_HFNMIENA_Msk;
    }

    MPU->CTRL = ctrl;

    // 内存屏障
    __DSB();
    __ISB();
}

/*
 * 禁用MPU
 */
void mpu_disable(void) {
    MPU->CTRL = 0;
    __DSB();
    __ISB();
}

/*
 * 配置MPU区域 (完整版)
 */
void mpu_config_region_full(uint32_t region_num,
                            uint32_t base_addr,
                            uint32_t size_log2,  // 实际大小 = 2^(size_log2+1)
                            uint32_t attributes,
                            uint32_t srd_mask,   // 子区域禁用掩码
                            uint32_t enable) {
    // 选择区域
    MPU->RNR = region_num;

    // 设置基址
    // VALID=1, REGION=0 (使用RNR选择的区域)
    MPU->RBAR = (base_addr & 0xFFFFFFE0) | 0x10;

    // 设置属性和大小
    MPU->RASR = (attributes) |
                ((srd_mask & 0xFF) << RASR_SRD_Pos) |
                ((size_log2 & 0x1F) << RASR_SIZE_Pos) |
                (enable ? 1 : 0);
}

/*
 * 便捷的MPU配置宏
 */
#define MPU_CONFIG_CODE(addr, size) \
    mpu_config_region_full(region++, addr, size, \
        RASR_AP_PRIV_RO_USER_RO | RASR_WRITE_THROUGH, 0, 1)

#define MPU_CONFIG_DATA(addr, size) \
    mpu_config_region_full(region++, addr, size, \
        RASR_AP_PRIV_RW_USER_RW | RASR_WRITE_BACK | (1 << RASR_XN_Pos), 0, 1)

#define MPU_CONFIG_DEVICE(addr, size) \
    mpu_config_region_full(region++, addr, size, \
        RASR_AP_PRIV_RW | RASR_SHAREABLE_DEVICE | (1 << RASR_XN_Pos), 0, 1)

#define MPU_CONFIG_STRONGLY_ORDERED(addr, size) \
    mpu_config_region_full(region++, addr, size, \
        RASR_AP_PRIV_RW | RASR_STRONGLY_ORDERED | (1 << RASR_XN_Pos), 0, 1)

#endif // MPU_H
```

## 5. RISC-V PMP

### 5.1 RISC-V PMP架构

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                    RISC-V PMP (Physical Memory Protection)                               │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   PMP特点                                                                       │  │
│   │   ───────                                                                        │  │
│   │                                                                                 │  │
│   │   • 最多64个PMP条目 (标准支持0-64个，由实现决定)                                 │  │
│   │   • 每个PMP条目由一个配置寄存器和一个地址寄存器组成                             │  │
│   │   • 支持M、S、U三种特权模式                                                      │  │
│   │   • 仅用于物理地址保护，不进行地址转换                                           │  │
│   │   • 支持地址范围匹配: TOR (Top of Range)、NA4、NAPOT                             │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   PMP CSR寄存器布局                                                             │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   配置寄存器 (pmpcfg0-pmpcfg15, 每个配置4个条目):                               │  │
│   │                                                                                 │  │
│   │   pmpcfg0 (8 bits × 4 = 32 bits):                                               │  │
│   │   ┌────────┬────────┬────────┬────────┐                                        │  │
│   │   │cfg[3]  │cfg[2]  │cfg[1]  │cfg[0]  │                                        │  │
│   │   │ 8bits  │ 8bits  │ 8bits  │ 8bits  │                                        │  │
│   │   └────────┴────────┴────────┴────────┘                                        │  │
│   │                                                                                 │  │
│   │   每个cfg[i] (8 bits):                                                          │  │
│   │   ┌────┬────┬────┬────┬────┬────┬────┬────┐                                    │  │
│   │   │ L  │ 0  │ 0  │ A1 │ A0 │ X  │ W  │ R  │                                    │  │
│   │   │ 7  │ 6  │ 5  │ 4  │ 3  │ 2  │ 1  │ 0  │                                    │  │
│   │   ├────┼────┼────┼────┼────┼────┼────┼────┤                                    │  │
│   │   │锁定│保留│保留│地址匹配│ 执行│ 写入│ 读取│                                    │  │
│   │   └────┴────┴────┴────┴────┴────┴────┴────┘                                    │  │
│   │                                                                                 │  │
│   │   地址寄存器 (pmpaddr0-pmpaddr63):                                              │  │
│   │   • 64位架构: 54位地址 (右移2位，所以存储[55:2])                                │  │
│   │   • 32位架构: 34位地址 (右移2位，所以存储[33:2])                                │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   地址匹配模式详解                                                              │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   1. OFF (A=00): 禁用                                                          │  │
│   │      此PMP条目不匹配任何地址                                                    │  │
│   │                                                                                 │  │
│   │   2. TOR (Top of Range, A=01):                                                 │  │
│   │      地址范围: (pmpaddr[i-1] × 4) 到 (pmpaddr[i] × 4)                          │  │
│   │      pmpaddr[i-1]不包含在内 (如果i=0，则从0开始)                               │  │
│   │                                                                                 │  │
│   │      示例:                                                                       │  │
│   │      pmpaddr0 = 0x2000 (8KB)                                                   │  │
│   │      pmpaddr1 = 0x4000 (16KB)                                                  │  │
│   │      cfg[1] = TOR -> 区域: 0x8000 到 0x10000 (8KB到16KB)                       │  │
│   │                                                                                 │  │
│   │   3. NA4 (Naturally Aligned 4-byte, A=10):                                     │  │
│   │      4字节自然对齐区域                                                          │  │
│   │      pmpaddr[i]必须4字节对齐                                                    │  │
│   │      区域: (pmpaddr[i] × 4) 到 (pmpaddr[i] × 4 + 3)                            │  │
│   │      主要用于锁定单个指令或数据访问                                             │  │
│   │                                                                                 │  │
│   │   4. NAPOT (Naturally Aligned Power-of-2, A=11):                               │  │
│   │      自然对齐的2的幂次大小区域                                                  │  │
│   │      使用"G...G"编码: 低位为1表示大小                                          │  │
│   │                                                                                 │  │
│   │      编码:                                                                       │  │
│   │      pmpaddr = base_addr >> 2 | (size/4 - 1)                                   │  │
│   │                                                                                 │  │
│   │      示例:                                                                       │  │
│   │      4KB区域 @ 0x10000:                                                        │  │
│   │      base = 0x10000, size = 4096                                               │  │
│   │      pmpaddr = (0x10000 >> 2) | (4096/4 - 1)                                   │  │
│   │               = 0x4000 | 0x3FF                                                 │  │
│   │               = 0x43FF                                                         │  │
│   │      二进制: ...0100 0011 1111 1111                                            │  │
│   │                   │  └── 低位9个1表示大小 (2^10 = 4KB)                         │  │
│   │                   └── base的高22位                                             │  │
│   │                                                                                 │  │
│   │      区域大小计算: 如果最低N位为1，大小为 2^(N+3) bytes                        │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   PMP优先级规则                                                                 │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   1. 条目号越大优先级越高 (pmpaddr0 < pmpaddr1 < ... < pmpaddr63)              │  │
│   │                                                                                 │  │
│   │   2. 第一个匹配的条目决定访问权限                                               │  │
│   │                                                                                 │  │
│   │   3. 如果没有条目匹配:                                                          │  │
│   │      • M模式: 总是允许访问                                                      │  │
│   │      • S/U模式: 触发访问异常                                                    │  │
│   │                                                                                 │  │
│   │   图示:                                                                          │  │
│   │                                                                                 │  │
│   │   Address ──> [Match Entry 0?] ──No──> [Match Entry 1?] ──No──> ...           │  │
│   │                      │                            │                             │  │
│   │                    Yes                          Yes                            │  │
│   │                      │                            │                             │  │
│   │                      ▼                            ▼                             │  │
│   │               Check Permission             Check Permission                    │  │
│   │                      │                            │                             │  │
│   │                      ▼                            ▼                             │  │
│   │              [Allow/Deny/Fault]          [Allow/Deny/Fault]                    │  │
│   │                                                                                 │  │
│   │   所有条目都不匹配:                                                             │  │
│   │   M模式 ──> 允许                                                               │  │
│   │   S/U模式 ──> 触发异常                                                        │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 5.2 RISC-V PMP编程

```c
/*
 * RISC-V PMP 配置示例
 */

#include <stdint.h>

// PMP CSR编号
#define CSR_PMPCFG0     0x3A0
#define CSR_PMPCFG1     0x3A1
#define CSR_PMPCFG2     0x3A2
#define CSR_PMPCFG3     0x3A3
#define CSR_PMPADDR0    0x3B0
#define CSR_PMPADDR1    0x3B1
// ... 到 CSR_PMPADDR63 0x3EF

/*
 * 读取/写入PMP配置CSR
 */
#define read_csr(csr) ({ \
    uint64_t __v; \
    __asm__ __volatile__ ("csrr %0, " #csr : "=r" (__v)); \
    __v; \
})

#define write_csr(csr, val) ({ \
    uint64_t __v = (uint64_t)(val); \
    __asm__ __volatile__ ("csrw " #csr ", %0" : : "rK" (__v)); \
})

#define set_csr(csr, val) ({ \
    uint64_t __v = (uint64_t)(val); \
    __asm__ __volatile__ ("csrs " #csr ", %0" : : "rK" (__v)); \
})

/*
 * PMP配置结构
 */
typedef struct {
    uint8_t r : 1;      // 读权限
    uint8_t w : 1;      // 写权限
    uint8_t x : 1;      // 执行权限
    uint8_t a : 2;      // 地址匹配模式 (0=OFF, 1=TOR, 2=NA4, 3=NAPOT)
    uint8_t reserved : 2;
    uint8_t l : 1;      // 锁定
} pmpcfg_t;

#define PMP_A_OFF       0
#define PMP_A_TOR       1
#define PMP_A_NA4       2
#define PMP_A_NAPOT     3

/*
 * 将地址转换为NAPOT格式的pmpaddr值
 */
uint64_t pmpaddr_napot_encode(uint64_t addr, uint64_t size) {
    // addr必须是size对齐的
    // size必须是2的幂次
    return (addr >> 2) | ((size >> 3) - 1);
}

/*
 * 配置单个PMP条目
 */
void pmp_config_entry(uint32_t entry_num,
                      uint64_t pmpaddr_val,
                      pmpcfg_t cfg) {
    // 写入pmpaddr
    write_csr(CSR_PMPADDR0 + entry_num, pmpaddr_val);

    // 计算pmpcfg中的位置
    uint32_t cfg_reg = entry_num / 4;
    uint32_t cfg_shift = (entry_num % 4) * 8;

    // 读取当前cfg值
    uint64_t cfg_val = read_csr(CSR_PMPCFG0 + cfg_reg);

    // 清除旧的配置，设置新的配置
    cfg_val &= ~(0xFFULL << cfg_shift);
    cfg_val |= ((uint64_t)(*(uint8_t *)&cfg)) << cfg_shift;

    // 写回
    write_csr(CSR_PMPCFG0 + cfg_reg, cfg_val);
}

/*
 * 典型的嵌入式系统PMP配置
 */
void pmp_setup_embedded(void) {
    pmpcfg_t cfg;

    /*
     * Entry 0: 闪存 (代码) - 只读执行
     * 范围: 0x20000000 - 0x20080000 (512KB)
     */
    cfg.r = 1; cfg.w = 0; cfg.x = 1;
    cfg.a = PMP_A_NAPOT; cfg.l = 0;
    pmp_config_entry(0,
        pmpaddr_napot_encode(0x20000000, 512 * 1024),
        cfg);

    /*
     * Entry 1: SRAM (数据) - 读写
     * 范围: 0x80000000 - 0x80040000 (256KB)
     */
    cfg.r = 1; cfg.w = 1; cfg.x = 0;
    cfg.a = PMP_A_NAPOT; cfg.l = 0;
    pmp_config_entry(1,
        pmpaddr_napot_encode(0x80000000, 256 * 1024),
        cfg);

    /*
     * Entry 2: 外设 - 读写 (不可执行)
     * 范围: 0x10000000 - 0x20000000 (256MB)
     */
    cfg.r = 1; cfg.w = 1; cfg.x = 0;
    cfg.a = PMP_A_NAPOT; cfg.l = 0;
    pmp_config_entry(2,
        pmpaddr_napot_encode(0x10000000, 256 * 1024 * 1024),
        cfg);

    /*
     * Entry 3: 栈保护 - 无访问 (作为guard page)
     * 范围: 0x8003F000 - 0x80040000 (4KB在SRAM末尾)
     */
    cfg.r = 0; cfg.w = 0; cfg.x = 0;
    cfg.a = PMP_A_NAPOT; cfg.l = 0;
    pmp_config_entry(3,
        pmpaddr_napot_encode(0x8003F000, 4 * 1024),
        cfg);

    /*
     * Entry 4: M模式代码 - 锁定
     * 确保即使后续代码也无法修改此配置
     */
    cfg.r = 1; cfg.w = 1; cfg.x = 1;
    cfg.a = PMP_A_NAPOT; cfg.l = 1;  // 锁定!
    pmp_config_entry(4,
        pmpaddr_napot_encode(0x20000000, 64 * 1024),
        cfg);
}

/*
 * TOR模式配置示例
 */
void pmp_setup_tor_example(void) {
    pmpcfg_t cfg;

    /*
     * 使用TOR模式创建连续区域
     *
     * Entry 0: 从0到8KB (保留，无访问)
     * Entry 1: 从8KB到32KB (代码，只读执行)
     * Entry 2: 从32KB到64KB (数据，读写)
     */

    // Entry 0: 设置上界 (不包含在区域中)
    // pmpaddr0 = 0x2000 (8KB >> 2)
    cfg.r = 0; cfg.w = 0; cfg.x = 0;
    cfg.a = PMP_A_OFF; cfg.l = 0;  // 先禁用
    pmp_config_entry(0, 0x2000 >> 2, cfg);

    // Entry 1: 8KB到32KB (代码)
    // pmpaddr1 = 0x8000 (32KB >> 2)
    cfg.r = 1; cfg.w = 0; cfg.x = 1;
    cfg.a = PMP_A_TOR; cfg.l = 0;
    pmp_config_entry(1, 0x8000 >> 2, cfg);

    // Entry 2: 32KB到64KB (数据)
    // pmpaddr2 = 0x10000 (64KB >> 2)
    cfg.r = 1; cfg.w = 1; cfg.x = 0;
    cfg.a = PMP_A_TOR; cfg.l = 0;
    pmp_config_entry(2, 0x10000 >> 2, cfg);
}

/*
 * PMP异常处理
 */
void handle_pmp_fault(void) {
    // 读取mcause确定异常类型
    uint64_t mcause;
    __asm__ volatile ("csrr %0, mcause" : "=r" (mcause));

    if ((mcause & 0xFF) == 1) {
        // Instruction access fault
        uint64_t mepc;
        __asm__ volatile ("csrr %0, mepc" : "=r" (mepc));
        printf("PMP Instruction fault at 0x%lx\n", mepc);
    }
    else if ((mcause & 0xFF) == 5) {
        // Load access fault
        uint64_t mtval;
        __asm__ volatile ("csrr %0, mtval" : "=r" (mtval));
        printf("PMP Load fault at 0x%lx\n", mtval);
    }
    else if ((mcause & 0xFF) == 7) {
        // Store/AMO access fault
        uint64_t mtval;
        __asm__ volatile ("csrr %0, mtval" : "=r" (mtval));
        printf("PMP Store fault at 0x%lx\n", mtval);
    }
}
```

## 6. 与C语言内存安全

### 6.1 MPU防止常见内存错误

```c
/*
 * MPU如何防止常见的C语言内存安全问题
 */

/*
 * 1. 缓冲区溢出保护
 * MPU可以在栈和堆之间设置guard page
 */
void stack_overflow_protection(void) {
    /*
     * 栈布局:
     * 高地址
     * ┌─────────────┐
     * │  Heap       │  <- 堆向上增长
     * ├─────────────┤
     * │  Guard Page │  <- MPU保护，无访问权限
     * ├─────────────┤
     * │  Stack      │  <- 栈向下增长
     * └─────────────┘
     * 低地址
     *
     * 当栈溢出时会访问Guard Page，触发异常
     */

    char buffer[100];
    // 如果写入超过100字节，会触发MPU异常
    // 而不是静默破坏堆数据
}

/*
 * 2. 空指针解引用保护
 * 将地址0设置为不可访问
 */
void null_pointer_protection(void) {
    // MPU配置: 地址0-4KB无访问权限

    int *ptr = NULL;
    *ptr = 42;  // 触发MPU异常，而不是写入随机位置
}

/*
 * 3. 代码注入防护
 * 数据区设置为不可执行
 */
void code_injection_protection(void) {
    /*
     * MPU配置:
     * - 代码区 (Flash): 可读可执行
     * - 数据区 (SRAM): 可读可写，不可执行 (XN=1)
     * - 堆栈: 可读可写，不可执行
     *
     * 这样攻击者无法执行注入到数据区的shellcode
     */

    char shellcode[] = "\x90\x90\x90...";  // NOP sled
    void (*func)() = (void (*)())shellcode;
    // func();  // 会触发MPU异常，因为数据区不可执行
}

/*
 * 4. 堆损坏检测
 * 在堆块之间设置guard区域
 */
void heap_corruption_detection(void) {
    /*
     * 堆布局带MPU保护:
     *
     * ┌──────────────┐
     * │  Allocated   │
     * │  Block 1     │
     * ├──────────────┤
     * │  Guard Page  │  <- 防止Block 1溢出到Block 2
     * ├──────────────┤
     * │  Allocated   │
     * │  Block 2     │
     * ├──────────────┤
     * │  Guard Page  │
     * ├──────────────┤
     * │  Allocated   │
     * │  Block 3     │
     * └──────────────┘
     *
     * 缺点: 每个guard page消耗4KB内存
     * 实际系统可能只在关键边界使用
     */
}

/*
     * 5. 特权分离
 * 用户模式代码只能访问受限区域
 */
void privilege_separation(void) {
    /*
     * MPU配置示例:
     *
     * 特权模式 (内核):
     * - 可以访问所有区域
     *
     * 用户模式 (应用程序):
     * - 代码区: 只读执行
     * - 数据区: 读写
     * - 系统区域: 无访问
     *
     * 这样可以防止用户程序:
     * - 修改内核代码
     * - 直接访问硬件寄存器
     * - 访问其他进程的数据
     */
}

/*
 * 6. 防止使用未初始化指针
 * 保留特定地址范围，访问时触发异常
 */
void uninitialized_pointer_protection(void) {
    /*
     * 很多未初始化指针值为0或随机值
     * 将低地址空间(0-4KB)设置为不可访问
     * 可以捕获大部分未初始化指针使用
     */

    int *ptr;  // 未初始化
    // ...
    *ptr = 42;  // 如果ptr是0或低地址，触发异常
}
```

### 6.2 MPU与编译器优化

```c
/*
 * MPU配置对编译器优化的影响
 */

/*
 * 内存类型属性
 * 告诉编译器内存区域的属性，帮助优化
 */

// 强序内存 (设备寄存器)
#define STRONGLY_ORDERED __attribute__((section(".strongly_ordered")))

// 设备内存
#define DEVICE_MEMORY __attribute__((section(".device")))

// 可缓存内存
#define CACHEABLE_MEMORY __attribute__((section(".cacheable")))

/*
 * 使用volatile防止编译器优化设备访问
 */
volatile uint32_t * const TIMER_REG = (uint32_t *)0x40000000;

void timer_delay(uint32_t ticks) {
    uint32_t start = *TIMER_REG;
    // 如果不加volatile，编译器可能优化掉这个循环
    while ((*TIMER_REG - start) < ticks) {
        // 等待
    }
}

/*
 * 内存屏障与MPU
 * 当MPU将内存映射为不同属性时，需要屏障确保一致性
 */
void memory_barrier_example(void) {
    // 写入设备寄存器
    *DEVICE_REG = value;

    // 内存屏障 - 确保写操作完成
    __DSB();  // 数据同步屏障

    // 现在可以安全读取
    uint32_t status = *STATUS_REG;
}

/*
 * 链接器脚本与MPU区域对齐
 * 确保区域满足MPU对齐要求
 */

/*
 * linker_script.ld:
 *
 * MEMORY {
 *   FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
 *   RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 128K
 * }
 *
 * SECTIONS {
 *   .text : {
 *     *(.text*)
 *   } > FLASH
 *
 *   .data : {
 *     *(.data*)
 *   } > RAM AT > FLASH
 *
 *   // 确保栈底对齐到4KB边界 (MPU要求)
 *   __stack_bottom = ALIGN(4096);
 * }
 */
```

## 7. Verilog实现框架

```verilog
//////////////////////////////////////////////////////////////////////////////
// MPU (Memory Protection Unit) Verilog实现框架
// 支持ARM Cortex-M风格MPU
//////////////////////////////////////////////////////////////////////////////

`ifndef MPU_V
`define MPU_V

//////////////////////////////////////////////////////////////////////////////
// 参数定义
//////////////////////////////////////////////////////////////////////////////

`define MPU_REGIONS     8       // MPU区域数量
`define ADDR_BITS       32      // 地址宽度
`define REGION_SIZE_MIN 5       // 最小区域大小: 2^5 = 32 bytes

//////////////////////////////////////////////////////////////////////////////
// MPU配置寄存器格式
//////////////////////////////////////////////////////////////////////////////

// RASR寄存器位定义
`define RASR_XN_Pos     28
`define RASR_AP_Pos     24
`define RASR_TEX_Pos    19
`define RASR_S_Pos      18
`define RASR_C_Pos      17
`define RASR_B_Pos      16
`define RASR_SRD_Pos    8
`define RASR_SIZE_Pos   1
`define RASR_ENABLE_Pos 0

//////////////////////////////////////////////////////////////////////////////
// 模块: MPU Top
//////////////////////////////////////////////////////////////////////////////

module mpu_top (
    input  wire                clk,
    input  wire                rst_n,

    // 配置接口 (来自CPU/总线)
    input  wire                cfg_en,
    input  wire [3:0]          cfg_region,     // 区域号
    input  wire [31:0]         cfg_rbar,       // 基址配置
    input  wire [31:0]         cfg_rasr,       // 属性配置

    // 访问检查接口
    input  wire                check_en,       // 检查使能
    input  wire [31:0]         check_addr,     // 访问地址
    input  wire [1:0]          check_priv,     // 00=User, 11=Priv
    input  wire                check_read,     // 读访问
    input  wire                check_write,    // 写访问
    input  wire                check_exec,     // 执行访问

    // 检查结果
    output reg                 check_ok,       // 访问允许
    output reg                 check_fault,    // 访问违规
    output reg [3:0]           fault_status,   // 错误状态

    // 属性输出
    output reg [2:0]           mem_attr,       // 内存属性
    output reg                 mem_cacheable,  // 可缓存
    output reg                 mem_bufferable  // 可缓冲
);

    //////////////////////////////////////////////////////////////////////////
    // MPU区域寄存器
    //////////////////////////////////////////////////////////////////////////

    reg [31:0] region_base [`MPU_REGIONS-1:0];
    reg [31:0] region_limit [`MPU_REGIONS-1:0];
    reg [31:0] region_rasr [`MPU_REGIONS-1:0];
    reg        region_valid [`MPU_REGIONS-1:0];

    // 解码后的属性
    reg [2:0]  region_ap [`MPU_REGIONS-1:0];
    reg        region_xn [`MPU_REGIONS-1:0];
    reg [2:0]  region_memattr [`MPU_REGIONS-1:0];

    integer i;

    // 配置寄存器更新
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (i = 0; i < `MPU_REGIONS; i = i + 1) begin
                region_valid[i] <= 1'b0;
            end
        end else if (cfg_en) begin
            region_base[cfg_region] <= cfg_rbar & 32'hFFFFFFE0;
            region_rasr[cfg_region] <= cfg_rasr;
            region_valid[cfg_region] <= cfg_rasr[`RASR_ENABLE_Pos];

            // 计算区域上限
            // limit = base + (2 << size) - 1
            region_limit[cfg_region] <= (cfg_rbar & 32'hFFFFFFE0) +
                                        ((32'b1 << ((cfg_rasr >> `RASR_SIZE_Pos) & 5'h1F)) << 1) - 1;

            // 解码属性
            region_ap[cfg_region] <= (cfg_rasr >> `RASR_AP_Pos) & 3'h7;
            region_xn[cfg_region] <= (cfg_rasr >> `RASR_XN_Pos) & 1'b1;
            region_memattr[cfg_region] <= {(cfg_rasr >> `RASR_TEX_Pos) & 3'b111,
                                           (cfg_rasr >> `RASR_C_Pos) & 1'b1,
                                           (cfg_rasr >> `RASR_B_Pos) & 1'b1};
        end
    end

    //////////////////////////////////////////////////////////////////////////
    // 地址匹配逻辑
    //////////////////////////////////////////////////////////////////////////

    // 每个区域的匹配信号
    reg [`MPU_REGIONS-1:0] region_match;
    reg [3:0] matched_region;

    genvar g;
    generate
        for (g = 0; g < `MPU_REGIONS; g = g + 1) begin : gen_match
            always @(*) begin
                region_match[g] = region_valid[g] &&
                                  (check_addr >= region_base[g]) &&
                                  (check_addr <= region_limit[g]);
            end
        end
    endgenerate

    // 优先级编码 - 区域号大的优先级高
    always @(*) begin
        matched_region = 4'd15;  // 默认最高区域
        for (i = `MPU_REGIONS-1; i >= 0; i = i - 1) begin
            if (region_match[i])
                matched_region = i[3:0];
        end
    end

    wire match_found = |region_match;

    //////////////////////////////////////////////////////////////////////////
    // 权限检查逻辑
    //////////////////////////////////////////////////////////////////////////

    // 提取匹配区域的属性
    wire [2:0] matched_ap = region_ap[matched_region];
    wire       matched_xn = region_xn[matched_region];
    wire [2:0] matched_attr = region_memattr[matched_region];

    // 权限解码
    // AP[2:0]: 000=NA, 001=PrivRW, 010=PrivRW_UserRO, 011=PrivRW_UserRW,
    //          101=PrivRO, 110=PrivRO_UserRO, 111=PrivRO_UserRW
    reg priv_rw, priv_ro, user_rw, user_ro;

    always @(*) begin
        case (matched_ap)
            3'b000: {priv_rw, priv_ro, user_rw, user_ro} = 4'b0000;  // 无访问
            3'b001: {priv_rw, priv_ro, user_rw, user_ro} = 4'b1000;  // 特权读写
            3'b010: {priv_rw, priv_ro, user_rw, user_ro} = 4'b1001;  // 特权读写，用户只读
            3'b011: {priv_rw, priv_ro, user_rw, user_ro} = 4'b1011;  // 全部读写
            3'b101: {priv_rw, priv_ro, user_rw, user_ro} = 4'b0100;  // 特权只读
            3'b110: {priv_rw, priv_ro, user_rw, user_ro} = 4'b0101;  // 全部只读
            3'b111: {priv_rw, priv_ro, user_rw, user_ro} = 4'b0111;  // 特权只读，用户读写
            default: {priv_rw, priv_ro, user_rw, user_ro} = 4'b0000;
        endcase
    end

    // 检查权限
    reg read_ok, write_ok, exec_ok;

    always @(*) begin
        if (check_priv[0]) begin  // 特权模式
            read_ok  = priv_ro || priv_rw;
            write_ok = priv_rw;
            exec_ok  = !matched_xn;
        end else begin  // 用户模式
            read_ok  = user_ro || user_rw;
            write_ok = user_rw;
            exec_ok  = !matched_xn;
        end
    end

    //////////////////////////////////////////////////////////////////////////
    // 输出逻辑
    //////////////////////////////////////////////////////////////////////////

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            check_ok <= 1'b0;
            check_fault <= 1'b0;
        end else if (check_en) begin
            if (!match_found) begin
                // 没有匹配区域
                // 特权模式默认允许，用户模式默认拒绝
                check_ok <= check_priv[0];
                check_fault <= !check_priv[0];
                fault_status <= 4'b0001;  // 区域未定义
            end else begin
                // 检查具体权限
                if ((check_read && !read_ok) ||
                    (check_write && !write_ok) ||
                    (check_exec && !exec_ok)) begin
                    check_ok <= 1'b0;
                    check_fault <= 1'b1;
                    fault_status <= {check_exec, check_write, check_read, 1'b0};
                end else begin
                    check_ok <= 1'b1;
                    check_fault <= 1'b0;
                    fault_status <= 4'b0000;
                end
            end

            // 输出内存属性
            mem_attr <= matched_attr;
            mem_cacheable <= matched_attr[1];  // C位
            mem_bufferable <= matched_attr[0]; // B位
        end else begin
            check_ok <= 1'b0;
            check_fault <= 1'b0;
        end
    end

endmodule

`endif // MPU_V
```

## 8. 总结

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                         MPU关键概念总结                                                  │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   核心概念                                                                       │  │
│   │   ─────────                                                                       │  │
│   │                                                                                 │  │
│   │   1. MPU提供轻量级内存保护，无虚拟地址转换                                        │  │
│   │   2. 通过配置内存区域的访问权限(读/写/执行)实现保护                              │  │
│   │   3. 支持8-16个保护区域，适合资源受限的嵌入式系统                                 │  │
│   │   4. 访问延迟确定，适合实时系统                                                   │  │
│   │   5. 可以防止缓冲区溢出、代码注入、空指针解引用等常见安全问题                    │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   主要实现对比                                                                   │  │
│   │   ─────────────                                                                   │  │
│   │                                                                                 │  │
│   │   ARM Cortex-M MPU:                                                              │  │
│   │   • 8-16个区域，固定大小(2^n)                                                   │  │
│   │   • 基址+大小寄存器配置                                                          │  │
│   │   • 丰富的内存类型属性                                                           │  │
│   │   • 集成在NVIC中，MemManage异常处理                                              │  │
│   │                                                                                 │  │
│   │   RISC-V PMP:                                                                    │  │
│   │   • 最多64个条目(实现决定)                                                       │  │
│   │   • 支持TOR/NA4/NAPOT三种地址匹配模式                                           │  │
│   │   • 支持锁定防止后续修改                                                         │  │
│   │   • 简单的权限位(R/W/X)                                                         │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   应用场景                                                                       │  │
│   │   ─────────                                                                       │  │
│   │                                                                                 │  │
│   │   • 嵌入式RTOS的任务隔离                                                          │  │
│   │   • 安全关键系统的分区保护                                                        │  │
│   │   • 防止恶意代码执行 (XN保护)                                                     │  │
│   │   • 外设访问控制                                                                  │  │
│   │   • 栈溢出检测                                                                    │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

---

**文档信息**

- 版本: 1.0
- 创建日期: 2026-03-19
- 最后更新: 2026-03-19
- 作者: Hardware-Software Co-design Team
- 关联文档:
  - `01_MMU_Architecture.md` - MMU架构
  - `02_Page_Table_Walker.md` - 页表遍历器
  - `03_TLB_Implementation.md` - TLB实现


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
