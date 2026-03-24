# 页表遍历器 (Page Table Walker)

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

页表遍历器(Page Table Walker, PTW)是MMU的核心组件之一，负责在TLB未命中时执行多级页表的遍历，将虚拟地址转换为物理地址。
它是硬件实现的地址转换引擎，直接决定了内存访问的效率。

### 1.1 页表遍历器的作用

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                         页表遍历器在内存系统中的位置                                       │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                                                                                 │  │
│   │   CPU Core                ┌─────────────┐                                      │  │
│   │      │                    │    TLB      │                                      │  │
│   │      │   Virtual Address  │  Lookup     │                                      │  │
│   │      ├───────────────────>│             │                                      │  │
│   │      │                    └──────┬──────┘                                      │  │
│   │      │                           │                                            │  │
│   │      │              ┌────────────┴────────────┐                               │  │
│   │      │              ▼                         ▼                               │  │
│   │      │         [TLB Hit]                 [TLB Miss]                           │  │
│   │      │              │                         │                               │  │
│   │      │              ▼                         ▼                               │  │
│   │      │         Physical Addr         ┌─────────────────┐                       │  │
│   │      │              │                │  Page Table     │                       │  │
│   │      │              │                │   Walker        │                       │  │
│   │      │              │                │  ┌───────────┐  │                       │  │
│   │      │              │                │  │  Walk FSM │  │                       │  │
│   │      │              │                │  │  (状态机)  │  │                       │  │
│   │      │              │                │  ├───────────┤  │                       │  │
│   │      │              │                │  │  Address  │  │                       │  │
│   │      │              │                │  │  Calc     │  │                       │  │
│   │      │              │                │  ├───────────┤  │                       │  │
│   │      │              │                │  │  PTE      │  │                       │  │
│   │      │              │                │  │  Buffer   │  │                       │  │
│   │      │              │                │  └───────────┘  │                       │  │
│   │      │              │                └────────┬────────┘                       │  │
│   │      │              │                         │                               │  │
│   │      │              │                    Memory Bus                           │  │
│   │      │              │                    (多次读取)                            │  │
│   │      │              │                         │                               │  │
│   │      │              │                         ▼                               │  │
│   │      │              │                   ┌───────────┐                          │  │
│   │      │              │                   │   DRAM    │                          │  │
│   │      │              │                   │  (页表)   │                          │  │
│   │      │              │                   └───────────┘                          │  │
│   │      │              │                                                       │  │
│   │      └──────────────┴──────────────────────────────────────────────────────>  │  │
│   │                     Physical Address (转换完成后)                              │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 为什么需要硬件页表遍历器

```c
/*
 * 软件页表遍历 vs 硬件页表遍历对比
 */

/*
 * 方案1: 软件页表遍历 (早期系统如MIPS R3000)
 * 问题: TLB未命中时触发异常，由软件遍历页表
 */
void software_page_table_walk(uintptr_t vaddr, int access_type) {
    // 1. 保存寄存器状态
    save_context();

    // 2. 从专用寄存器读取页表基址
    uintptr_t pgd_base = read_c0_context();

    // 3. 软件遍历多级页表 (可能触发更多异常!)
    pgd_t *pgd = pgd_offset(pgd_base, vaddr);
    if (!pgd_present(*pgd)) {
        handle_page_fault(vaddr, access_type);
        return;
    }

    pud_t *pud = pud_offset(pgd, vaddr);
    if (!pud_present(*pud)) {
        handle_page_fault(vaddr, access_type);
        return;
    }

    pmd_t *pmd = pmd_offset(pud, vaddr);
    if (!pmd_present(*pmd)) {
        handle_page_fault(vaddr, access_type);
        return;
    }

    pte_t *pte = pte_offset(pmd, vaddr);
    if (!pte_present(*pte)) {
        handle_page_fault(vaddr, access_type);
        return;
    }

    // 4. 更新TLB (写TLB寄存器)
    write_c0_entryhi(vaddr & PAGE_MASK);
    write_c0_entrylo(pte_val(*pte));
    tlb_write_index();

    // 5. 恢复上下文并返回
    restore_context();
    eret();  // 异常返回
}

/*
 * 软件遍历的问题:
 * 1. 异常处理开销: 数十到数百个周期
 * 2. 页表访问可能触发更多TLB未命中 (嵌套异常)
 * 3. 需要保存/恢复完整上下文
 * 4. 代码复杂，容易出错
 */

/*
 * 方案2: 硬件页表遍历 (现代系统如x86-64, ARM64)
 * 优势: 硬件自动完成遍历，对软件透明
 */
// 无需软件介入！硬件自动完成:
// 1. 检测到TLB未命中
// 2. 硬件状态机自动遍历页表
// 3. 更新TLB
// 4. 重试原始内存访问
// 总延迟: 约10-100个周期 (取决于内存延迟)
```

## 2. 多级页表结构

### 2.1 x86-64四级页表详细结构

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                    x86-64 四级页表详细结构                                               │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   虚拟地址分解 (48位虚拟地址，4KB页):                                                    │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                                                                                 │  │
│   │   ┌────────┬─────────┬─────────┬─────────┬─────────┬────────────────────────┐  │  │
│   │   │ 未使用 │  PML4   │  PDPT   │   PD    │   PT    │       Offset           │  │  │
│   │   │ 16bits │ 9 bits  │ 9 bits  │ 9 bits  │ 9 bits  │       12 bits          │  │  │
│   │   ├────────┼─────────┼─────────┼─────────┼─────────┼────────────────────────┤  │  │
│   │   │63..48  │47..39   │38..30   │29..21   │20..12   │11..0                   │  │  │
│   │   └────────┴─────────┴─────────┴─────────┴─────────┴────────────────────────┘  │  │
│   │                                                                                 │  │
│   │   每级页表: 512项 × 8字节 = 4KB (正好一页)                                       │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   页表结构图示:                                                                          │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                                                                                 │  │
│   │   CR3寄存器:                                                                     │  │
│   │   ┌────────────────────────────────────────────────────────────────────────┐   │  │
│   │   │ Reserved │ PML4 Base Address (40 bits) │ PCD │ PWT │                  │   │  │
│   │   │  11:5    │          51:12              │  4  │  3  │                  │   │  │
│   │   └────────────────────────────────────────────────────────────────────────┘   │  │
│   │                              │                                                   │  │
│   │                              ▼                                                   │  │
│   │   ┌────────────────────────────────────────────────────────────────────────┐   │  │
│   │   │                       PML4 Table (Page Map Level 4)                    │   │  │
│   │   │  ┌───────────┬────────────────────────────────────────────────────┐   │   │  │
│   │   │  │ Entry 0   │ Points to PDPT Table #0                          │   │   │  │
│   │   │  │ PML4E[0]  │ Address[51:12] | Flags[11:0]                       │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │ Entry 1   │ Points to PDPT Table #1                          │   │   │  │
│   │   │  │ PML4E[1]  │ Address[51:12] | Flags[11:0]                       │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │    ...    │                                                    │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │ Entry 511 │ Points to PDPT Table #511                        │   │   │  │
│   │   │  │PML4E[511] │ Address[51:12] | Flags[11:0]                       │   │   │  │
│   │   │  └───────────┴────────────────────────────────────────────────────┘   │   │  │
│   │   │                    4KB (512 × 8 bytes)                                │   │  │
│   │   └────────────────────────────────────────────────────────────────────────┘   │  │
│   │                              │                                                   │  │
│   │                              ▼ (PML4 Index选择)                                  │  │
│   │   ┌────────────────────────────────────────────────────────────────────────┐   │  │
│   │   │                       PDPT Table (Page Directory Pointer Table)        │   │  │
│   │   │  ┌───────────┬────────────────────────────────────────────────────┐   │   │  │
│   │   │  │ Entry 0   │ Points to PD Table #0  OR 1GB Page Frame           │   │   │  │
│   │   │  │ PDPTE[0]  │ Address[51:12] | Flags[11:0] | PS (Page Size)      │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │ Entry 1   │ Points to PD Table #1  OR 1GB Page Frame           │   │   │  │
│   │   │  │ PDPTE[1]  │ Address[51:12] | Flags[11:0] | PS                  │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │    ...    │                                                    │   │   │  │
│   │   │  │           │ PS=1: 1GB大页，Address[51:30]为页框号               │   │   │  │
│   │   │  │           │ PS=0: 指向PD表                                     │   │   │  │
│   │   │  └───────────┴────────────────────────────────────────────────────┘   │   │  │
│   │   │                    4KB (512 × 8 bytes)                                │   │  │
│   │   └────────────────────────────────────────────────────────────────────────┘   │  │
│   │                              │                                                   │  │
│   │                              ▼ (PDPT Index选择)                                  │  │
│   │   ┌────────────────────────────────────────────────────────────────────────┐   │  │
│   │   │                       PD Table (Page Directory)                        │   │  │
│   │   │  ┌───────────┬────────────────────────────────────────────────────┐   │   │  │
│   │   │  │ Entry 0   │ Points to PT Table #0  OR 2MB Page Frame           │   │   │  │
│   │   │  │  PDE[0]   │ Address[51:12] | Flags[11:0] | PS                  │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │ Entry 1   │ Points to PT Table #1  OR 2MB Page Frame           │   │   │  │
│   │   │  │  PDE[1]   │ Address[51:12] | Flags[11:0] | PS                  │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │    ...    │                                                    │   │   │  │
│   │   │  │           │ PS=1: 2MB大页，Address[51:21]为页框号               │   │   │  │
│   │   │  │           │ PS=0: 指向PT表                                     │   │   │  │
│   │   │  └───────────┴────────────────────────────────────────────────────┘   │   │  │
│   │   │                    4KB (512 × 8 bytes)                                │   │  │
│   │   └────────────────────────────────────────────────────────────────────────┘   │  │
│   │                              │                                                   │  │
│   │                              ▼ (PD Index选择)                                    │  │
│   │   ┌────────────────────────────────────────────────────────────────────────┐   │  │
│   │   │                       PT Table (Page Table)                            │   │  │
│   │   │  ┌───────────┬────────────────────────────────────────────────────┐   │   │  │
│   │   │  │ Entry 0   │ Points to 4KB Page Frame                           │   │   │  │
│   │   │  │  PTE[0]   │ Address[51:12] | Flags[11:0]                       │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │ Entry 1   │ Points to 4KB Page Frame                           │   │   │  │
│   │   │  │  PTE[1]   │ Address[51:12] | Flags[11:0]                       │   │   │  │
│   │   │  ├───────────┼────────────────────────────────────────────────────┤   │   │  │
│   │   │  │    ...    │                                                    │   │   │  │
│   │   │  │           │ 每个PTE映射一个4KB物理页                           │   │   │  │
│   │   │  │           │ Address[51:12] | Offset[11:0] = Physical Address   │   │   │  │
│   │   │  └───────────┴────────────────────────────────────────────────────┘   │   │  │
│   │   │                    4KB (512 × 8 bytes)                                │   │  │
│   │   └────────────────────────────────────────────────────────────────────────┘   │  │
│   │                              │                                                   │  │
│   │                              ▼ (PT Index选择)                                    │  │
│   │   ┌────────────────────────────────────────────────────────────────────────┐   │  │
│   │   │                    Physical 4KB Page Frame                             │   │  │
│   │   │                    ┌────────────────────┐                              │   │  │
│   │   │   Offset[11:0] ───>│   Byte at offset   │                              │   │  │
│   │   │                    └────────────────────┘                              │   │  │
│   │   └────────────────────────────────────────────────────────────────────────┘   │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 页表项(PTE)详细格式

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                         x86-64 页表项(PTE)详细格式                                       │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                     页表项位定义 (64位)                                           │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   Bit(s)    Name        Description                                             │  │
│   │   ────────────────────────────────────────────────────────────────              │  │
│   │   0         P           Present (存在) - 页是否在内存中                          │  │
│   │   1         R/W         Read/Write (读写) - 0=只读, 1=可读写                     │  │
│   │   2         U/S         User/Supervisor (用户/超级用户)                          │  │
│   │                         0=仅Ring 0-2可访问, 1=所有特权级可访问                   │  │
│   │   3         PWT         Page Write Through (页写透)                             │  │
│   │   4         PCD         Page Cache Disable (页缓存禁用)                          │  │
│   │   5         A           Accessed (已访问) - CPU访问时自动置1                     │  │
│   │   6         D           Dirty (脏位) - 写入时自动置1                             │  │
│   │   7         PS          Page Size (页大小) - 大页标志                            │  │
│   │   8         G           Global (全局页) - TLB条目在CR3切换时不刷新               │  │
│   │   9-11      AVL         Available for system programmer's use                    │  │
│   │   12        PAT         Page Attribute Table index                              │  │
│   │   13-51     PFN         Page Frame Number (页框号)                               │  │
│   │   52-62     Reserved    保留 (可用于软件)                                        │  │
│   │   63        XD/EXB      Execute Disable (执行禁用) - 需要EFER.NXE=1             │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   页表项布局 (二进制):                                                                   │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                                                                                 │  │
│   │   63 │ 62    52 │ 51        12 │ 11 9 │ 8 │ 7 │ 6 │ 5 │ 4 │ 3 │ 2 │ 1 │ 0      │  │
│   │   ├────┴────────┼──────────────┼──────┼───┼───┼───┼───┼───┼───┼───┼───┼───────┤  │
│   │   │ XD │  Resv  │    PFN       │ AVL  │ G │PS │ D │ A │PCD│PWT│U/S│R/W│  P    │  │
│   │   └────┴────────┴──────────────┴──────┴───┴───┴───┴───┴───┴───┴───┴───┴───────┘  │
│   │                                                                                 │  │
│   │   XD = Execute Disable     PFN = Page Frame Number                              │  │
│   │   G  = Global              AVL = Available                                      │  │
│   │   PS = Page Size           PCD = Page Cache Disable                             │  │
│   │   D  = Dirty               PWT = Page Write Through                             │  │
│   │   A  = Accessed            U/S = User/Supervisor                                │  │
│   │   P  = Present             R/W = Read/Write                                     │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   常见页表项值示例:                                                                       │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                                                                                 │  │
│   │   Value        Binary                             Description                 │  │
│   │   ─────────────────────────────────────────────────────────────────             │  │
│   │   0x00000001   0000...0001                        存在，只读，内核            │  │
│   │   0x00000003   0000...0011                        存在，可读写，内核          │  │
│   │   0x00000005   0000...0101                        存在，只读，用户            │  │
│   │   0x00000007   0000...0111                        存在，可读写，用户          │  │
│   │   0x00000103   0001...00000011                    存在，可读写，内核，已访问  │  │
│   │   0x80000003   1000...00000011                    存在，可读写，内核，不可执行│  │
│   │   0x00000183   0001...10000011                    2MB大页，可读写，内核，已访问│  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

## 3. 页表遍历算法

### 3.1 地址转换流程图

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                    页表遍历状态机流程图                                                  │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                                                                                 │  │
│   │                              ┌─────────────┐                                    │  │
│   │                    ┌────────>│    IDLE     │<────────┐                         │  │
│   │                    │         │   (空闲)    │         │                         │  │
│   │                    │         └──────┬──────┘         │                         │  │
│   │                    │                │                │                         │  │
│   │                    │         TLB Miss│                │                         │  │
│   │                    │                │                │                         │  │
│   │                    │                ▼                │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │    READ_PML4    │        │                         │  │
│   │                    │      │ 读取PML4表项    │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │                    │      │ addr = CR3.base │        │                         │  │
│   │                    │      │       + vaddr[47:39]×8│        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │               │                         │  │
│   │                    │                 │ Mem Read      │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │  CHECK_PML4     │        │                         │  │
│   │                    │      │ 检查PML4表项    │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │         P=0        │      │ if PML4E.P == 0 │────────┼───> Page Fault          │  │
│   │         ───────────┤      │    goto FAULT   │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │               │                         │  │
│   │                    │                 │ P=1           │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │    READ_PDPT    │        │                         │  │
│   │                    │      │ 读取PDPT表项    │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │                    │      │ addr = PML4E.pfn│        │                         │  │
│   │                    │      │       + vaddr[38:30]×8│        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │               │                         │  │
│   │                    │                 │ Mem Read      │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │  CHECK_PDPT     │        │                         │  │
│   │                    │      │ 检查PDPT表项    │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │         P=0        │      │ if PDPTE.P == 0 │────────┼───> Page Fault          │  │
│   │         ───────────┤      │    goto FAULT   │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │         PS=1       │      │ if PDPTE.PS == 1│────────┼───> 1GB Page (DONE)    │  │
│   │         (1GB页)    │      │    1GB page     │        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │ PS=0          │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │     READ_PD     │        │                         │  │
│   │                    │      │ 读取PD表项      │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │                    │      │ addr = PDPTE.pfn│        │                         │  │
│   │                    │      │       + vaddr[29:21]×8│        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │               │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │    CHECK_PD     │        │                         │  │
│   │                    │      │ 检查PD表项      │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │         P=0        │      │ if PDE.P == 0   │────────┼───> Page Fault          │  │
│   │         ───────────┤      │    goto FAULT   │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │         PS=1       │      │ if PDE.PS == 1  │────────┼───> 2MB Page (DONE)    │  │
│   │         (2MB页)    │      │    2MB page     │        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │ PS=0          │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │     READ_PT     │        │                         │  │
│   │                    │      │ 读取PT表项      │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │                    │      │ addr = PDE.pfn  │        │                         │  │
│   │                    │      │       + vaddr[20:12]×8│        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │               │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │    CHECK_PT     │        │                         │  │
│   │                    │      │ 检查PTE表项     │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │         P=0        │      │ if PTE.P == 0   │────────┼───> Page Fault          │  │
│   │         ───────────┤      │    goto FAULT   │        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │ P=1           │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │    BUILD_PA     │        │                         │  │
│   │                    │      │ 构建物理地址    │        │                         │  │
│   │                    │      │                 │        │                         │  │
│   │                    │      │ PA = PTE.pfn    │        │                         │  │
│   │                    │      │      : vaddr[11:0]│        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │               │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │  UPDATE_TLB     │────────┼───> Update TLB          │  │
│   │                    │      │ 更新TLB缓存     │        │                         │  │
│   │                    │      └────────┬────────┘        │                         │  │
│   │                    │                 │               │                         │  │
│   │                    │                 ▼               │                         │  │
│   │                    │      ┌─────────────────┐        │                         │  │
│   │                    │      │     DONE        │────────┼───> Return PA to CPU    │  │
│   │                    │      │ 完成，返回结果  │        │                         │  │
│   │                    │      └─────────────────┘        │                         │  │
│   │                    │                                 │                         │  │
│   │                    └─────────────────────────────────┘                         │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 C语言模拟页表遍历

```c
/*
 * 页表遍历器的C语言模拟实现
 * 用于理解页表遍历的算法逻辑
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
 * 页表项定义 (x86-64风格)
 */
typedef union {
    uint64_t value;
    struct {
        uint64_t present    : 1;   // 0
        uint64_t rw         : 1;   // 1
        uint64_t us         : 1;   // 2
        uint64_t pwt        : 1;   // 3
        uint64_t pcd        : 1;   // 4
        uint64_t accessed   : 1;   // 5
        uint64_t dirty      : 1;   // 6
        uint64_t ps         : 1;   // 7
        uint64_t global     : 1;   // 8
        uint64_t avl        : 3;   // 9-11
        uint64_t pfn        : 40;  // 12-51 (页框号)
        uint64_t reserved   : 11;  // 52-62
        uint64_t nx         : 1;   // 63
    } bits;
} pte_t;

/*
 * 模拟物理内存 (简化模型)
 */
#define PHYS_MEM_SIZE (256 * 1024 * 1024)  // 256MB物理内存
#define PAGE_SIZE 4096
#define PFN_SHIFT 12

static uint8_t physical_memory[PHYS_MEM_SIZE];

/*
 * 页表基址 (模拟CR3)
 */
static uint64_t cr3_base = 0x10000;  // PML4表在物理地址0x10000

/*
 * 读取物理内存 (模拟内存总线)
 */
static uint64_t read_physical_qword(uint64_t phys_addr) {
    if (phys_addr + 8 > PHYS_MEM_SIZE) {
        printf("ERROR: Physical address out of range: 0x%lx\n", phys_addr);
        return 0;
    }
    uint64_t value;
    memcpy(&value, &physical_memory[phys_addr], sizeof(value));
    return value;
}

/*
 * 写入物理内存
 */
static void write_physical_qword(uint64_t phys_addr, uint64_t value) {
    if (phys_addr + 8 > PHYS_MEM_SIZE) {
        printf("ERROR: Physical address out of range: 0x%lx\n", phys_addr);
        return;
    }
    memcpy(&physical_memory[phys_addr], &value, sizeof(value));
}

/*
 * 初始化页表 (设置一个恒等映射)
 */
void init_page_tables(void) {
    printf("=== 初始化页表 ===\n");

    // PML4表地址
    uint64_t pml4_addr = cr3_base;
    // PDPT表地址 (紧跟PML4)
    uint64_t pdpt_addr = pml4_addr + PAGE_SIZE;
    // PD表地址
    uint64_t pd_addr = pdpt_addr + PAGE_SIZE;
    // PT表地址
    uint64_t pt_addr = pd_addr + PAGE_SIZE;

    printf("PML4 表地址: 0x%lx\n", pml4_addr);
    printf("PDPT 表地址: 0x%lx\n", pdpt_addr);
    printf("PD   表地址: 0x%lx\n", pd_addr);
    printf("PT   表地址: 0x%lx\n", pt_addr);

    // 设置PML4[0] -> PDPT
    pte_t pml4e = { .value = 0 };
    pml4e.bits.present = 1;
    pml4e.bits.rw = 1;
    pml4e.bits.us = 1;
    pml4e.bits.pfn = pdpt_addr >> PFN_SHIFT;
    write_physical_qword(pml4_addr, pml4e.value);
    printf("PML4[0] = 0x%016lx (指向PDPT)\n", pml4e.value);

    // 设置PDPT[0] -> PD
    pte_t pdpte = { .value = 0 };
    pdpte.bits.present = 1;
    pdpte.bits.rw = 1;
    pdpte.bits.us = 1;
    pdpte.bits.pfn = pd_addr >> PFN_SHIFT;
    write_physical_qword(pdpt_addr, pdpte.value);
    printf("PDPT[0] = 0x%016lx (指向PD)\n", pdpte.value);

    // 设置PD[0] -> PT
    pte_t pde = { .value = 0 };
    pde.bits.present = 1;
    pde.bits.rw = 1;
    pde.bits.us = 1;
    pde.bits.pfn = pt_addr >> PFN_SHIFT;
    write_physical_qword(pd_addr, pde.value);
    printf("PD[0]   = 0x%016lx (指向PT)\n", pde.value);

    // 设置PT[0..511] -> 物理页 (恒等映射前2MB)
    for (int i = 0; i < 512; i++) {
        pte_t pte = { .value = 0 };
        pte.bits.present = 1;
        pte.bits.rw = 1;
        pte.bits.us = 1;
        pte.bits.pfn = i;  // 恒等映射
        write_physical_qword(pt_addr + i * 8, pte.value);
    }
    printf("PT[0..511] 设置为恒等映射\n");
}

/*
 * 页表遍历结果
 */
typedef struct {
    bool success;
    uint64_t physical_addr;
    pte_t final_pte;
    const char* page_size;
    int access_count;  // 内存访问次数
    const char* error_msg;
} walk_result_t;

/*
 * 页表遍历器 (核心算法)
 */
walk_result_t walk_page_table(uint64_t vaddr) {
    walk_result_t result = {0};
    result.access_count = 0;

    printf("\n=== 页表遍历: VA = 0x%016lx ===\n", vaddr);

    // 分解虚拟地址
    uint64_t pml4_index = (vaddr >> 39) & 0x1FF;
    uint64_t pdpt_index = (vaddr >> 30) & 0x1FF;
    uint64_t pd_index   = (vaddr >> 21) & 0x1FF;
    uint64_t pt_index   = (vaddr >> 12) & 0x1FF;
    uint64_t offset     = vaddr & 0xFFF;

    printf("虚拟地址分解:\n");
    printf("  PML4 index: %3lu (0x%03lx)\n", pml4_index, pml4_index);
    printf("  PDPT index: %3lu (0x%03lx)\n", pdpt_index, pdpt_index);
    printf("  PD   index: %3lu (0x%03lx)\n", pd_index, pd_index);
    printf("  PT   index: %3lu (0x%03lx)\n", pt_index, pt_index);
    printf("  Offset:     0x%03lx\n", offset);

    // 步骤1: 读取PML4项
    uint64_t pml4_entry_addr = cr3_base + (pml4_index * 8);
    printf("\n步骤1: 读取PML4项\n");
    printf("  PML4表基址: 0x%lx (来自CR3)\n", cr3_base);
    printf("  PML4项地址: 0x%lx + %lu×8 = 0x%lx\n",
           cr3_base, pml4_index, pml4_entry_addr);

    pte_t pml4e = { .value = read_physical_qword(pml4_entry_addr) };
    result.access_count++;
    printf("  PML4E值: 0x%016lx\n", pml4e.value);
    printf("    Present: %lu, R/W: %lu, U/S: %lu\n",
           pml4e.bits.present, pml4e.bits.rw, pml4e.bits.us);

    if (!pml4e.bits.present) {
        result.error_msg = "PML4 entry not present";
        goto fault;
    }

    // 步骤2: 读取PDPT项
    uint64_t pdpt_base = pml4e.bits.pfn << PFN_SHIFT;
    uint64_t pdpt_entry_addr = pdpt_base + (pdpt_index * 8);
    printf("\n步骤2: 读取PDPT项\n");
    printf("  PDPT表基址: 0x%lx (来自PML4E)\n", pdpt_base);
    printf("  PDPT项地址: 0x%lx + %lu×8 = 0x%lx\n",
           pdpt_base, pdpt_index, pdpt_entry_addr);

    pte_t pdpte = { .value = read_physical_qword(pdpt_entry_addr) };
    result.access_count++;
    printf("  PDPTE值: 0x%016lx\n", pdpte.value);
    printf("    Present: %lu, PS: %lu\n", pdpte.bits.present, pdpte.bits.ps);

    if (!pdpte.bits.present) {
        result.error_msg = "PDPT entry not present";
        goto fault;
    }

    // 检查1GB大页
    if (pdpte.bits.ps) {
        printf("\n  >>> 1GB大页! <<<?\n");
        result.page_size = "1GB";
        result.physical_addr = (pdpte.bits.pfn << PFN_SHIFT) | (vaddr & 0x3FFFFFFF);
        result.final_pte = pdpte;
        goto success;
    }

    // 步骤3: 读取PD项
    uint64_t pd_base = pdpte.bits.pfn << PFN_SHIFT;
    uint64_t pd_entry_addr = pd_base + (pd_index * 8);
    printf("\n步骤3: 读取PD项\n");
    printf("  PD表基址: 0x%lx (来自PDPTE)\n", pd_base);
    printf("  PD项地址: 0x%lx + %lu×8 = 0x%lx\n",
           pd_base, pd_index, pd_entry_addr);

    pte_t pde = { .value = read_physical_qword(pd_entry_addr) };
    result.access_count++;
    printf("  PDE值: 0x%016lx\n", pde.value);
    printf("    Present: %lu, PS: %lu\n", pde.bits.present, pde.bits.ps);

    if (!pde.bits.present) {
        result.error_msg = "PD entry not present";
        goto fault;
    }

    // 检查2MB大页
    if (pde.bits.ps) {
        printf("\n  >>> 2MB大页! <<<\n");
        result.page_size = "2MB";
        result.physical_addr = (pde.bits.pfn << PFN_SHIFT) | (vaddr & 0x1FFFFF);
        result.final_pte = pde;
        goto success;
    }

    // 步骤4: 读取PT项
    uint64_t pt_base = pde.bits.pfn << PFN_SHIFT;
    uint64_t pt_entry_addr = pt_base + (pt_index * 8);
    printf("\n步骤4: 读取PT项\n");
    printf("  PT表基址: 0x%lx (来自PDE)\n", pt_base);
    printf("  PT项地址: 0x%lx + %lu×8 = 0x%lx\n",
           pt_base, pt_index, pt_entry_addr);

    pte_t pte = { .value = read_physical_qword(pt_entry_addr) };
    result.access_count++;
    printf("  PTE值: 0x%016lx\n", pte.value);
    printf("    Present: %lu, R/W: %lu, U/S: %lu\n",
           pte.bits.present, pte.bits.rw, pte.bits.us);

    if (!pte.bits.present) {
        result.error_msg = "PTE not present";
        goto fault;
    }

    printf("\n  >>> 4KB页 <<<\n");
    result.page_size = "4KB";
    result.physical_addr = (pte.bits.pfn << PFN_SHIFT) | offset;
    result.final_pte = pte;

success:
    result.success = true;
    printf("\n=== 遍历成功 ===\n");
    printf("页大小: %s\n", result.page_size);
    printf("物理地址: 0x%016lx\n", result.physical_addr);
    printf("内存访问次数: %d\n", result.access_count);
    return result;

fault:
    result.success = false;
    printf("\n=== 页错误 ===\n");
    printf("错误: %s\n", result.error_msg);
    printf("内存访问次数: %d\n", result.access_count);
    return result;
}

int main() {
    // 初始化页表
    init_page_tables();

    // 测试地址转换
    // 虚拟地址0x1000应该映射到物理地址0x1000 (恒等映射)
    walk_result_t r1 = walk_page_table(0x1000);

    // 虚拟地址0x2000应该映射到物理地址0x2000
    walk_result_t r2 = walk_page_table(0x2000);

    // 虚拟地址0x12345678的映射
    walk_result_t r3 = walk_page_table(0x12345678);

    return 0;
}
```

## 4. 硬件遍历 vs 软件遍历

### 4.1 对比分析

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                    硬件遍历 vs 软件遍历对比                                              │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                              硬件页表遍历                                        │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   架构: x86-64, ARM64 (ARMv7+), RISC-V (S模式)                                   │  │
│   │                                                                                 │  │
│   │   优点:                                                                          │  │
│   │   ✓ 透明 - 对操作系统完全透明，无需异常处理                                      │  │
│   │   ✓ 快速 - 10-100周期完成遍历 (取决于缓存命中)                                  │  │
│   │   ✓ 安全 - 硬件直接访问页表，不会被软件干扰                                      │  │
│   │   ✓ 嵌套安全 - 页表访问通过专用内存端口，不会触发额外TLB未命中                   │  │
│   │   ✓ 低功耗 - 无需上下文切换和指令缓存访问                                       │  │
│   │                                                                                 │  │
│   │   缺点:                                                                          │  │
│   │   ✗ 硬件复杂 - 需要专用状态机和内存端口                                         │  │
│   │   ✗ 灵活性差 - 页表格式由硬件固定                                               │  │
│   │   ✗ 调试困难 - 硬件问题难以诊断                                                 │  │
│   │                                                                                 │  │
│   │   实现复杂度: ████████░░ (高)                                                   │  │
│   │   遍历延迟:    ████░░░░░░ (10-100周期)                                          │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                              软件页表遍历                                        │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   架构: MIPS (R3000, R4000), 早期SPARC, 一些嵌入式处理器                         │  │
│   │                                                                                 │  │
│   │   优点:                                                                          │  │
│   │   ✓ 硬件简单 - 只需TLB和异常触发机制                                            │  │
│   │   ✓ 灵活性高 - 软件可实现任意页表格式                                           │  │
│   │   ✓ 可扩展 - 可实现复杂功能如压缩页表、加密页表                                 │  │
│   │   ✓ 可调试 - 软件遍历代码可调试、可分析                                         │  │
│   │   ✓ 可优化 - 可实现软件TLB预取、批量更新等优化                                  │  │
│   │                                                                                 │  │
│   │   缺点:                                                                          │  │
│   │   ✗ 异常开销 - 需要保存/恢复上下文 (数十到数百周期)                             │  │
│   │   ✗ 嵌套问题 - 页表访问可能触发额外TLB未命中 (TLB嵌套异常)                      │  │
│   │   ✗ 安全风险 - 软件代码可能有漏洞                                               │  │
│   │   ✗ 实时性差 - 异常处理时间不确定                                               │  │
│   │                                                                                 │  │
│   │   实现复杂度: ███░░░░░░░ (低)                                                   │  │
│   │   遍历延迟:    █████████░ (数百到数千周期)                                      │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                           混合方案 (现代趋势)                                    │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   一些现代处理器结合两种方案:                                                     │  │
│   │   • 硬件遍历主路径 - 快速处理常见情况                                            │  │
│   │   • 软件回退 - 处理特殊情况 (如页错误、自定义格式)                               │  │
│   │   • 硬件预取 - 硬件自动预取页表项减少延迟                                        │  │
│   │   • 可编程遍历 - 硬件支持多种页表格式配置                                        │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 性能对比数据

```c
/*
 * 硬件遍历 vs 软件遍历性能对比
 * 假设：4级页表，每个页表项在单独缓存行
 */

/*
 * 硬件页表遍历延迟分析
 */
void hardware_walk_timing(void) {
    /*
     * 场景1: 所有页表项在L1缓存中
     * - 每个内存访问: 4-5周期
     * - 4级页表需要4次读取
     * - 额外开销: 状态机、地址计算
     * 总延迟: 约20-30周期
     */
    int cycles_best_case = 25;

    /*
     * 场景2: 页表项在L2缓存中
     * - 每个内存访问: 12-15周期
     * - 4次读取
     * 总延迟: 约50-70周期
     */
    int cycles_l2_case = 60;

    /*
     * 场景3: 页表项在L3缓存中
     * - 每个内存访问: 30-40周期
     * - 4次读取
     * 总延迟: 约120-160周期
     */
    int cycles_l3_case = 140;

    /*
     * 场景4: 页表项需要从内存读取
     * - 每个内存访问: 100-300周期
     * - 4次读取 (可能部分并行)
     * 总延迟: 约200-500周期
     */
    int cycles_mem_case = 350;

    printf("硬件遍历延迟:\n");
    printf("  L1缓存命中: %d 周期\n", cycles_best_case);
    printf("  L2缓存命中: %d 周期\n", cycles_l2_case);
    printf("  L3缓存命中: %d 周期\n", cycles_l3_case);
    printf("  内存访问:   %d 周期\n", cycles_mem_case);
}

/*
 * 软件页表遍历延迟分析
 */
void software_walk_timing(void) {
    /*
     * 场景: TLB未命中触发TLB缺失异常
     *
     * 1. 异常入口开销:
     *    - 保存上下文 (16-32个寄存器) : 16-32周期
     *    - 切换堆栈、特权级            : 5-10周期
     *    - 跳转到异常处理程序          : 5-10周期
     *    小计: 约30-50周期
     *
     * 2. 软件遍历开销:
     *    - 读取页表基址                : 1-5周期
     *    - 遍历4级页表 (每次读取)      : 4 × (5-100) = 20-400周期
     *    - 可能触发额外的TLB未命中!
     *    小计: 约30-500周期
     *
     * 3. 异常退出开销:
     *    - 恢复上下文                  : 16-32周期
     *    - 返回原程序                  : 5-10周期
     *    小计: 约25-45周期
     *
     * 总延迟: 约85-600周期 (不包含可能的嵌套TLB未命中)
     */
    int cycles_min = 85;
    int cycles_typical = 300;
    int cycles_max = 1000;  // 包含嵌套TLB未命中

    printf("\n软件遍历延迟:\n");
    printf("  最佳情况:   %d 周期\n", cycles_min);
    printf("  典型情况:   %d 周期\n", cycles_typical);
    printf("  最坏情况:   %d+ 周期\n", cycles_max);
}

/*
 * 加速比计算
 */
void calculate_speedup(void) {
    /*
     * 硬件遍历平均: 60周期
     * 软件遍历平均: 300周期
     * 加速比: 5倍
     */
    double speedup = 300.0 / 60.0;
    printf("\n加速比: %.1fx\n", speedup);

    /*
     * 在现代多核系统中，TLB未命中更频繁
     * 硬件遍历的优势更加显著
     */
}
```

## 5. 页表缓存 (PTE缓存)

### 5.1 PTE缓存原理

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                         页表缓存 (Page Table Entry Cache)                                │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   问题: 多级页表遍历需要多次内存访问，每次访问都有延迟                             │  │
│   │   解决方案: 缓存中间级页表项，减少内存访问次数                                     │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                         带PTE缓存的MMU架构                                       │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   ┌─────────────────────────────────────────────────────────────────────────┐  │  │
│   │   │                          Virtual Address                                 │  │  │
│   │   │  ┌────────┬─────────┬─────────┬─────────┬────────────────────────────┐  │  │  │
│   │   │  │ Unused │  PML4   │  PDPT   │   PD    │           Offset           │  │  │  │
│   │   │  │        │  Index  │  Index  │  Index  │                            │  │  │  │
│   │   │  └────────┴────┬────┴────┬────┴────┬────┴────────────────────────────┘  │  │  │
│   │   │                │         │         │                                      │  │  │
│   │   │                ▼         ▼         ▼                                      │  │  │
│   │   │         ┌──────────────────────────────────────┐                         │  │  │
│   │   │         │           TLB Lookup                 │                         │  │  │
│   │   │         └────────────────┬─────────────────────┘                         │  │  │
│   │   │                          │                                                 │  │  │
│   │   │                ┌─────────┴─────────┐                                      │  │  │
│   │   │                ▼                   ▼                                      │  │  │
│   │   │           [Hit]               [Miss]                                      │  │  │
│   │   │              │                   │                                        │  │  │
│   │   │              ▼                   ▼                                        │  │  │
│   │   │         Return PA         ┌──────────────────┐                           │  │  │
│   │   │                         │  Page Table      │                           │  │  │
│   │   │                         │  Walker with     │                           │  │  │
│   │   │                         │  PTE Caches      │                           │  │  │
│   │   │                         │                  │                           │  │  │
│   │   │                         │  ┌────────────┐  │                           │  │  │
│   │   │                         │  │ PML4 Cache │  │  缓存PML4条目              │  │  │
│   │   │                         │  │ (4-8项)    │  │  命中时避免读取PML4表      │  │  │
│   │   │                         │  └─────┬──────┘  │                           │  │  │
│   │   │                         │        │         │                           │  │  │
│   │   │                         │  ┌─────┴──────┐  │                           │  │  │
│   │   │                         │  │ PDPT Cache │  │  缓存PDPT条目              │  │  │
│   │   │                         │  │ (8-16项)   │  │  命中时避免读取PDPT表      │  │  │
│   │   │                         │  └─────┬──────┘  │                           │  │  │
│   │   │                         │        │         │                           │  │  │
│   │   │                         │  ┌─────┴──────┐  │                           │  │  │
│   │   │                         │  │  PD Cache  │  │  缓存PD条目                │  │  │
│   │   │                         │  │ (16-32项)  │  │  命中时避免读取PD表        │  │  │
│   │   │                         │  └────────────┘  │                           │  │  │
│   │   │                         └──────────────────┘                           │  │  │
│   │   │                                    │                                    │  │  │
│   │   │                                    ▼                                    │  │  │
│   │   │                              Memory Bus                                 │  │  │
│   │   │                           (减少的访问次数)                               │  │  │
│   │   │                                    │                                    │  │  │
│   │   │                                    ▼                                    │  │  │
│   │   │                              Physical Memory                            │  │  │
│   │   │                              (Page Tables)                              │  │  │
│   │   └─────────────────────────────────────────────────────────────────────────┘  │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   PTE缓存命中场景:                                                               │  │
│   │   ────────────────                                                                │  │
│   │                                                                                 │  │
│   │   场景1: 完全缓存命中 (最佳情况)                                                │  │
│   │   - PML4缓存命中 → PDPT缓存命中 → PD缓存命中                                    │  │
│   │   - 只需要1次内存访问读取PTE                                                    │  │
│   │   - 延迟: ~10周期                                                               │  │
│   │                                                                                 │  │
│   │   场景2: 部分缓存命中 (常见情况)                                                │  │
│   │   - PML4缓存命中 → PDPT缓存命中 → PD缓存未命中                                  │  │
│   │   - 需要2次内存访问: PD表 + PTE                                                 │  │
│   │   - 延迟: ~30周期                                                               │  │
│   │                                                                                 │  │
│   │   场景3: 无缓存命中 (最坏情况)                                                  │  │
│   │   - 需要4次内存访问遍历所有页表                                                 │  │
│   │   - 延迟: ~60-100周期                                                           │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 5.2 PTE缓存设计

```c
/*
 * 页表缓存的C语言模拟实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
 * PTE缓存条目
 */
#define PML4_CACHE_SIZE 8
#define PDPT_CACHE_SIZE 16
#define PD_CACHE_SIZE 32

typedef struct {
    uint16_t tag;       // 部分VPN作为标签
    uint64_t pte;       // 页表项
    bool valid;
    uint8_t lru_counter;
} pte_cache_entry_t;

/*
 * PTE缓存结构
 */
typedef struct {
    pte_cache_entry_t entries[PML4_CACHE_SIZE];
    const char* name;
} pte_cache_t;

/*
 * 三级PTE缓存
 */
typedef struct {
    pte_cache_t pml4_cache;
    pte_cache_t pdpt_cache;
    pte_cache_t pd_cache;

    // 统计信息
    uint64_t hits;
    uint64_t misses;
} pte_cache_system_t;

/*
 * 初始化PTE缓存
 */
void init_pte_cache(pte_cache_system_t* cache) {
    memset(cache, 0, sizeof(*cache));
    cache->pml4_cache.name = "PML4";
    cache->pdpt_cache.name = "PDPT";
    cache->pd_cache.name = "PD";
}

/*
 * PTE缓存查找
 */
bool lookup_pte_cache(pte_cache_t* cache, uint16_t tag, uint64_t* pte) {
    for (int i = 0; i < PML4_CACHE_SIZE; i++) {
        if (cache->entries[i].valid && cache->entries[i].tag == tag) {
            *pte = cache->entries[i].pte;
            // 更新LRU
            cache->entries[i].lru_counter = 0;
            for (int j = 0; j < PML4_CACHE_SIZE; j++) {
                if (j != i && cache->entries[j].valid) {
                    cache->entries[j].lru_counter++;
                }
            }
            return true;
        }
    }
    return false;
}

/*
 * PTE缓存更新
 */
void update_pte_cache(pte_cache_t* cache, uint16_t tag, uint64_t pte) {
    // 查找空闲条目或LRU最老的条目
    int victim_idx = -1;
    uint8_t max_lru = 0;

    for (int i = 0; i < PML4_CACHE_SIZE; i++) {
        if (!cache->entries[i].valid) {
            victim_idx = i;
            break;
        }
        if (cache->entries[i].lru_counter > max_lru) {
            max_lru = cache->entries[i].lru_counter;
            victim_idx = i;
        }
    }

    if (victim_idx >= 0) {
        cache->entries[victim_idx].valid = true;
        cache->entries[victim_idx].tag = tag;
        cache->entries[victim_idx].pte = pte;
        cache->entries[victim_idx].lru_counter = 0;
    }
}

/*
 * 带PTE缓存的页表遍历
 */
typedef struct {
    bool success;
    uint64_t physical_addr;
    int memory_accesses;  // 实际内存访问次数
    int cache_hits;       // 缓存命中次数
} cached_walk_result_t;

cached_walk_result_t walk_with_pte_cache(pte_cache_system_t* cache,
                                          uint64_t vaddr,
                                          uint64_t cr3_base) {
    cached_walk_result_t result = {0};

    uint64_t pml4_index = (vaddr >> 39) & 0x1FF;
    uint64_t pdpt_index = (vaddr >> 30) & 0x1FF;
    uint64_t pd_index   = (vaddr >> 21) & 0x1FF;
    uint64_t pt_index   = (vaddr >> 12) & 0x1FF;
    uint64_t offset     = vaddr & 0xFFF;

    uint64_t pte;
    bool hit;

    // 步骤1: 查找PML4缓存
    hit = lookup_pte_cache(&cache->pml4_cache, pml4_index, &pte);
    if (!hit) {
        // 从内存读取PML4项
        result.memory_accesses++;
        pte = read_physical_qword(cr3_base + pml4_index * 8);
        update_pte_cache(&cache->pml4_cache, pml4_index, pte);
    } else {
        result.cache_hits++;
    }

    if (!(pte & 1)) {  // P=0
        result.success = false;
        return result;
    }

    uint64_t pdpt_base = (pte >> 12) << 12;

    // 步骤2: 查找PDPT缓存
    uint16_t pdpt_tag = (pml4_index << 9) | pdpt_index;
    hit = lookup_pte_cache(&cache->pdpt_cache, pdpt_tag, &pte);
    if (!hit) {
        result.memory_accesses++;
        pte = read_physical_qword(pdpt_base + pdpt_index * 8);
        update_pte_cache(&cache->pdpt_cache, pdpt_tag, pte);
    } else {
        result.cache_hits++;
    }

    if (!(pte & 1)) {
        result.success = false;
        return result;
    }

    if (pte & (1ULL << 7)) {  // PS=1, 1GB页
        result.success = true;
        result.physical_addr = ((pte >> 12) << 12) | (vaddr & 0x3FFFFFFF);
        return result;
    }

    uint64_t pd_base = (pte >> 12) << 12;

    // 步骤3: 查找PD缓存
    uint16_t pd_tag = (pdpt_tag << 9) | pd_index;
    hit = lookup_pte_cache(&cache->pd_cache, pd_tag, &pte);
    if (!hit) {
        result.memory_accesses++;
        pte = read_physical_qword(pd_base + pd_index * 8);
        update_pte_cache(&cache->pd_cache, pd_tag, pte);
    } else {
        result.cache_hits++;
    }

    if (!(pte & 1)) {
        result.success = false;
        return result;
    }

    if (pte & (1ULL << 7)) {  // PS=1, 2MB页
        result.success = true;
        result.physical_addr = ((pte >> 12) << 12) | (vaddr & 0x1FFFFF);
        return result;
    }

    uint64_t pt_base = (pte >> 12) << 12;

    // 步骤4: 读取PTE (最后一级不缓存，直接读取)
    result.memory_accesses++;
    pte = read_physical_qword(pt_base + pt_index * 8);

    if (!(pte & 1)) {
        result.success = false;
        return result;
    }

    result.success = true;
    result.physical_addr = ((pte >> 12) << 12) | offset;
    return result;
}
```

## 6. 页错误处理硬件

### 6.1 页错误类型与检测

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                         页错误类型与硬件检测                                             │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                     页错误类型分类                                                │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   1. 不存在页错误 (Page Not Present)                                            │  │
│   │      ─────────────────────────────────                                          │  │
│   │      原因: 访问的页表项P位=0                                                    │  │
│   │      处理: 分配物理页，填充页表项，重试访问                                     │  │
│   │      触发: 首次访问、页被换出                                                     │  │
│   │                                                                                 │  │
│   │   2. 保护错误 (Protection Violation)                                            │  │
│   │      ───────────────────────────────                                            │  │
│   │      原因: 违反页权限 (写只读页、执行不可执行页、用户访问内核页)               │  │
│   │      处理: 通常发送SIGSEGV给进程                                                │  │
│   │      触发: 程序错误、安全攻击                                                     │  │
│   │                                                                                 │  │
│   │   3. 保留位错误 (Reserved Bit Violation)                                        │  │
│   │      ───────────────────────────────────                                        │  │
│   │      原因: 页表项保留位被置位                                                     │  │
│   │      处理: 发送SIGSEGV                                                          │  │
│   │                                                                                 │  │
│   │   4. 指令获取错误 (Instruction Fetch)                                           │  │
│   │      ────────────────────────────────                                           │  │
│   │      原因: 尝试从不可执行页获取指令                                             │  │
│   │      处理: 发送SIGSEGV                                                          │  │
│   │                                                                                 │  │
│   │   5. 对齐错误 (Alignment Check)                                                 │  │
│   │      ──────────────────────────                                                 │  │
│   │      原因: 未对齐访问 (当AC标志启用时)                                          │  │
│   │      处理: 发送SIGBUS                                                           │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │                     x86-64 页错误错误码格式                                       │  │
│   ├─────────────────────────────────────────────────────────────────────────────────┤  │
│   │                                                                                 │  │
│   │   当页错误发生时，CPU将错误码压入异常处理程序的栈中:                              │  │
│   │                                                                                 │  │
│   │   ┌────┬────┬────┬────┬────┬────────────┬────────────┬────────────┐            │  │
│   │   │ P  │ W/R│ U/S│RSVD│ I/D│   PK       │   SS       │  Reserved  │            │  │
│   │   │ 0  │ 1  │ 2  │ 3  │ 4  │  5:14      │  15        │  16:31     │            │  │
│   │   ├────┼────┼────┼────┼────┼────────────┼────────────┼────────────┤            │  │
│   │   │ P=0: 不存在页错误                                          │            │  │
│   │   │ P=1: 保护错误                                              │            │  │
│   │   │ W/R=0: 读访问                                              │            │  │
│   │   │ W/R=1: 写访问                                              │            │  │
│   │   │ U/S=0: 超级用户模式 (Ring 0)                               │            │  │
│   │   │ U/S=1: 用户模式 (Ring 3)                                   │            │  │
│   │   │ RSVD=1: 保留位违规                                         │            │  │
│   │   │ I/D=1: 指令获取错误                                        │            │  │
│   │   │ PK: 保护键 (如果启用)                                      │            │  │
│   │   │ SS: Shadow Stack访问 (如果启用)                            │            │  │
│   │   └──────────────────────────────────────────────────────────┘            │  │
│   │                                                                                 │  │
│   │   错误码示例:                                                                   │  │
│   │   - 0x00000000: 读访问不存在的内核页                                            │  │
│   │   - 0x00000001: 写访问不存在的页 或 读保护错误                                  │  │
│   │   - 0x00000002: 写访问不存在的页 (用户模式)                                     │  │
│   │   - 0x00000003: 写保护错误                                                      │  │
│   │   - 0x00000010: 指令获取错误                                                    │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### 6.2 硬件页错误处理流程

```c
/*
 * Linux内核页错误处理流程
 * 对应硬件检测到的页错误
 */

#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/traps.h>

/*
 * 页错误入口 (由硬件调用)
 * 当MMU检测到页错误时，自动跳转到这里
 */
void do_page_fault(struct pt_regs *regs, unsigned long error_code) {
    unsigned long address = read_cr2();  // 读取CR2获取错误虚拟地址
    struct task_struct *tsk = current;
    struct mm_struct *mm = tsk->mm;
    int fault;

    /*
     * 错误码解析
     */
    int present = error_code & PF_PRESENT;   // 页是否存在
    int write = error_code & PF_WRITE;       // 写访问
    int user = error_code & PF_USER;         // 用户模式
    int reserved = error_code & PF_RSVD;     // 保留位违规
    int instruction = error_code & PF_INSTR; // 指令获取

    printk(KERN_DEBUG "Page fault at 0x%lx, error_code: 0x%lx\n",
           address, error_code);

    /*
     * 情况1: 保留位违规 - 立即终止
     */
    if (reserved) {
        printk(KERN_ALERT "Reserved bit violation at 0x%lx\n", address);
        force_sig(SIGSEGV);
        return;
    }

    /*
     * 情况2: 内核空间页错误 (address >= TASK_SIZE_MAX)
     */
    if (unlikely(address >= TASK_SIZE_MAX)) {
        /*
         * 可能是vmalloc地址 - 需要更新内核页表
         */
        if (is_vmalloc_addr((void *)address)) {
            fault = vmalloc_fault(address);
            if (fault == 0)
                return;  // 成功处理
        }

        /*
         * 内核访问无效地址 - 内核恐慌或oops
         */
        if (user_mode(regs)) {
            // 用户程序通过系统调用传入坏指针
            force_sig(SIGSEGV);
        } else {
            // 内核bug
            die("Oops", regs, error_code);
        }
        return;
    }

    /*
     * 情况3: 用户空间页错误
     */
    if (user) {
        // 检查是否在内核中断上下文
        if (in_atomic() || !mm) {
            force_sig(SIGSEGV);
            return;
        }
    }

    /*
     * 查找对应的VMA (Virtual Memory Area)
     */
    struct vm_area_struct *vma = find_vma(mm, address);

    if (!vma) {
        // 地址不在任何VMA中 - 无效访问
        printk(KERN_DEBUG "No VMA for address 0x%lx\n", address);
        force_sig(SIGSEGV);
        return;
    }

    /*
     * 检查访问权限
     */
    if (write) {
        // 写访问
        if (!(vma->vm_flags & VM_WRITE)) {
            printk(KERN_DEBUG "Write to read-only region\n");
            force_sig(SIGSEGV);
            return;
        }
    } else if (instruction) {
        // 指令获取
        if (!(vma->vm_flags & VM_EXEC)) {
            printk(KERN_DEBUG "Execute from non-executable region\n");
            force_sig(SIGSEGV);
            return;
        }
    } else {
        // 读访问
        if (!(vma->vm_flags & (VM_READ | VM_EXEC))) {
            printk(KERN_DEBUG "Read from non-readable region\n");
            force_sig(SIGSEGV);
            return;
        }
    }

    /*
     * 处理缺页 (需求分页)
     */
    fault = handle_mm_fault(vma, address,
                            write ? FAULT_FLAG_WRITE : 0);

    if (fault & VM_FAULT_OOM) {
        // 内存不足
        pagefault_out_of_memory();
    } else if (fault & VM_FAULT_SIGBUS) {
        // 总线错误
        force_sig(SIGBUS);
    } else if (fault & VM_FAULT_SIGSEGV) {
        // 段错误
        force_sig(SIGSEGV);
    }

    /*
     * 成功处理，MMU TLB会在返回用户空间前自动更新
     * 指令会重新执行，这次会成功
     */
}

/*
 * 处理需求分页
 */
int handle_mm_fault(struct vm_area_struct *vma, unsigned long address,
                    unsigned int flags) {
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    /*
     * 获取或分配各级页表
     */
    pgd = pgd_offset(vma->vm_mm, address);
    p4d = p4d_alloc(vma->vm_mm, pgd, address);
    if (!p4d)
        return VM_FAULT_OOM;

    pud = pud_alloc(vma->vm_mm, p4d, address);
    if (!pud)
        return VM_FAULT_OOM;

    pmd = pmd_alloc(vma->vm_mm, pud, address);
    if (!pmd)
        return VM_FAULT_OOM;

    /*
     * 处理PTE级别的缺页
     * 这可能是:
     * 1. 匿名页 - 分配零页或复制COW页
     * 2. 文件映射 - 从文件读取页
     * 3. 共享内存 - 连接共享页
     */
    return handle_pte_fault(vma, address, pmd, flags);
}
```

## 7. Verilog实现框架

```verilog
//////////////////////////////////////////////////////////////////////////////
// Page Table Walker (页表遍历器) Verilog实现
// 支持x86-64风格四级页表，带PTE缓存
//////////////////////////////////////////////////////////////////////////////

`ifndef PAGE_TABLE_WALKER_V
`define PAGE_TABLE_WALKER_V

//////////////////////////////////////////////////////////////////////////////
// 参数定义
//////////////////////////////////////////////////////////////////////////////

`define VA_BITS         48
`define PA_BITS         52
`define PTE_BITS        64
`define PAGE_SHIFT      12
`define PT_LEVELS       4

// PTE标志位
`define PTE_P           0
`define PTE_RW          1
`define PTE_US          2
`define PTE_PWT         3
`define PTE_PCD         4
`define PTE_A           5
`define PTE_D           6
`define PTE_PS          7
`define PTE_G           8
`define PTE_NX          63

//////////////////////////////////////////////////////////////////////////////
// 模块: Page Table Walker
//////////////////////////////////////////////////////////////////////////////

module page_table_walker (
    input  wire                    clk,
    input  wire                    rst_n,

    // 控制接口
    output wire                    ptw_idle,
    input  wire                    ptw_start,
    input  wire [`VA_BITS-1:0]     ptw_vaddr,
    input  wire [`PA_BITS-1:0]     ptw_pml4_base,
    input  wire [1:0]              ptw_access_type,  // 00=读, 01=写, 10=执行
    input  wire                    ptw_user_mode,

    // 结果接口
    output reg                     ptw_done,
    output reg                     ptw_page_fault,
    output reg  [3:0]              ptw_fault_code,
    output reg  [`PA_BITS-1:0]     ptw_paddr,
    output reg  [7:0]              ptw_flags,

    // 内存接口 (用于页表遍历)
    output reg                     mem_req_valid,
    input  wire                    mem_req_ready,
    output reg  [`PA_BITS-1:0]     mem_req_addr,
    output reg  [2:0]              mem_req_size,
    output reg                     mem_req_cacheable,  // 页表访问通常是cacheable

    input  wire                    mem_resp_valid,
    output wire                    mem_resp_ready,
    input  wire [`PTE_BITS-1:0]    mem_resp_data,
    input  wire                    mem_resp_error,

    // PTE缓存控制
    input  wire                    ptc_enable,
    output reg                     ptc_update_en,
    output reg  [1:0]              ptc_update_level,
    output reg  [8:0]              ptc_update_index,
    output reg  [`PTE_BITS-1:0]    ptc_update_pte
);

    //////////////////////////////////////////////////////////////////////////
    // 状态机定义
    //////////////////////////////////////////////////////////////////////////

    localparam STATE_IDLE       = 4'd0;
    localparam STATE_CHECK_PML4_CACHE = 4'd1;
    localparam STATE_READ_PML4  = 4'd2;
    localparam STATE_CHECK_PDPT_CACHE = 4'd3;
    localparam STATE_READ_PDPT  = 4'd4;
    localparam STATE_CHECK_PD_CACHE   = 4'd5;
    localparam STATE_READ_PD    = 4'd6;
    localparam STATE_READ_PT    = 4'd7;
    localparam STATE_BUILD_PA   = 4'd8;
    localparam STATE_FAULT      = 4'd9;
    localparam STATE_DONE       = 4'd10;

    reg [3:0] state, next_state;

    //////////////////////////////////////////////////////////////////////////
    // 内部寄存器
    //////////////////////////////////////////////////////////////////////////

    // 虚拟地址分解
    reg [8:0]  pml4_index_reg;
    reg [8:0]  pdpt_index_reg;
    reg [8:0]  pd_index_reg;
    reg [8:0]  pt_index_reg;
    reg [11:0] page_offset_reg;

    // 访问信息
    reg [1:0]  access_type_reg;
    reg        user_mode_reg;

    // 当前页表项
    reg [`PTE_BITS-1:0] current_pte;
    reg [`PA_BITS-1:0]  next_table_base;

    // 页表基址
    reg [`PA_BITS-1:0] pml4_base_reg;

    // PTE缓存命中
    reg        ptc_hit;
    reg [`PTE_BITS-1:0] ptc_pte;

    //////////////////////////////////////////////////////////////////////////
    // 虚拟地址分解
    //////////////////////////////////////////////////////////////////////////

    wire [8:0]  pml4_index = ptw_vaddr[47:39];
    wire [8:0]  pdpt_index = ptw_vaddr[38:30];
    wire [8:0]  pd_index   = ptw_vaddr[29:21];
    wire [8:0]  pt_index   = ptw_vaddr[20:12];
    wire [11:0] page_offset = ptw_vaddr[11:0];

    //////////////////////////////////////////////////////////////////////////
    // PTE缓存接口 (简化模型)
    //////////////////////////////////////////////////////////////////////////

    // PTE缓存查找端口
    reg        ptc_lookup_en;
    reg [1:0]  ptc_lookup_level;
    reg [8:0]  ptc_lookup_index;

    // 使用简化PTE缓存
    wire ptc_hit_w;
    wire [`PTE_BITS-1:0] ptc_pte_w;

    pte_cache u_ptc (
        .clk(clk),
        .rst_n(rst_n),
        .lookup_en(ptc_lookup_en),
        .lookup_level(ptc_lookup_level),
        .lookup_index(ptc_lookup_index),
        .lookup_hit(ptc_hit_w),
        .lookup_pte(ptc_pte_w),
        .update_en(ptc_update_en),
        .update_level(ptc_update_level),
        .update_index(ptc_update_index),
        .update_pte(ptc_update_pte)
    );

    always @(posedge clk) begin
        ptc_hit <= ptc_hit_w;
        ptc_pte <= ptc_pte_w;
    end

    //////////////////////////////////////////////////////////////////////////
    // 状态机 - 时序逻辑
    //////////////////////////////////////////////////////////////////////////

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= STATE_IDLE;
        end else begin
            state <= next_state;
        end
    end

    //////////////////////////////////////////////////////////////////////////
    // 状态机 - 组合逻辑
    //////////////////////////////////////////////////////////////////////////

    always @(*) begin
        next_state = state;

        case (state)
            STATE_IDLE: begin
                if (ptw_start)
                    next_state = STATE_CHECK_PML4_CACHE;
            end

            STATE_CHECK_PML4_CACHE: begin
                if (ptc_hit)
                    next_state = STATE_CHECK_PDPT_CACHE;
                else
                    next_state = STATE_READ_PML4;
            end

            STATE_READ_PML4: begin
                if (mem_req_ready)
                    next_state = STATE_CHECK_PDPT_CACHE;
            end

            STATE_CHECK_PDPT_CACHE: begin
                if (check_pte_fault(current_pte)) begin
                    next_state = STATE_FAULT;
                end else if (is_large_page(current_pte, 2'd1)) begin  // 1GB页
                    next_state = STATE_BUILD_PA;
                end else if (ptc_hit) begin
                    next_state = STATE_CHECK_PD_CACHE;
                end else begin
                    next_state = STATE_READ_PDPT;
                end
            end

            STATE_READ_PDPT: begin
                if (mem_req_ready)
                    next_state = STATE_CHECK_PD_CACHE;
            end

            STATE_CHECK_PD_CACHE: begin
                if (check_pte_fault(current_pte)) begin
                    next_state = STATE_FAULT;
                end else if (is_large_page(current_pte, 2'd2)) begin  // 2MB页
                    next_state = STATE_BUILD_PA;
                end else if (ptc_hit) begin
                    next_state = STATE_READ_PT;
                end else begin
                    next_state = STATE_READ_PD;
                end
            end

            STATE_READ_PD: begin
                if (mem_req_ready)
                    next_state = STATE_READ_PT;
            end

            STATE_READ_PT: begin
                if (mem_req_ready)
                    next_state = STATE_BUILD_PA;
            end

            STATE_BUILD_PA: begin
                if (check_pte_fault(current_pte))
                    next_state = STATE_FAULT;
                else
                    next_state = STATE_DONE;
            end

            STATE_FAULT: begin
                next_state = STATE_DONE;
            end

            STATE_DONE: begin
                next_state = STATE_IDLE;
            end

            default: next_state = STATE_IDLE;
        endcase
    end

    //////////////////////////////////////////////////////////////////////////
    // 数据通路
    //////////////////////////////////////////////////////////////////////////

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            pml4_index_reg <= 0;
            pdpt_index_reg <= 0;
            pd_index_reg <= 0;
            pt_index_reg <= 0;
            page_offset_reg <= 0;
            access_type_reg <= 0;
            user_mode_reg <= 0;
            pml4_base_reg <= 0;
        end else if (ptw_start) begin
            // 启动时锁存输入
            pml4_index_reg <= pml4_index;
            pdpt_index_reg <= pdpt_index;
            pd_index_reg <= pd_index;
            pt_index_reg <= pt_index;
            page_offset_reg <= page_offset;
            access_type_reg <= ptw_access_type;
            user_mode_reg <= ptw_user_mode;
            pml4_base_reg <= ptw_pml4_base;
        end
    end

    // 内存响应处理
    always @(posedge clk) begin
        if (mem_resp_valid) begin
            current_pte <= mem_resp_data;
        end else if (ptc_hit) begin
            current_pte <= ptc_pte;
        end
    end

    //////////////////////////////////////////////////////////////////////////
    // 输出逻辑
    //////////////////////////////////////////////////////////////////////////

    assign ptw_idle = (state == STATE_IDLE);
    assign mem_resp_ready = 1'b1;

    // 内存请求
    always @(*) begin
        mem_req_valid = 1'b0;
        mem_req_addr = 0;
        mem_req_size = 3'b011;  // 8字节
        mem_req_cacheable = 1'b1;

        case (state)
            STATE_READ_PML4: begin
                mem_req_valid = 1'b1;
                mem_req_addr = pml4_base_reg + (pml4_index_reg << 3);
            end

            STATE_READ_PDPT: begin
                mem_req_valid = 1'b1;
                mem_req_addr = next_table_base + (pdpt_index_reg << 3);
            end

            STATE_READ_PD: begin
                mem_req_valid = 1'b1;
                mem_req_addr = next_table_base + (pd_index_reg << 3);
            end

            STATE_READ_PT: begin
                mem_req_valid = 1'b1;
                mem_req_addr = next_table_base + (pt_index_reg << 3);
            end
        endcase
    end

    // PTE缓存查找控制
    always @(*) begin
        ptc_lookup_en = 1'b0;
        ptc_lookup_level = 0;
        ptc_lookup_index = 0;

        case (state)
            STATE_CHECK_PML4_CACHE: begin
                ptc_lookup_en = ptc_enable;
                ptc_lookup_level = 2'd0;
                ptc_lookup_index = pml4_index_reg;
            end

            STATE_CHECK_PDPT_CACHE: begin
                ptc_lookup_en = ptc_enable;
                ptc_lookup_level = 2'd1;
                ptc_lookup_index = pdpt_index_reg;
            end

            STATE_CHECK_PD_CACHE: begin
                ptc_lookup_en = ptc_enable;
                ptc_lookup_level = 2'd2;
                ptc_lookup_index = pd_index_reg;
            end
        endcase
    end

    // 下一级表基址计算
    always @(posedge clk) begin
        if (mem_resp_valid || ptc_hit) begin
            next_table_base <= current_pte[51:12] << 12;
        end
    end

    // PTE缓存更新
    always @(posedge clk) begin
        ptc_update_en <= 1'b0;

        case (state)
            STATE_READ_PML4: begin
                if (mem_resp_valid && !mem_resp_error) begin
                    ptc_update_en <= ptc_enable;
                    ptc_update_level <= 2'd0;
                    ptc_update_index <= pml4_index_reg;
                    ptc_update_pte <= mem_resp_data;
                end
            end

            STATE_READ_PDPT: begin
                if (mem_resp_valid && !mem_resp_error) begin
                    ptc_update_en <= ptc_enable;
                    ptc_update_level <= 2'd1;
                    ptc_update_index <= pdpt_index_reg;
                    ptc_update_pte <= mem_resp_data;
                end
            end

            STATE_READ_PD: begin
                if (mem_resp_valid && !mem_resp_error) begin
                    ptc_update_en <= ptc_enable;
                    ptc_update_level <= 2'd2;
                    ptc_update_index <= pd_index_reg;
                    ptc_update_pte <= mem_resp_data;
                end
            end
        endcase
    end

    // 结果输出
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ptw_done <= 1'b0;
            ptw_page_fault <= 1'b0;
        end else begin
            case (state)
                STATE_BUILD_PA: begin
                    ptw_done <= 1'b1;
                    ptw_page_fault <= 1'b0;

                    // 构建物理地址
                    case (1'b1)
                        // 1GB页
                        is_large_page(current_pte, 2'd1): begin
                            ptw_paddr <= {current_pte[51:30], ptw_vaddr[29:0]};
                        end
                        // 2MB页
                        is_large_page(current_pte, 2'd2): begin
                            ptw_paddr <= {current_pte[51:21], ptw_vaddr[20:0]};
                        end
                        // 4KB页
                        default: begin
                            ptw_paddr <= {current_pte[51:12], page_offset_reg};
                        end
                    endcase

                    ptw_flags <= current_pte[7:0];
                end

                STATE_FAULT: begin
                    ptw_done <= 1'b1;
                    ptw_page_fault <= 1'b1;
                    ptw_fault_code <= generate_fault_code(
                        current_pte, access_type_reg, user_mode_reg
                    );
                end

                default: begin
                    ptw_done <= 1'b0;
                end
            endcase
        end
    end

    //////////////////////////////////////////////////////////////////////////
    // 辅助函数
    //////////////////////////////////////////////////////////////////////////

    // 检查页错误
    function check_pte_fault;
        input [`PTE_BITS-1:0] pte;
        begin
            check_pte_fault = (pte[`PTE_P] == 1'b0);
        end
    endfunction

    // 检查是否为大页
    function is_large_page;
        input [`PTE_BITS-1:0] pte;
        input [1:0] level;  // 1=PDPT(1GB), 2=PD(2MB)
        begin
            is_large_page = (pte[`PTE_PS] == 1'b1);
        end
    endfunction

    // 生成错误码
    function [3:0] generate_fault_code;
        input [`PTE_BITS-1:0] pte;
        input [1:0] access;  // 00=读, 01=写, 10=执行
        input user;
        begin
            generate_fault_code[0] = pte[`PTE_P];        // P位
            generate_fault_code[1] = access[0];          // W/R
            generate_fault_code[2] = user;               // U/S
            generate_fault_code[3] = access[1];          // I/D
        end
    endfunction

endmodule

//////////////////////////////////////////////////////////////////////////////
// 模块: PTE Cache (页表缓存)
//////////////////////////////////////////////////////////////////////////////

module pte_cache #(
    parameter PML4_ENTRIES = 8,
    parameter PDPT_ENTRIES = 16,
    parameter PD_ENTRIES = 32
)(
    input  wire                    clk,
    input  wire                    rst_n,

    // 查找端口
    input  wire                    lookup_en,
    input  wire [1:0]              lookup_level,  // 0=PML4, 1=PDPT, 2=PD
    input  wire [8:0]              lookup_index,
    output reg                     lookup_hit,
    output reg  [63:0]             lookup_pte,

    // 更新端口
    input  wire                    update_en,
    input  wire [1:0]              update_level,
    input  wire [8:0]              update_index,
    input  wire [63:0]             update_pte
);

    // PML4缓存
    reg [8:0]  pml4_tag    [0:PML4_ENTRIES-1];
    reg [63:0] pml4_pte    [0:PML4_ENTRIES-1];
    reg        pml4_valid  [0:PML4_ENTRIES-1];
    reg [7:0]  pml4_lru    [0:PML4_ENTRIES-1];

    // PDPT缓存
    reg [8:0]  pdpt_tag    [0:PDPT_ENTRIES-1];
    reg [63:0] pdpt_pte    [0:PDPT_ENTRIES-1];
    reg        pdpt_valid  [0:PDPT_ENTRIES-1];
    reg [7:0]  pdpt_lru    [0:PDPT_ENTRIES-1];

    // PD缓存
    reg [8:0]  pd_tag      [0:PD_ENTRIES-1];
    reg [63:0] pd_pte      [0:PD_ENTRIES-1];
    reg        pd_valid    [0:PD_ENTRIES-1];
    reg [7:0]  pd_lru      [0:PD_ENTRIES-1];

    integer i;

    // 初始化
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (i = 0; i < PML4_ENTRIES; i = i + 1)
                pml4_valid[i] <= 1'b0;
            for (i = 0; i < PDPT_ENTRIES; i = i + 1)
                pdpt_valid[i] <= 1'b0;
            for (i = 0; i < PD_ENTRIES; i = i + 1)
                pd_valid[i] <= 1'b0;
        end
    end

    // 查找逻辑
    always @(*) begin
        lookup_hit = 1'b0;
        lookup_pte = 64'd0;

        case (lookup_level)
            2'd0: begin  // PML4
                for (i = 0; i < PML4_ENTRIES; i = i + 1) begin
                    if (pml4_valid[i] && pml4_tag[i] == lookup_index) begin
                        lookup_hit = 1'b1;
                        lookup_pte = pml4_pte[i];
                    end
                end
            end

            2'd1: begin  // PDPT
                for (i = 0; i < PDPT_ENTRIES; i = i + 1) begin
                    if (pdpt_valid[i] && pdpt_tag[i] == lookup_index) begin
                        lookup_hit = 1'b1;
                        lookup_pte = pdpt_pte[i];
                    end
                end
            end

            2'd2: begin  // PD
                for (i = 0; i < PD_ENTRIES; i = i + 1) begin
                    if (pd_valid[i] && pd_tag[i] == lookup_index) begin
                        lookup_hit = 1'b1;
                        lookup_pte = pd_pte[i];
                    end
                end
            end
        endcase
    end

    // 更新逻辑
    always @(posedge clk) begin
        if (update_en) begin
            case (update_level)
                2'd0: begin  // PML4
                    // 查找空闲或使用LRU替换
                    // 简化实现: 使用轮转
                    // 实际应使用LRU算法
                end
                // 类似处理其他级别...
            endcase
        end
    end

endmodule

`endif // PAGE_TABLE_WALKER_V
```

## 8. 总结

### 8.1 关键概念回顾

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                         页表遍历器关键概念总结                                           │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   核心概念                                                                       │  │
│   │   ─────────                                                                       │  │
│   │                                                                                 │  │
│   │   1. 页表遍历是TLB未命中时执行的硬件操作                                          │  │
│   │   2. 多级页表结构将虚拟地址分层映射到物理地址                                     │  │
│   │   3. 硬件遍历比软件遍历快5-10倍，且对操作系统透明                                 │  │
│   │   4. PTE缓存可以减少内存访问次数，加速遍历                                        │  │
│   │   5. 大页(2MB/1GB)可以减少页表层级，提高TLB效率                                   │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   性能优化技术                                                                   │  │
│   │   ─────────────                                                                   │  │
│   │                                                                                 │  │
│   │   • PTE缓存 - 缓存中间级页表项                                                   │  │
│   │   • 页表预取 - 预测并预取可能需要的页表项                                         │  │
│   │   • 投机性遍历 - 提前开始遍历，等待地址可用                                       │  │
│   │   • 并行遍历 - 同时发起多个内存请求                                               │  │
│   │   • 大页支持 - 减少页表层级                                                       │  │
│   │                                                                                 │  │
│   └─────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                         │
│   ┌─────────────────────────────────────────────────────────────────────────────────┐  │
│   │   与C语言/操作系统的关系                                                          │  │
│   │   ──────────────────────                                                          │  │
│   │                                                                                 │  │
│   │   • 操作系统负责创建和管理页表                                                    │  │
│   │   • 页错误处理程序是C代码，处理硬件检测到的缺页                                   │  │
│   │   • mmap/mprotect等系统调用修改页表                                               │  │
│   │   • 用户程序通过虚拟地址访问内存，完全透明                                        │  │
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
  - `01_MMU_Architecture.md` - MMU架构概述
  - `03_TLB_Implementation.md` - TLB实现
  - `04_Memory_Protection_Unit.md` - 内存保护单元


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
